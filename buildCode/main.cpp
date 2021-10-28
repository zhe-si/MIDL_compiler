#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

#include "FileReader.h"
#include "LexicalAnalyzer.h"
#include "SyntacticAnalyzer.h"
#include "exceptions/FileException.h"


using namespace std;


/// <summary>
/// 从文件读单词序列
/// 
/// 为满足实验从文件读的要求，添加此方法，可代替LexicalAnalyzer的getTokenList接口
/// 但本程序未用此方法，而是保持原设计直接调用的getTokenList接口。如果要从文件读，可以用该方法直接替换getTokenList
/// 由于文件没保存单词行数，报错是行数会显示-1表示空
/// </summary>
/// <param name="path">文件路径</param>
/// <returns>单词序列</returns>
vector<Token> readTokenFile(const string& path)
{
	ifstream inputFile(path, ios::in);
	if (inputFile.is_open()) {
		vector<Token> tList;
		string value, type;
		while (true) {
			inputFile >> value >> type;
			tList.push_back(Token(value, Token::changeStringToType(type)));
			if (Token::changeStringToType(type) == Token::TokenType::kEOF) break;
		}
		return tList;
	}
	else {
        throw FileException("文件打开失败！");
	}
}


int main()
{
    system(R"(C:\Windows\system32\chcp 65001)");// 修改终端字符集为UTF8

    string filePath = "此处填写代码文件位置"; // TODO: 此处填写代码文件位置 (无文件后缀)
	string fileLexicalOutPath = filePath + "_lexical_out.txt";
	string fileSyntacticOutPath = filePath + "_syntactic_out.txt";
	string fileErrorPath = filePath + "_error.txt";
	filePath = filePath + ".txt";

	ofstream fileLexicalOut(fileLexicalOutPath, ios::out);
	ofstream fileSyntacticOut(fileSyntacticOutPath, ios::out);
	ofstream fileError(fileErrorPath, ios::out);

	FileReader fReader(filePath);
	LexicalAnalyzer lexAnalyzer(fReader);
	// 进行词法分析
	lexAnalyzer.analyze();
	// 查看词法分析是否有错
	if (lexAnalyzer.isLexicalError()) {
		// 获取错误信息
		for (const string& errorMsg : lexAnalyzer.getErrorMsgs()) {
			fileError << errorMsg << endl;
		}
		// 获取词法分析结果
		for (const Token& token : lexAnalyzer.getTokenList()) {
			fileLexicalOut << token.value << "\t" << token.getStringType() << endl;
		}
	}
	else {
		// 获取词法分析结果
		for (const Token& token : lexAnalyzer.getTokenList()) {
			fileLexicalOut << token.value << "\t" << token.getStringType() << endl;
		}

		SyntacticAnalyzer syntacticAnalyzer(lexAnalyzer.getTokenList());
		syntacticAnalyzer.analyze();
		if (syntacticAnalyzer.isSyntacticError()) {
			for (const string& errorMsg : syntacticAnalyzer.getErrorMsgs()) {
				fileError << errorMsg << endl;
			}
		}
		else {
			for (const string& data : SyntacticTreeNode::printTree(syntacticAnalyzer.getSyntaxTree())) {
				fileSyntacticOut << data << endl;
			}
		}
	}

	fileLexicalOut.close();
	fileSyntacticOut.close();
	fileError.close();

	return 0;
}