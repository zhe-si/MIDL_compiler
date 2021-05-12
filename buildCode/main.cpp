#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "FileReader.h"
#include "LexicalAnalyzer.h"
#include "SyntacticAnalyzer.h"

using namespace std;


int main()
{
	string filePath = "E:/课程与任务/编译原理/实验/2021编译原理语法分析程序实验作业/语法测试文件/MIDL语法测试文件/test15_complex";
	//string filePath = "E:/课程与任务/编译原理/实验/2021编译原理语法分析程序实验作业/语法测试文件/MIDL语法测试文件/type_spec/test8_with_error";
	string fileOutPath = filePath + "_out.txt";
	string fileErrorPath = filePath + "_error.txt";
	filePath = filePath + ".txt";

	ofstream fileOut(fileOutPath, ios::out);
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
			fileOut << token.value << "\t" << token.getStringType() << endl;
		}
	}
	else {
		// 获取词法分析结果
		for (Token token : lexAnalyzer.getTokenList()) {
			fileOut << token.value << "\t" << token.getStringType() << endl;
		}

		SyntacticAnalyzer syntactivAnalyzer(lexAnalyzer.getTokenList());
		syntactivAnalyzer.analyze();
		if (syntactivAnalyzer.isSyntacticError()) {
			for (string errorMsg : syntactivAnalyzer.getErrorMsgs()) {
				fileError << errorMsg << endl;
			}
		}
		else {

		}
	}

	fileOut.close();
	fileError.close();

	return 0;
}