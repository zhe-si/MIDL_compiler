#pragma once
#include "DataReader.h"
#include <string>

using namespace std;

class StringReader : public DataReader
{
public:
	StringReader(const string &inputString);

	// ͨ�� DataReader �̳�
	virtual char getChar() override;

private:
	string inputString;
	int nowPos = 0;
};

