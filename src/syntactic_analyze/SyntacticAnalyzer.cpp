#include "SyntacticAnalyzer.h"

#include <stdexcept>
#include <algorithm>


SyntacticAnalyzer::SyntacticAnalyzer(const vector<Token>& tokenList)
	:tokenList{ tokenList }
{
}

SyntacticAnalyzer::~SyntacticAnalyzer()
{
	if (this->root != nullptr) delete root;
}

void SyntacticAnalyzer::analyze()
{
	this->root = match_start_struct_type();
}

SyntacticTreeNode* SyntacticAnalyzer::getSyntaxTree()
{
	return this->root;
}

bool SyntacticAnalyzer::isSyntacticError() const
{
	return this->isError;
}

const vector<string>& SyntacticAnalyzer::getErrorMsgs()
{
	return this->errorMsgs;
}

void SyntacticAnalyzer::addAnalyzerError(const std::string& errorMessage)
{
	throw std::out_of_range("SyntacticAnalyzer: " + errorMessage);
}

void SyntacticAnalyzer::addCodeError(const std::string& errorMessage, int lineNum, const std::string& errorTokens)
{
	this->isError = true;
	this->needResumeRuning = true;
	this->errorMsgs.push_back("SyntacticAnalyzer: " + to_string(lineNum) + " >  " + "\"" + errorTokens + "\" : " + errorMessage);
}

Token SyntacticAnalyzer::getToken(int pos)
{
	int tarPos = this->nowTokenNum + pos;
	if (tarPos < (int) this->tokenList.size()) {
		Token token = this->tokenList.at(tarPos);
		transform(token.value.begin(), token.value.end(), token.value.begin(), ::tolower);
		return token;
	}
	else {
		addAnalyzerError("预读取单词超出范围");
		return Token("", Token::TokenType::kUnknown);
	}
}

bool SyntacticAnalyzer::toNextToken()
{
	int tarPos = this->nowTokenNum + 1;
	if (tarPos < (int) this->tokenList.size()) {
		this->nowTokenNum = tarPos;
		return true;
	}
	return false;
}

Token SyntacticAnalyzer::match(Token::TokenType tokenType)
{
	Token nowToken = getToken();
	if (nowToken.type == tokenType) {
		toNextToken();
		return nowToken;
	}
	else {
		addCodeError(nowToken.getStringType() + " 单词类型不匹配 " + Token::changeTypeToString(tokenType), nowToken.tokenLine, nowToken.value);
		return Token("ERROR", Token::TokenType::kUnknown);
	}
}

Token SyntacticAnalyzer::match(const std::string& tokenValue)
{
	Token nowToken = getToken();
	if (nowToken.value == tokenValue) {
		toNextToken();
		return nowToken;
	}
	else {
		addCodeError("单词不匹配 " + tokenValue, nowToken.tokenLine, nowToken.value);
		return Token("ERROR", Token::TokenType::kUnknown);
	}
}

