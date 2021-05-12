#pragma once

#include <vector>

#include "LexicalAnalyzer.h"


using namespace std;


/// <summary>
/// 语法分析类
/// </summary>
class SyntacticAnalyzer
{
public:
	SyntacticAnalyzer(const vector<Token>& tokenList);

	// 进行语法分析：递归下降法
	void analyze();

	// 获取语法分析结果
	void getSyntaxTree();

	// 查看语法分析是否有错
	bool isSyntacticError();
	// 获取错误信息
	const vector<string>& getErrorMsgs();

private:
	const vector<Token>& tokenList;

	bool isError = false;
	vector<string> errorMsgs;

	void addAnalyzerError(std::string errorMessage);
	void addCodeError(std::string errorMessage, int lineNum, std::string errorTokens);

	int nowTokenNum = 0;
	Token getToken(int pos = 0);
	bool toNextToken();

	void match(Token::TokenType tokenType);
	void match(std::string tokenValue);
	void match(Token::TokenType tokenType, std::string tokenValue);

	// 为了保证与上下文无关文法名字一致，此处不用小驼峰形式命名，采用“match_非终结符”形式
	void match_start_struct_type();
	void match_struct_type();
	void match_member_list();
	void match_type_spec();
	void match_base_type_spec();
	void match_floating_pt_type();
	void match_integer_type();
	void match_signed_int();
	void match_unsigned_int();
	void match_declarators();
	void match_declarator();
	void match_exp_list();
	void match_or_expr();
	void match_xor_expr();
	void match_and_expr();
	void match_shift_expr();
	void match_add_expr();
	void match_mult_expr();
	void match_unary_expr();

	// 错误恢复
	bool needResumeRuning = false;

	void resumeRuning();
};

