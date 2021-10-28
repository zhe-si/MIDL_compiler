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
/// ���ļ�����������
/// 
/// Ϊ����ʵ����ļ�����Ҫ����Ӵ˷������ɴ���LexicalAnalyzer��getTokenList�ӿ�
/// ��������δ�ô˷��������Ǳ���ԭ���ֱ�ӵ��õ�getTokenList�ӿڡ����Ҫ���ļ����������ø÷���ֱ���滻getTokenList
/// �����ļ�û���浥����������������������ʾ-1��ʾ��
/// </summary>
/// <param name="path">�ļ�·��</param>
/// <returns>��������</returns>
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
        throw FileException("�ļ���ʧ�ܣ�");
	}
}


int main()
{
    system(R"(C:\Windows\system32\chcp 65001)");// �޸��ն��ַ���ΪUTF8

    string filePath = "�˴���д�����ļ�λ��"; // TODO: �˴���д�����ļ�λ�� (���ļ���׺)
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
		for (const string& errorMsg : lexAnalyzer.getErrorMsgs()) {
			fileError << errorMsg << endl;
		}
		// ��ȡ�ʷ��������
		for (const Token& token : lexAnalyzer.getTokenList()) {
			fileLexicalOut << token.value << "\t" << token.getStringType() << endl;
		}
	}
	else {
		// ��ȡ�ʷ��������
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