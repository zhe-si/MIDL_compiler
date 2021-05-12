#include "SyntacticAnalyzer.h"

#include <stdexcept>
#include <algorithm>


SyntacticAnalyzer::SyntacticAnalyzer(const vector<Token>& tokenList)
	:tokenList{ tokenList }
{
}

void SyntacticAnalyzer::analyze()
{
	match_start_struct_type();
}

bool SyntacticAnalyzer::isSyntacticError()
{
	return this->isError;
}

const vector<string>& SyntacticAnalyzer::getErrorMsgs()
{
	return this->errorMsgs;
}

void SyntacticAnalyzer::addAnalyzerError(std::string errorMessage)
{
	throw std::out_of_range("SyntacticAnalyzer: " + errorMessage);
}

void SyntacticAnalyzer::addCodeError(std::string errorMessage, int lineNum, std::string errorTokens)
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

void SyntacticAnalyzer::match(Token::TokenType tokenType)
{
	Token nowToken = getToken();
	if (nowToken.type == tokenType) {
		toNextToken();
	}
	else {
		addCodeError(nowToken.getStringType() + " 单词类型不匹配 " + Token::changeTypeToString(tokenType), nowToken.tokenLine, nowToken.value);
	}
}

void SyntacticAnalyzer::match(std::string tokenValue)
{
	Token nowToken = getToken();
	if (nowToken.value == tokenValue) {
		toNextToken();
	}
	else {
		addCodeError("单词不匹配 " + tokenValue, nowToken.tokenLine, nowToken.value);
	}
}

void SyntacticAnalyzer::match(Token::TokenType tokenType, std::string tokenValue)
{	
	Token nowToken = getToken();
	if (nowToken.value == tokenValue && nowToken.type == tokenType) {
		toNextToken();
	}
	else {
		addCodeError("单词及其类型不匹配 " + tokenValue + "(" + Token::changeTypeToString(tokenType) + ")", nowToken.tokenLine, nowToken.value);
	}
}

void SyntacticAnalyzer::match_start_struct_type()
{
	match_struct_type();
	match(Token::TokenType::kEOF);
}

void SyntacticAnalyzer::match_struct_type()
{
	match(Token::TokenType::kKeyWord, KeyWord::kStruct);
	match(Token::TokenType::kIdentifier);
	match(Token::TokenType::kSign, Sign::kLBrace);
	match_member_list();
	match(Token::TokenType::kSign, Sign::kRBrace);
}

void SyntacticAnalyzer::match_member_list()
{
	while (getToken().value != Sign::kRBrace && getToken().type != Token::TokenType::kEOF) {
		match_type_spec();
		match_declarators();
		resumeRuning(); // 出错后定位到member_list下一条语句
		match(Token::TokenType::kSign, Sign::kSemicolon);
	}
}

void SyntacticAnalyzer::resumeRuning()
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

void SyntacticAnalyzer::match_type_spec()
{
	if (getToken().value == KeyWord::kStruct) {
		match_struct_type();
	}
	else {
		match_base_type_spec();
	}
}

void SyntacticAnalyzer::match_base_type_spec()
{
	if (getToken().value == KeyWord::kChar) {
		match(Token::TokenType::kKeyWord, KeyWord::kChar);
	}
	else if (getToken().value == KeyWord::kBoolean) {
		match(Token::TokenType::kKeyWord, KeyWord::kBoolean);
	}
	else {
		if (getToken().value == KeyWord::kFloat) {
			match_floating_pt_type();
		}
		else if (getToken().value == KeyWord::kDouble) {
			match_floating_pt_type();
		}
		else if (getToken().value == KeyWord::kLong && getToken(1).value == KeyWord::kDouble) {
			match_floating_pt_type();
		}
		else {
			match_integer_type();
		}
	}
}

void SyntacticAnalyzer::match_floating_pt_type()
{
	if (getToken().value == KeyWord::kFloat) {
		match(Token::TokenType::kKeyWord, KeyWord::kFloat);
	}
	else if (getToken().value == KeyWord::kDouble) {
		match(Token::TokenType::kKeyWord, KeyWord::kDouble);
	}
	else if (getToken().value == KeyWord::kLong && getToken(1).value == KeyWord::kDouble) {
		match(Token::TokenType::kKeyWord, KeyWord::kLong);
		match(Token::TokenType::kKeyWord, KeyWord::kDouble);
	}
	else {
		addCodeError("floating_pt_type不匹配", getToken().tokenLine, getToken().value);
		if (getToken().type != Token::TokenType::kIdentifier) {
			toNextToken();
		}
	}
}

void SyntacticAnalyzer::match_integer_type()
{
	if (getToken().value.at(0) == 'u') {
		match_unsigned_int();
	}
	else {
		match_signed_int();
	}
}

