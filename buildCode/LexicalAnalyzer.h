#pragma once

#include <string>
#include <vector>

#include "DataReader.h"


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

	Token();
	Token(const string& value, TokenType type);
	Token(const string& value, TokenType type, int tokenLine);

	string value;
	TokenType type = TokenType::kUnknown;
	int tokenLine = -1;

	static string changeTypeToString(TokenType type);
	static TokenType changeStringToType(const string& type);
	string getStringType() const;
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
}


namespace Sign
{
	extern const string kLBrace;
	extern const string kRBrace;
	extern const string kSemicolon;
	extern const string kLParenthesis;
	extern const string kRParenthesis;
	extern const string kMultiply;
	extern const string kPlus;
	extern const string kMinus;
	extern const string kWavyLine;
	extern const string kDivision;
	extern const string kPercent;
	extern const string kAnd;
	extern const string kSuperscript;
	extern const string kOr;
	extern const string kLSquareBracket;
	extern const string kRSquareBracket;
	extern const string kComma;

	extern const string kRightShift;
	extern const string kLeftShift;

	extern const vector<string> signs;
	extern const vector<string> signsWithMoreChar;

	bool isOneCharSign(char word);
	bool isTwoCharSignStart(char word);
}


/// <summary>
/// 词法分析类
/// </summary>
class LexicalAnalyzer
{
public:
	explicit LexicalAnalyzer(DataReader& dataReader);

	// 进行词法分析（考虑转义字符）
	void analyze();

	// 获取词法分析结果
	const vector<Token>& getTokenList();

	// 查看词法分析是否有错
	bool isLexicalError() const;
	// 获取错误信息
	const vector<string>& getErrorMsgs();

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

	void addCodeError(const std::string& errorMessage, bool& notReadNext, bool& toNextLine, int lineNum, std::string& nowWord, char nowChar, LexicalAnalyzer::State& nowState, Token::TokenType& nowType);
	void addAnalyzerError(const std::string& errorMessage, bool& notReadNext, bool& toNextLine, LexicalAnalyzer::State& nowState, std::string& nowWord, Token::TokenType& nowType);

	void statesOperation(LexicalAnalyzer::State& nowState, char nowChar, Token::TokenType& nowType, bool& notReadNext, bool& toNextLine, int lineNum, std::string& nowWord, bool& toNextLineInString);
	void finishOperation(bool& toNextLine, LexicalAnalyzer::State& nowState, bool& notReadNext, int lineNum, std::string& nowWord, char nowChar, Token::TokenType& nowType);
};