Token SyntacticAnalyzer::match(Token::TokenType tokenType, const std::string& tokenValue)
{	
	Token nowToken = getToken();
	if (nowToken.value == tokenValue && nowToken.type == tokenType) {
		toNextToken();
		return nowToken;
	}
	else {
		addCodeError("单词及其类型不匹配 " + tokenValue + "(" + Token::changeTypeToString(tokenType) + ")", nowToken.tokenLine, nowToken.value);
		return Token("ERROR", Token::TokenType::kUnknown);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_start_struct_type()
{
	SyntacticTreeNode* node = match_struct_type();
	match(Token::TokenType::kEOF);
	return node;
}

SyntacticTreeNode* SyntacticAnalyzer::match_struct_type()
{
	match(Token::TokenType::kKeyWord, KeyWord::kStruct);
	Token id = match(Token::TokenType::kIdentifier);
	match(Token::TokenType::kSign, Sign::kLBrace);
	SyntacticTreeNode* memberListNode = match_member_list();
	match(Token::TokenType::kSign, Sign::kRBrace);

	if (isSyntacticError()) {
		if (memberListNode != nullptr) delete memberListNode;
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeStructNode(id.value, memberListNode);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_member_list()
{
	vector<SyntacticTreeNode*> members;
	while (getToken().value != Sign::kRBrace && getToken().type != Token::TokenType::kEOF) {
		SyntacticTreeNode* memberType = match_type_spec();
		SyntacticTreeNode* memberDec = match_declarators();
        resumeRunning(); // 出错后定位到member_list下一条语句
		match(Token::TokenType::kSign, Sign::kSemicolon);

		if (isSyntacticError()) {
			if (memberType != nullptr) delete memberType;
			if (memberDec != nullptr) delete memberDec;
		}
		else {
			members.push_back(SyntacticTreeNode::makeMemberNode(memberType, memberDec));
		}
	}

	if (isSyntacticError()) {
		for (SyntacticTreeNode* node : members) delete node;
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeMemberListNode(members);
	}
}

void SyntacticAnalyzer::resumeRunning()
{
	if (this->needResumeRuning) {
		this->needResumeRuning = false;

		// 找到；后退出
		while (isSyntacticError() && getToken().value != Sign::kSemicolon) {
			toNextToken();
			if (getToken().type == Token::TokenType::kEOF) {
				// 没找到，退出
				break;
			}
			else if (getToken().value == Sign::kLBrace) {
				// 找到可匹配项，匹配后退出
				match(Token::TokenType::kSign, Sign::kLBrace);
				match_member_list();
				match(Token::TokenType::kSign, Sign::kRBrace);
				match_declarators();
				break;
			}
		}
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_type_spec()
{
	SyntacticTreeNode* node = nullptr;
	if (getToken().value == KeyWord::kStruct) {
		node = match_struct_type();
	}
	else {
		node = match_base_type_spec();
	}

	if (isSyntacticError()) {
		if (node != nullptr) delete node;
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeTypeSpecNode(node);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_base_type_spec()
{
	if (getToken().value == KeyWord::kChar) {
		Token t = match(Token::TokenType::kKeyWord, KeyWord::kChar);
		if (isSyntacticError()) {
			return nullptr;
		}
		else {
			return SyntacticTreeNode::makeBaseTypeSpecNode(t.value);
		}
	}
	else if (getToken().value == KeyWord::kBoolean) {
		Token t = match(Token::TokenType::kKeyWord, KeyWord::kBoolean);
		if (isSyntacticError()) {
			return nullptr;
		}
		else {
			return SyntacticTreeNode::makeBaseTypeSpecNode(t.value);
		}
	}
	else {
		SyntacticTreeNode* node = nullptr;
		if (getToken().value == KeyWord::kFloat) {
			node = match_floating_pt_type();
		}
		else if (getToken().value == KeyWord::kDouble) {
			node = match_floating_pt_type();
		}
		else if (getToken().value == KeyWord::kLong && getToken(1).value == KeyWord::kDouble) {
			node = match_floating_pt_type();
		}
		else {
			node = match_integer_type();
		}

		if (isSyntacticError()) {
			if (node != nullptr) delete node;
			return nullptr;
		}
		else {
			return SyntacticTreeNode::makeBaseTypeSpecNode(node);
		}
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_floating_pt_type()
{
	string type;
	if (getToken().value == KeyWord::kFloat) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kFloat).value;
	}
	else if (getToken().value == KeyWord::kDouble) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kDouble).value;
	}
	else if (getToken().value == KeyWord::kLong && getToken(1).value == KeyWord::kDouble) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kLong).value;
		type.push_back(' ');
		type += match(Token::TokenType::kKeyWord, KeyWord::kDouble).value;
	}
	else {
		addCodeError("floating_pt_type不匹配", getToken().tokenLine, getToken().value);
		if (getToken().type != Token::TokenType::kIdentifier) {
			toNextToken();
		}
		return nullptr;
	}
	if (isSyntacticError()) {
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeFloatingPtTypeNode(type);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_integer_type()
{
	SyntacticTreeNode* node = nullptr;
	if (getToken().value.at(0) == 'u') {
		node = match_unsigned_int();
	}
	else {
		node = match_signed_int();
	}

	if (isSyntacticError()) {
		if (node != nullptr) delete node;
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeIntegerTypeNode(node);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_signed_int()
{
	string type;
	if (getToken().value == KeyWord::kShort) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kShort).value;
	}
	else if (getToken().value == KeyWord::kInt16) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kInt16).value;
	}
	else if (getToken().value == KeyWord::kLong && getToken(1).value == KeyWord::kLong) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kLong).value;
		type.push_back(' ');
		type += match(Token::TokenType::kKeyWord, KeyWord::kLong).value;
	}
	else if (getToken().value == KeyWord::kInt64) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kInt64).value;
	}
	else if (getToken().value == KeyWord::kLong) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kLong).value;
	}
	else if (getToken().value == KeyWord::kInt32) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kInt32).value;
	}
	else if (getToken().value == KeyWord::kInt8) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kInt8).value;
	}
	else {
		addCodeError("type_spec不匹配", getToken().tokenLine, getToken().value);
		if (getToken().type != Token::TokenType::kIdentifier) {
			toNextToken();
		}
		return nullptr;
	}

	if (isSyntacticError()) {
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeSignedIntNode(type);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_unsigned_int()
{
	string type;
	if (getToken().value == KeyWord::kUnsigned) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kUnsigned).value;
		type.push_back(' ');

		if (getToken().value == KeyWord::kShort) {
			type += match(Token::TokenType::kKeyWord, KeyWord::kShort).value;
		}
		else if (getToken().value == KeyWord::kLong && getToken(1).value == KeyWord::kLong) {
			type += match(Token::TokenType::kKeyWord, KeyWord::kLong).value;
			type.push_back(' ');
			type += match(Token::TokenType::kKeyWord, KeyWord::kLong).value;
		}
		else if (getToken().value == KeyWord::kLong) {
			type += match(Token::TokenType::kKeyWord, KeyWord::kLong).value;
		}
		else {
			addCodeError("unsigned_int不匹配", getToken().tokenLine, getToken().value);
			if (getToken().type != Token::TokenType::kIdentifier) {
				toNextToken();
			}
			return nullptr;
		}
	}
	else if (getToken().value == KeyWord::kUnit16) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kUnit16).value;
	}
	else if (getToken().value == KeyWord::kUnit32) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kUnit32).value;
	}
	else if (getToken().value == KeyWord::kUnit64) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kUnit64).value;
	}
	else if (getToken().value == KeyWord::kUnit8) {
		type = match(Token::TokenType::kKeyWord, KeyWord::kUnit8).value;
	}
	else {
		addCodeError("type_spec不匹配", getToken().tokenLine, getToken().value);
		if (getToken().type != Token::TokenType::kIdentifier) {
			toNextToken();
		}
		return nullptr;
	}

	if (isSyntacticError()) {
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeUnsignedIntNode(type);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_declarators()
{
	SyntacticTreeNode* node = match_declarator();
	vector<SyntacticTreeNode*> decs;
	while (getToken().value == Sign::kComma) {
		match(Token::TokenType::kSign, Sign::kComma);
		SyntacticTreeNode* sNode = match_declarator();
		decs.push_back(sNode);
	}
	if (isSyntacticError()) {
		if (node != nullptr) delete node;
		for (SyntacticTreeNode* n : decs) {
			if (n != nullptr) delete n;
		}
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeDeclaratorsNode(node, decs);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_declarator()
{
	string id = match(Token::TokenType::kIdentifier).value;
	SyntacticTreeNode* node = nullptr;
	if (getToken().value == Sign::kLSquareBracket) {
		node = match_exp_list();
	}
	if (isSyntacticError()) {
		if (node != nullptr) delete node;
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeDeclaratorNode(id, node);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_exp_list()
{
	match(Token::TokenType::kSign, Sign::kLSquareBracket);
	SyntacticTreeNode* node = match_or_expr();
	vector<SyntacticTreeNode*> orExprs;
	while (getToken().value == Sign::kComma) {
		match(Token::TokenType::kSign, Sign::kComma);
		SyntacticTreeNode* sNode = match_or_expr();
		orExprs.push_back(sNode);
	}
	match(Token::TokenType::kSign, Sign::kRSquareBracket);

	if (isSyntacticError()) {
		if (node != nullptr) delete node;
		for (SyntacticTreeNode* n : orExprs) {
			if (n != nullptr) delete n;
		}
		return nullptr;
	}
	else {
		return SyntacticTreeNode::makeExpListNode(node, orExprs);
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_or_expr()
{
	SyntacticTreeNode* node = match_xor_expr();
	vector<string> types;
	vector<SyntacticTreeNode*> nodes;
	while (getToken().value == Sign::kOr) {
		types.push_back(match(Token::TokenType::kSign, Sign::kOr).value);
		nodes.push_back(match_xor_expr());
	}

	return makeExprResult(node, nodes, SyntacticTreeNode::NodeType::kOrExpr);
}

SyntacticTreeNode* SyntacticAnalyzer::match_xor_expr()
{
	SyntacticTreeNode* node = match_and_expr();
	vector<string> types;
	vector<SyntacticTreeNode*> nodes;
	while (getToken().value == Sign::kSuperscript) {
		types.push_back(match(Token::TokenType::kSign, Sign::kSuperscript).value);
		nodes.push_back(match_and_expr());
	}

	return makeExprResult(node, nodes, SyntacticTreeNode::NodeType::kXorExpr);
}

SyntacticTreeNode* SyntacticAnalyzer::makeExprResult(SyntacticTreeNode* node, std::vector<SyntacticTreeNode*>& nodes, SyntacticTreeNode::NodeType type) const
{
	if (isSyntacticError()) {
		if (node != nullptr) delete node;
		for (SyntacticTreeNode* n : nodes) {
			if (n != nullptr) delete n;
		}
		return nullptr;
	}
	else {
		SyntacticTreeNode* root = SyntacticTreeNode::makeExprNode(type, node);
		if (!nodes.empty()) {
			SyntacticTreeNode* now = root;
			for (int i = 0; i < (int) nodes.size(); i++) {
				SyntacticTreeNode* next = SyntacticTreeNode::makeExprNode(type, nodes.at(i));
				now->addChild(next);
				now = next;
			}
		}
		return root;
	}
}

SyntacticTreeNode* SyntacticAnalyzer::match_and_expr()
{
	SyntacticTreeNode* node = match_shift_expr();
	vector<string> types;
	vector<SyntacticTreeNode*> nodes;
	while (getToken().value == Sign::kAnd) {
		types.push_back(match(Token::TokenType::kSign, Sign::kAnd).value);
		nodes.push_back(match_shift_expr());
	}
	return makeExprResult(node, nodes, SyntacticTreeNode::NodeType::kAndExpr);
}

SyntacticTreeNode* SyntacticAnalyzer::match_shift_expr()
{
	SyntacticTreeNode* node = match_add_expr();
	vector<string> types;
	vector<SyntacticTreeNode*> nodes;
	while (getToken().value == Sign::kRightShift || getToken().value == Sign::kLeftShift) {
		types.push_back(match(Token::TokenType::kSign).value);
		nodes.push_back(match_add_expr());
	}
	return makeExprResult(node, nodes, SyntacticTreeNode::NodeType::kShiftExpr);
}

SyntacticTreeNode* SyntacticAnalyzer::match_add_expr()
{
	SyntacticTreeNode* node = match_mult_expr();
	vector<string> types;
	vector<SyntacticTreeNode*> nodes;
	while (getToken().value == Sign::kPlus || getToken().value == Sign::kMinus) {
		types.push_back(match(Token::TokenType::kSign).value);
		nodes.push_back(match_mult_expr());
	}
	return makeExprResult(node, nodes, SyntacticTreeNode::NodeType::kAddExpr);
}

SyntacticTreeNode* SyntacticAnalyzer::match_mult_expr()
{
	SyntacticTreeNode* node = match_unary_expr();
	vector<string> types;
	vector<SyntacticTreeNode*> nodes;
	while (getToken().value == Sign::kMultiply || getToken().value == Sign::kDivision || getToken().value == Sign::kPercent) {
		types.push_back(match(Token::TokenType::kSign).value);
		nodes.push_back(match_unary_expr());
	}
	return makeExprResult(node, nodes, SyntacticTreeNode::NodeType::kMultExpr);
}

SyntacticTreeNode* SyntacticAnalyzer::match_unary_expr()
{
	string sign;
	if (getToken().value == Sign::kPlus || getToken().value == Sign::kMinus || getToken().value == Sign::kWavyLine) {
		sign = match(Token::TokenType::kSign).value;
	}
	Token t;
	if (getToken().type == Token::TokenType::kInteger) {
		t = match(Token::TokenType::kInteger);
	}
	else if (getToken().type == Token::TokenType::kString) {
		t = match(Token::TokenType::kString);
	}
	else if (getToken().type == Token::TokenType::kBoolean) {
		t = match(Token::TokenType::kBoolean);
	}
	else {
		addCodeError("unary_expr不匹配", getToken().tokenLine, getToken().value);
		if (getToken().type != Token::TokenType::kSign) {
			toNextToken();
		}
		return nullptr;
	}

	if (isSyntacticError()) {
		return nullptr;
	}
	else {
		if (!sign.empty()) {
			return SyntacticTreeNode::makeUnaryExpr(t, sign);
		}
		else {
			return SyntacticTreeNode::makeUnaryExpr(t);
		}
	}
}

string SyntacticTreeNode::changeNodeTypeToString(NodeType type)
{
	switch (type) {
	case NodeType::kStruct:
		return "struct_type";
	case NodeType::kMemberList:
		return "member_list";
	case NodeType::kMember:
		return "member";
	case NodeType::kTypeSpec:
		return "type_spec";
	case NodeType::kBaseTypeSpec:
		return "base_type_spec";
	case NodeType::kFloatingPtType:
		return "floating_pt_type";
	case NodeType::kIntegerType:
		return "interger_type";
	case NodeType::kSignedInt:
		return "signed_int";
	case NodeType::kUnsignedInt:
		return "unsigned_int";
	case NodeType::kDeclarators:
		return "declarators";
	case NodeType::kDeclarator:
		return "declarator";
	case NodeType::kExpList:
		return "exp_list";
	case NodeType::kOrExpr:
		return "or_expr";
	case NodeType::kXorExpr:
		return "xor_expr";
	case NodeType::kAndExpr:
		return "and_expr";
	case NodeType::kShiftExpr:
		return "shift_expr";
	case NodeType::kAddExpr:
		return "add_expr";
	case NodeType::kMultExpr:
		return "mult_expr";
	case NodeType::kUnaryExpr:
		return "unary_expr";
	default:
		return "Unknown";
	}
}

SyntacticTreeNode::SyntacticTreeNode(const SyntacticTreeNode& node)
{
	this->nodeType = node.nodeType;
	this->children = node.children;
	//this->minChildrenNum = node.minChildrenNum;
	//this->maxChildrenNum = node.maxChildrenNum;
	this->info = new string(*node.info);
	this->value = new Token(*node.value);
}

SyntacticTreeNode::SyntacticTreeNode(NodeType nodeType)
{
	this->nodeType = nodeType;
}

SyntacticTreeNode::~SyntacticTreeNode()
{
	if (info != nullptr) {
		delete info;
		info = nullptr;
	}
	if (value != nullptr) {
		delete value;
		value = nullptr;
	}

	for (SyntacticTreeNode* node : this->children) {
		if (node != nullptr)
			delete node;
	}
}

void SyntacticTreeNode::addChild(SyntacticTreeNode* node)
{
	this->children.push_back(node);
}

Token SyntacticTreeNode::getValue()
{
	if (this->value != nullptr) {
		return *this->value;
	}
	else {
		return Token();
	}
}

string SyntacticTreeNode::getInfo()
{
	if (this->info != nullptr) {
		return *this->info;
	}
	else {
		return "";
	}
}

const vector<SyntacticTreeNode*>& SyntacticTreeNode::getChildren()
{
	return this->children;
}

SyntacticTreeNode::NodeType SyntacticTreeNode::getNodeType()
{
	return this->nodeType;
}

vector<string> SyntacticTreeNode::printTree(SyntacticTreeNode* root)
{
	vector<string> tree;
	tranTree(root, tree, 0);
	return tree;
}

void SyntacticTreeNode::tranTree(SyntacticTreeNode* now, vector<string>& printer, int deepth)
{
	string data;
	for (int i = 0; i < deepth; i++) data.push_back('\t');
	data.append(SyntacticTreeNode::changeNodeTypeToString(now->getNodeType()));
	string info = now->getInfo();
	Token value = now->getValue();
	if (!info.empty()) data.append(" " + info);
	if (value.type != Token::TokenType::kUnknown) data.append(" " + value.value + "(" + value.getStringType() + ")");
	printer.push_back(data);

	for (SyntacticTreeNode* node : now->getChildren()) {
		tranTree(node, printer, deepth + 1);
	}
}

SyntacticTreeNode* SyntacticTreeNode::makeStructNode(string identifier, SyntacticTreeNode* memberListNode)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kStruct);
	node->info = new string(std::move(identifier));
	node->children.push_back(memberListNode);
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeMemberListNode(vector<SyntacticTreeNode*> memberNodes)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kMemberList);
	node->children = memberNodes;
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeMemberNode(SyntacticTreeNode* typeSpecNode, SyntacticTreeNode* declaratorsNode)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kMember);
	node->children.push_back(typeSpecNode);
	node->children.push_back(declaratorsNode);
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeTypeSpecNode(SyntacticTreeNode* typeNode)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kTypeSpec);
	node->children.push_back(typeNode);
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeBaseTypeSpecNode(string type)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kBaseTypeSpec);
	node->info = new string(std::move(type));
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeBaseTypeSpecNode(SyntacticTreeNode* typeNode)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kBaseTypeSpec);
	node->children.push_back(typeNode);
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeFloatingPtTypeNode(string type)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kFloatingPtType);
	node->info = new string(std::move(type));
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeIntegerTypeNode(SyntacticTreeNode* intTypeNode)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kIntegerType);
	node->children.push_back(intTypeNode);
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeSignedIntNode(string intType)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kSignedInt);
	node->info = new string(std::move(intType));
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeUnsignedIntNode(string uintType)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kUnsignedInt);
	node->info = new string(std::move(uintType));
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeDeclaratorsNode(SyntacticTreeNode* declarator1, vector<SyntacticTreeNode*> declarators)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kDeclarators);
	node->children.push_back(declarator1);
	for (SyntacticTreeNode* sNode : declarators) {
		node->children.push_back(sNode);
	}
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeDeclaratorNode(string identifier, SyntacticTreeNode* expList)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kDeclarator);
	node->info = new string(std::move(identifier));
	if (expList != nullptr) {
		node->children.push_back(expList);
	}
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeExpListNode(SyntacticTreeNode* orExpr1, vector<SyntacticTreeNode*> orExprs)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kExpList);
	node->children.push_back(orExpr1);
	for (SyntacticTreeNode* sNode : orExprs) {
		node->children.push_back(sNode);
	}
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeExprNode(NodeType nodeType, SyntacticTreeNode* node1)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(nodeType);
	node->children.push_back(node1);
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeExprNode(NodeType nodeType, SyntacticTreeNode* node1, SyntacticTreeNode* node2, string sign)
{
	SyntacticTreeNode* node = makeExprNode(nodeType, node1);
	node->children.push_back(node2);
	node->info = new string(std::move(sign));
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeUnaryExpr(Token value, string sign)
{
	SyntacticTreeNode* node = makeUnaryExpr(std::move(value));
	node->info = new string(std::move(sign));
	return node;
}

SyntacticTreeNode* SyntacticTreeNode::makeUnaryExpr(Token value)
{
	SyntacticTreeNode* node = new SyntacticTreeNode(NodeType::kUnaryExpr);
	node->value = new Token(std::move(value));
	return node;
}