void SyntacticAnalyzer::match_signed_int()
{
	if (getToken().value == KeyWord::kShort) {
		match(Token::TokenType::kKeyWord, KeyWord::kShort);
	}
	else if (getToken().value == KeyWord::kInt16) {
		match(Token::TokenType::kKeyWord, KeyWord::kInt16);
	}
	else if (getToken().value == KeyWord::kLong && getToken(1).value == KeyWord::kLong) {
		match(Token::TokenType::kKeyWord, KeyWord::kLong);
		match(Token::TokenType::kKeyWord, KeyWord::kLong);
	}
	else if (getToken().value == KeyWord::kInt64) {
		match(Token::TokenType::kKeyWord, KeyWord::kInt64);
	}
	else if (getToken().value == KeyWord::kLong) {
		match(Token::TokenType::kKeyWord, KeyWord::kLong);
	}
	else if (getToken().value == KeyWord::kInt32) {
		match(Token::TokenType::kKeyWord, KeyWord::kInt32);
	}
	else if (getToken().value == KeyWord::kInt8) {
		match(Token::TokenType::kKeyWord, KeyWord::kInt8);
	}
	else {
		addCodeError("type_spec不匹配", getToken().tokenLine, getToken().value);
		if (getToken().type != Token::TokenType::kIdentifier) {
			toNextToken();
		}
	}
}

void SyntacticAnalyzer::match_unsigned_int()
{
	if (getToken().value == KeyWord::kUnsigned) {
		match(Token::TokenType::kKeyWord, KeyWord::kUnsigned);

		if (getToken().value == KeyWord::kShort) {
			match(Token::TokenType::kKeyWord, KeyWord::kShort);
		}
		else if (getToken().value == KeyWord::kLong && getToken(1).value == KeyWord::kLong) {
			match(Token::TokenType::kKeyWord, KeyWord::kLong);
			match(Token::TokenType::kKeyWord, KeyWord::kLong);
		}
		else if (getToken().value == KeyWord::kLong) {
			match(Token::TokenType::kKeyWord, KeyWord::kLong);
		}
		else {
			addCodeError("unsigned_int不匹配", getToken().tokenLine, getToken().value);
			if (getToken().type != Token::TokenType::kIdentifier) {
				toNextToken();
			}
		}
	}
	else if (getToken().value == KeyWord::kUnit16) {
		match(Token::TokenType::kKeyWord, KeyWord::kUnit16);
	}
	else if (getToken().value == KeyWord::kUnit32) {
		match(Token::TokenType::kKeyWord, KeyWord::kUnit32);
	}
	else if (getToken().value == KeyWord::kUnit64) {
		match(Token::TokenType::kKeyWord, KeyWord::kUnit64);
	}
	else if (getToken().value == KeyWord::kUnit8) {
		match(Token::TokenType::kKeyWord, KeyWord::kUnit8);
	}
	else {
		addCodeError("type_spec不匹配", getToken().tokenLine, getToken().value);
		if (getToken().type != Token::TokenType::kIdentifier) {
			toNextToken();
		}
	}
}

void SyntacticAnalyzer::match_declarators()
{
	match_declarator();
	while (getToken().value == Sign::kComma) {
		match(Token::TokenType::kSign, Sign::kComma);
		match_declarator();
	}
}

void SyntacticAnalyzer::match_declarator()
{
	match(Token::TokenType::kIdentifier);
	if (getToken().value == Sign::kLSquareBracket) {
		match_exp_list();
	}
}

void SyntacticAnalyzer::match_exp_list()
{
	match(Token::TokenType::kSign, Sign::kLSquareBracket);
	match_or_expr();
	while (getToken().value == Sign::kComma) {
		match(Token::TokenType::kSign, Sign::kComma);
		match_or_expr();
	}
	match(Token::TokenType::kSign, Sign::kRSquareBracket);
}

void SyntacticAnalyzer::match_or_expr()
{
	match_xor_expr();
	while (getToken().value == Sign::kOr) {
		match(Token::TokenType::kSign, Sign::kOr);
		match_xor_expr();
	}
}

void SyntacticAnalyzer::match_xor_expr()
{
	match_and_expr();
	while (getToken().value == Sign::kSuperscript) {
		match(Token::TokenType::kSign, Sign::kSuperscript);
		match_and_expr();
	}
}

void SyntacticAnalyzer::match_and_expr()
{
	match_shift_expr();
	while (getToken().value == Sign::kAnd) {
		match(Token::TokenType::kSign, Sign::kAnd);
		match_shift_expr();
	}
}

void SyntacticAnalyzer::match_shift_expr()
{
	match_add_expr();
	while (getToken().value == Sign::kRightShift || getToken().value == Sign::kLeftShift) {
		match(Token::TokenType::kSign);
		match_add_expr();
	}
}

void SyntacticAnalyzer::match_add_expr()
{
	match_mult_expr();
	while (getToken().value == Sign::kPlus || getToken().value == Sign::kMinus) {
		match(Token::TokenType::kSign);
		match_mult_expr();
	}
}

void SyntacticAnalyzer::match_mult_expr()
{
	match_unary_expr();
	while (getToken().value == Sign::kMultiply || getToken().value == Sign::kDivision || getToken().value == Sign::kPercent) {
		match(Token::TokenType::kSign);
		match_unary_expr();
	}
}

void SyntacticAnalyzer::match_unary_expr()
{
	if (getToken().value == Sign::kPlus || getToken().value == Sign::kMinus || getToken().value == Sign::kWavyLine) {
		match(Token::TokenType::kSign);
	}
	if (getToken().type == Token::TokenType::kInteger) {
		match(Token::TokenType::kInteger);
	}
	else if (getToken().type == Token::TokenType::kString) {
		match(Token::TokenType::kString);
	}
	else if (getToken().type == Token::TokenType::kBoolean) {
		match(Token::TokenType::kBoolean);
	}
	else {
		addCodeError("unary_expr不匹配", getToken().tokenLine, getToken().value);
		if (getToken().type != Token::TokenType::kSign) {
			toNextToken();
		}
	}
}
