#pragma once
#include "DataReader.h"
#include <string>

using namespace std;

class StringReader : public DataReader
{
public:
	explicit StringReader(const string &inputString);

	// ͨ�� DataReader �̳�
	char getChar() override;

private:
	string inputString;
	int nowPos = 0;
};

