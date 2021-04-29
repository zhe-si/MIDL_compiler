#include "LexicalAnalyzer.h"

#include <algorithm>


namespace KeyWord
{
	const string kStruct = "struct";
	const string kFloat = "float";
	const string kBoolean = "boolean";
	const string kShort = "short";
	const string kLong = "long";
	const string kDouble = "double";
	const string kInt8 = "int8";
	const string kInt16 = "int16";
	const string kInt32 = "int32";
	const string kInt64 = "int64";
	const string kUnit8 = "unit8";
	const string kUnit16 = "unit16";
	const string kUnit32 = "unit32";
	const string kUnit64 = "unit64";
	const string kChar = "char";
	const string kUnsigned = "unsigned";

	const vector<string> keyWords{ kStruct, kFloat, kBoolean, kShort, kLong, kDouble,
		kInt8, kInt16, kInt32, kInt64, kUnit8, kUnit16, kUnit32, kUnit64, kChar, kUnsigned };


	bool isKeyWord(string word)
	{
		transform(word.begin(), word.end(), word.begin(), ::tolower);
		for (string keyWord : keyWords) {
			if (word == keyWord) return true;
		}
		return false;
	}
};

namespace Sign
{
	const char kLBrace = '{';
	const char kRBrace = '}';
	const char kSemicolon = ';';
	const char kLParenthesis = '(';
	const char kRParenthesis = ')';
	const char kMultiply = '*';
	const char kPlus = '+';
	const char kMinus = '-';
	const char kWavyLine = '~';
	const char kDivision = '/';
	const char kPercent = '%';
	const char kAnd = '&';
	const char kSuperscript = '^';
	const char kOr = '|';
	const char kLSquareBracket = '[';
	const char kRSquareBracket = ']';
	const char kComma = ',';

	const string kRightShift = ">>";
	const string kLeftShift = "<<";

	const vector<char> signs{ kLBrace, kRBrace, kSemicolon, kLParenthesis, kRParenthesis, kMultiply, kPlus,
		kMinus, kWavyLine, kDivision, kPercent, kAnd, kSuperscript, kOr, kLSquareBracket, kRSquareBracket, kComma };

	const vector<string> signsWithMoreChar{ kRightShift, kLeftShift };


	bool Sign::isOneCharSign(char word)
	{
		for (char keyWord : Sign::signs) {
			if (word == keyWord) return true;
		}
		return false;
	}

	bool isTwoCharSignStart(char word)
	{
		for (string keyWord : Sign::signsWithMoreChar) {
			if (word == keyWord.at(0)) return true;
		}
		return false;
	}
};

LexicalAnalyzer::LexicalAnalyzer(DataReader& dataReader)
	:dataReader{ dataReader }
{
}

void LexicalAnalyzer::analyze()
{
	char nowChar = -1;
	string nowWord = "";
	Token::TokenType nowType = Token::TokenType::kUnknown;

	int lineNum = 1;

	bool toNextLine = false;
	bool toNextLineInString = false;
	bool toNextLineESC = false;

	State nowState = State::kStart;

	bool notReadNext = false;

	while (true) {
		if (notReadNext) {
			notReadNext = false;
		}
		else {
			nowChar = this->dataReader.getChar();
		}
		if (nowChar == -1) break;

		if (nowChar == '\n') {
			lineNum++;
			if (!toNextLineInString) {
				toNextLine = false;
				toNextLineInString = false;
				toNextLineESC = false;
			}
			if (nowState != State::kString) continue;
		}
		if (toNextLine) {
			if (nowChar == '"' && !toNextLineESC) {
				toNextLineInString = !toNextLineInString;
			}
			if (nowChar == '\\' && !toNextLineESC) {
				toNextLineESC = true;
			}
			else {
				toNextLineESC = false;
			}
			continue;
		}

		statesOperation(nowState, nowChar, nowType, notReadNext, toNextLine, lineNum, nowWord, toNextLineInString);
	}

	finishOperation(toNextLine, nowState, notReadNext, lineNum, nowWord, nowChar, nowType);
}

