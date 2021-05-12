#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "FileReader.h"
#include "LexicalAnalyzer.h"
#include "SyntacticAnalyzer.h"

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
vector<Token> readTokenFile(string path) 
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
		throw exception("文件无法打开");
	}
}

int main()
{
	string filePath = "E:/课程与任务/编译原理/实验/2021编译原理语法分析程序实验作业/语法测试文件/MIDL语法测试文件/test15_complex";
	//string filePath = "E:/课程与任务/编译原理/实验/2021编译原理语法分析程序实验作业/语法测试文件/MIDL语法测试文件/type_spec/test8_with_error";
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
		for (string errorMsg : lexAnalyzer.getErrorMsgs()) {
			fileError << errorMsg << endl;
		}
		// 获取词法分析结果
		for (Token token : lexAnalyzer.getTokenList()) {
			fileLexicalOut << token.value << "\t" << token.getStringType() << endl;
		}
	}
	else {
		// 获取词法分析结果
		for (Token token : lexAnalyzer.getTokenList()) {
			fileLexicalOut << token.value << "\t" << token.getStringType() << endl;
		}

		SyntacticAnalyzer syntactivAnalyzer(lexAnalyzer.getTokenList());
		syntactivAnalyzer.analyze();
		if (syntactivAnalyzer.isSyntacticError()) {
			for (string errorMsg : syntactivAnalyzer.getErrorMsgs()) {
				fileError << errorMsg << endl;
			}
		}
		else {
			for (string data : SyntacticTreeNode::printTree(syntactivAnalyzer.getSyntaxTree())) {
				fileSyntacticOut << data << endl;
			}
		}
	}

	fileLexicalOut.close();
	fileSyntacticOut.close();
	fileError.close();

	return 0;
}