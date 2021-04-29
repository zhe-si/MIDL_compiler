#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "FileReader.h"
#include "LexicalAnalyzer.h"

using namespace std;


int main()
{
	string filePath = "E:/课程与任务/编译原理/实验/2021编译原理词法分析程序撰写的实验作业/work/测试用例/老师提供的测试文件/专有符号/test_3_formula";
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
	}

	fileOut.close();
	fileError.close();

	return 0;
}