void LexicalAnalyzer::statesOperation(LexicalAnalyzer::State& nowState, char nowChar, Token::TokenType& nowType, bool& notReadNext, bool& toNextLine, int lineNum, std::string& nowWord, bool& toNextLineInString)
{
	switch (nowState)
	{
	case State::kStart:
		if (isspace(nowChar))
			break;
		else if (nowChar == '0') {
			nowState = State::kIntager0;
			nowType = Token::TokenType::kInteger;
		}
		else if (isdigit(nowChar) && nowChar != '0') {
			nowState = State::kIntager;
			nowType = Token::TokenType::kInteger;
		}
		else if (Sign::isOneCharSign(nowChar)) {
			nowState = State::kOneCharSign;
			nowType = Token::TokenType::kSign;
		}
		else if (Sign::isTwoCharSignStart(nowChar)) {
			nowState = State::kTwoCharSign;
			nowType = Token::TokenType::kSign;
		}
		else if (nowChar == '"') {
			nowState = State::kString;
			nowType = Token::TokenType::kString;
		}
		else if (isalpha(nowChar))
			nowState = State::kIDOrKeyOrBool;
		else {
			// start 状态无法识别的单词开头（特殊符号）错误
			addCodeError("start 状态无法识别的单词开头（特殊符号）错误", notReadNext, toNextLine, lineNum, nowWord, nowChar, nowState, nowType);
			break;
		}
		nowWord.push_back(nowChar);
		break;
	case State::kDone:
		notReadNext = true;
		this->tokenList.push_back(Token(nowWord, nowType));
		nowWord.clear();
		nowType = Token::TokenType::kUnknown;
		nowState = State::kStart;
		break;
	case State::kIntager0:
		if (nowChar == 'l' || nowChar == 'L') {
			nowWord.push_back(nowChar);
		}
		else if (isdigit(nowChar) || (isalpha(nowChar) && nowChar != 'l' && nowChar != 'L')) {
			// 0作为数字或id开头错误
			addCodeError("0作为数字或id开头错误", notReadNext, toNextLine, lineNum, nowWord, nowChar, nowState, nowType);
			break;
		}
		else {
			notReadNext = true;
		}
		nowState = State::kDone;
		break;
	case State::kIntager:
		if (isdigit(nowChar)) {
			nowWord.push_back(nowChar);
		}
		else if (nowChar == 'l' || nowChar == 'L') {
			nowState = State::kDone;
			nowWord.push_back(nowChar);
		}
		else if (isalpha(nowChar) && nowChar != 'l' && nowChar != 'L') {
			// 数字作为id开头错误
			addCodeError("数字作为id开头错误", notReadNext, toNextLine, lineNum, nowWord, nowChar, nowState, nowType);
			break;
		}
		else {
			nowState = State::kDone;
			notReadNext = true;
		}
		break;
	case State::kOneCharSign:
		notReadNext = true;
		nowState = State::kDone;
		break;
	case State::kTwoCharSign:
		if (nowWord.size() != 1) {
			// 双符号第一个符号放入错误（词法分析程序）
			addAnalyzerError("双符号第一个符号放入错误（词法分析程序）", notReadNext, toNextLine, nowState, nowWord, nowType);
			break;
		}
		else if (nowWord.at(0) == nowChar) {
			nowState = State::kDone;
			nowWord.push_back(nowChar);
		}
		else {
			// 双符号读入第二个符号错误
			addCodeError("双符号读入第二个符号错误", notReadNext, toNextLine, lineNum, nowWord, nowChar, nowState, nowType);
			break;
		}
		break;
	case State::kString:
		if (nowChar == '"') {
			nowState = State::kDone;
			nowWord.push_back(nowChar);
		}
		else if (nowChar == '\\') {
			nowState = State::kESC;
			nowWord.push_back(nowChar);
		}
		else {
			nowWord.push_back(nowChar);
		}
		break;
	// 转义字符处理状态
	case State::kESC:
		if (nowChar == 'b' || nowChar == 't' || nowChar == 'n' || nowChar == 'f' 
			|| nowChar == 'r' || nowChar == '"' || nowChar == '\\') {

			nowState = State::kString;
			nowWord.push_back(nowChar);
		}
		else {
			// 无此转义字符错误
			addCodeError("无此转义字符错误", notReadNext, toNextLine, lineNum, nowWord, nowChar, nowState, nowType);
			toNextLineInString = true;
			break;
		}
		break;
	case State::kIDOrKeyOrBool:
		if (isalpha(nowChar) || isdigit(nowChar)) {
			nowWord.push_back(nowChar);
		}
		else if (nowChar == '_') {
			nowState = State::kID1;
			nowType = Token::TokenType::kIdentifier;
			nowWord.push_back(nowChar);
		}
		else {
			notReadNext = true;
			if (nowType == Token::TokenType::kUnknown) {
				if (KeyWord::isKeyWord(nowWord)) {
					nowType = Token::TokenType::kKeyWord;
				}
				else if (nowWord == "TRUE" or nowWord == "FALSE") {
					nowType = Token::TokenType::kBoolean;
				}
				else {
					nowType = Token::TokenType::kIdentifier;
				}
			}
			nowState = State::kDone;
		}
		break;
	case State::kID1:
		if (isalpha(nowChar) || isdigit(nowChar)) {
			nowState = State::kID2;
			nowWord.push_back(nowChar);
		}
		else {
			// identifier读取了下划线，之后没有数字或字母
			addCodeError("identifier读取了下划线，之后没有数字或字母错误", notReadNext, toNextLine, lineNum, nowWord, nowChar, nowState, nowType);
			break;
		}
		break;
	case State::kID2:
		if (isalpha(nowChar) || isdigit(nowChar)) {
			nowWord.push_back(nowChar);
		}
		else if (nowChar == '_') {
			nowWord.push_back(nowChar);
			nowState = State::kID1;
		}
		else {
			notReadNext = true;
			nowState = State::kDone;
		}
		break;
	default:
		// 状态异常错误（词法分析程序）
		addAnalyzerError("状态机状态异常错误（词法分析程序）", notReadNext, toNextLine, nowState, nowWord, nowType);
		break;
	}
}

