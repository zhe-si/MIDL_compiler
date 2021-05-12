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
	string filePath = "E:/�γ�������/����ԭ��/ʵ��/2021����ԭ���﷨��������ʵ����ҵ/�﷨�����ļ�/MIDL�﷨�����ļ�/test15_complex";
	//string filePath = "E:/�γ�������/����ԭ��/ʵ��/2021����ԭ���﷨��������ʵ����ҵ/�﷨�����ļ�/MIDL�﷨�����ļ�/type_spec/test8_with_error";
	string fileLexicalOutPath = filePath + "_lexical_out.txt";
	string fileSyntacticOutPath = filePath + "_syntactic_out.txt";
	string fileErrorPath = filePath + "_error.txt";
	filePath = filePath + ".txt";

	ofstream fileLexicalOut(fileLexicalOutPath, ios::out);
	ofstream fileSyntacticOut(fileSyntacticOutPath, ios::out);
	ofstream fileError(fileErrorPath, ios::out);

	FileReader fReader(filePath);
	LexicalAnalyzer lexAnalyzer(fReader);
	// ���дʷ�����
	lexAnalyzer.analyze();
	// �鿴�ʷ������Ƿ��д�
	if (lexAnalyzer.isLexicalError()) {
		// ��ȡ������Ϣ
		for (string errorMsg : lexAnalyzer.getErrorMsgs()) {
			fileError << errorMsg << endl;
		}
		// ��ȡ�ʷ��������
		for (Token token : lexAnalyzer.getTokenList()) {
			fileLexicalOut << token.value << "\t" << token.getStringType() << endl;
		}
	}
	else {
		// ��ȡ�ʷ��������
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