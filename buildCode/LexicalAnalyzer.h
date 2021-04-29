#pragma once

#include "DataReader.h"

#include <string>
#include <vector>

using namespace std;


struct Token
{
	enum class TokenType {
		kUnknown,
		kKeyWord,
		kSign,
		kIdentifier,
		kInteger,
		kString,
		kBoolean,
		kEOF
	};

	Token(const string& value, TokenType type);

	string value;
	TokenType type = TokenType::kUnknown;

	string changeTypeToString(TokenType type);
	string getStringType();
};

namespace KeyWord
{
	extern const string kStruct;
	extern const string kFloat;
	extern const string kBoolean;
	extern const string kShort;
	extern const string kLong;
	extern const string kDouble;
	extern const string kInt8;
	extern const string kInt16;
	extern const string kInt32;
	extern const string kInt64;
	extern const string kUnit8;
	extern const string kUnit16;
	extern const string kUnit32;
	extern const string kUnit64;
	extern const string kChar;
	extern const string kUnsigned;

	extern const vector<string> keyWords;

	bool isKeyWord(string word);
};

namespace Sign
{
	extern const char kLBrace;
	extern const char kRBrace;
	extern const char kSemicolon;
	extern const char kLParenthesis;
	extern const char kRParenthesis;
	extern const char kMultiply;
	extern const char kPlus;
	extern const char kMinus;
	extern const char kWavyLine;
	extern const char kDivision;
	extern const char kPercent;
	extern const char kAnd;
	extern const char kSuperscript;
	extern const char kOr;
	extern const char kLSquareBracket;
	extern const char kRSquareBracket;
	extern const char kComma;

	extern const string kRightShift;
	extern const string kLeftShift;

	extern const vector<char> signs;
	extern const vector<string> signsWithMoreChar;

	bool isOneCharSign(char word);
	bool isTwoCharSignStart(char word);
};

/// <summary>
/// 词法分析类
/// </summary>
class LexicalAnalyzer
{
public:
	LexicalAnalyzer(DataReader& dataReader);

	// 进行词法分析（考虑转义字符）
	void analyze();

	// 获取词法分析结果
	vector<Token> getTokenList();

	// 查看词法分析是否有错
	bool isLexicalError();
	// 获取错误信息
	vector<string> getErrorMsgs();

private:
	enum class State {
		kStart,
		kDone,
		kIntager0,
		kIntager,
		kOneCharSign,
		kTwoCharSign,
		kString,
		kESC,
		kIDOrKeyOrBool,
		kID1,
		kID2
	};

	DataReader &dataReader;

	vector<Token> tokenList;

	bool isError = false;
	vector<string> errorMsgs;
};