void LexicalAnalyzer::finishOperation(bool& toNextLine, LexicalAnalyzer::State& nowState, bool& notReadNext, int lineNum, std::string& nowWord, char nowChar, Token::TokenType& nowType)
{
	if (!toNextLine) {
		if (nowState == State::kTwoCharSign) {
			addCodeError("未检测到符号的第二个字符错误", notReadNext, toNextLine, lineNum, nowWord, nowChar, nowState, nowType);
		}
		else if (nowState == State::kString || nowState == State::kESC) {
			addCodeError("字符串未结束错误", notReadNext, toNextLine, lineNum, nowWord, nowChar, nowState, nowType);
		}
		else if (nowState == State::kID1) {
			addCodeError("identifier 下划线后无字母或数字错误", notReadNext, toNextLine, lineNum, nowWord, nowChar, nowState, nowType);
		}
		else if (nowWord.size() != 0) {
			if (nowType == Token::TokenType::kUnknown) {
				if (KeyWord::isKeyWord(nowWord)) {
					nowType = Token::TokenType::kKeyWord;
				}
				else if ("TRUE" == nowWord or "FALSE" == nowWord) {
					nowType = Token::TokenType::kBoolean;
				}
				else {
					nowType = Token::TokenType::kIdentifier;
				}
			}
			this->tokenList.push_back(Token(nowWord, nowType));
		}
	}

	this->tokenList.push_back(Token("EOF", Token::TokenType::kEOF));
}

void LexicalAnalyzer::addAnalyzerError(std::string errorMessage, bool& notReadNext, bool& toNextLine, LexicalAnalyzer::State& nowState, std::string& nowWord, Token::TokenType& nowType)
{
	notReadNext = true;
	toNextLine = true;
	this->isError = true;
	this->errorMsgs.push_back(errorMessage);
	nowState = State::kStart;
	nowWord.clear();
	nowType = Token::TokenType::kUnknown;
}

void LexicalAnalyzer::addCodeError(std::string errorMessage, bool& notReadNext, bool& toNextLine, int lineNum, std::string& nowWord, char nowChar, LexicalAnalyzer::State& nowState, Token::TokenType& nowType)
{
	notReadNext = true;
	toNextLine = true;
	this->isError = true;
	this->errorMsgs.push_back(to_string(lineNum) + " >  " + "\"" + nowWord + nowChar + "\" : " + errorMessage);
	nowState = State::kStart;
	nowWord.clear();
	nowType = Token::TokenType::kUnknown;
}

const vector<Token>& LexicalAnalyzer::getTokenList()
{
	return this->tokenList;
}

const vector<string>& LexicalAnalyzer::getErrorMsgs()
{
	return this->errorMsgs;
}

bool LexicalAnalyzer::isLexicalError()
{
	return this->isError;
}

Token::Token(const string& value, TokenType type)
{
	this->value = value;
	this->type = type;
}

string Token::changeTypeToString(TokenType type)
{
	switch (type)
	{
	case Token::TokenType::kUnknown:
		return "Unknown";
	case Token::TokenType::kKeyWord:
		return "KeyWord";
	case Token::TokenType::kSign:
		return "Sign";
	case Token::TokenType::kIdentifier:
		return "Identifier";
	case Token::TokenType::kInteger:
		return "Integer";
	case Token::TokenType::kString:
		return "String";
	case Token::TokenType::kBoolean:
		return "Boolean";
	case Token::TokenType::kEOF:
		return "EOF";
	default:
		return "Unknown";
	}
}

string Token::getStringType()
{
	return changeTypeToString(this->type);
}
