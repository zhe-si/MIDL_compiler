#pragma once

#include <vector>

#include "LexicalAnalyzer.h"


using namespace std;


/// <summary>
/// 抽象语法树节点
/// </summary>
class SyntacticTreeNode
{
public:
	enum class NodeType {
		kStruct,
		kMemberList,
		kMember,
		kTypeSpec,
		kBaseTypeSpec,
		kFloatingPtType,
		kIntegerType,
		kSignedInt,
		kUnsignedInt,
		kDeclarators,
		kDeclarator,
		kExpList,
		kOrExpr,
		kXorExpr,
		kAndExpr,
		kShiftExpr,
		kAddExpr,
		kMultExpr,
		kUnaryExpr
	};
	static string changeNodeTypeToString(NodeType type);

	SyntacticTreeNode(const SyntacticTreeNode& node);

	SyntacticTreeNode& operator=(const SyntacticTreeNode& node) = delete;

	~SyntacticTreeNode();

	void addChild(SyntacticTreeNode* node);

	Token getValue();
	string getInfo();
	const vector<SyntacticTreeNode*>& getChildren();
	NodeType getNodeType();

	static vector<string> printTree(SyntacticTreeNode* root);

	// 节点构造静态方法
	static SyntacticTreeNode* makeStructNode(string identifier, SyntacticTreeNode* memberListNode);
	static SyntacticTreeNode* makeMemberListNode(vector<SyntacticTreeNode*> memberNodes);
	static SyntacticTreeNode* makeMemberNode(SyntacticTreeNode* typeSpecNode, SyntacticTreeNode* declaratorsNode);
	static SyntacticTreeNode* makeTypeSpecNode(SyntacticTreeNode* typeNode);
	static SyntacticTreeNode* makeBaseTypeSpecNode(string type);
	static SyntacticTreeNode* makeBaseTypeSpecNode(SyntacticTreeNode* typeNode);
	static SyntacticTreeNode* makeFloatingPtTypeNode(string type);
	static SyntacticTreeNode* makeIntegerTypeNode(SyntacticTreeNode* intTypeNode);
	static SyntacticTreeNode* makeSignedIntNode(string intType);
	static SyntacticTreeNode* makeUnsignedIntNode(string uintType);
	static SyntacticTreeNode* makeDeclaratorsNode(SyntacticTreeNode* declarator1, vector<SyntacticTreeNode*> declarators);
	static SyntacticTreeNode* makeDeclaratorNode(string identifier, SyntacticTreeNode* expList = nullptr);
	static SyntacticTreeNode* makeExpListNode(SyntacticTreeNode* orExpr1, vector<SyntacticTreeNode*> orExprs);
	static SyntacticTreeNode* makeExprNode(NodeType nodeType, SyntacticTreeNode* node1);
	static SyntacticTreeNode* makeExprNode(NodeType nodeType, SyntacticTreeNode* node1, SyntacticTreeNode* node2, string sign);
	static SyntacticTreeNode* makeUnaryExpr(Token value, string sign);
	static SyntacticTreeNode* makeUnaryExpr(Token value);

private:
	SyntacticTreeNode(NodeType nodeType);

	NodeType nodeType;

	vector<SyntacticTreeNode*> children;
	//int minChildrenNum = 0;
	//int maxChildrenNum = INT_MAX;

	// 存储节点包含的其他信息（类型节点的具体类型、运算节点的符号等）
	string *info = nullptr;
	// 存储值节点的值（int、string、bool）
	Token *value = nullptr;


	static void tranTree(SyntacticTreeNode* now, vector<string>& printer, int deepth);
};


/// <summary>
/// 语法分析类
/// </summary>
class SyntacticAnalyzer
{
public:
	SyntacticAnalyzer(const vector<Token>& tokenList);
	~SyntacticAnalyzer();

	// 进行语法分析：递归下降法
	void analyze();

	// 获取语法分析结果
	SyntacticTreeNode* getSyntaxTree();

	// 查看语法分析是否有错
	bool isSyntacticError();
	// 获取错误信息
	const vector<string>& getErrorMsgs();

private:
	const vector<Token>& tokenList;

	SyntacticTreeNode* root = nullptr;

	bool isError = false;
	vector<string> errorMsgs;

	void addAnalyzerError(std::string errorMessage);
	void addCodeError(std::string errorMessage, int lineNum, std::string errorTokens);

	int nowTokenNum = 0;
	Token getToken(int pos = 0);
	bool toNextToken();

	Token match(Token::TokenType tokenType);
	Token match(std::string tokenValue);
	Token match(Token::TokenType tokenType, std::string tokenValue);

	// 为了保证与上下文无关文法名字一致，此处不用小驼峰形式命名，采用“match_非终结符”形式
	SyntacticTreeNode* match_start_struct_type();
	SyntacticTreeNode* match_struct_type();
	SyntacticTreeNode* match_member_list();
	SyntacticTreeNode* match_type_spec();
	SyntacticTreeNode* match_base_type_spec();
	SyntacticTreeNode* match_floating_pt_type();
	SyntacticTreeNode* match_integer_type();
	SyntacticTreeNode* match_signed_int();
	SyntacticTreeNode* match_unsigned_int();
	SyntacticTreeNode* match_declarators();
	SyntacticTreeNode* match_declarator();
	SyntacticTreeNode* match_exp_list();
	SyntacticTreeNode* match_or_expr();
	SyntacticTreeNode* match_xor_expr();
	SyntacticTreeNode* match_and_expr();
	SyntacticTreeNode* match_shift_expr();
	SyntacticTreeNode* match_add_expr();
	SyntacticTreeNode* match_mult_expr();
	SyntacticTreeNode* match_unary_expr();

	SyntacticTreeNode* makeExprRusult(SyntacticTreeNode* node, std::vector<SyntacticTreeNode*>& nodes, SyntacticTreeNode::NodeType type);

	// 错误恢复
	bool needResumeRuning = false;

	void resumeRuning();
};

