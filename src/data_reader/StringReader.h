#pragma once
#include "DataReader.h"
#include <string>

using namespace std;

class StringReader : public DataReader
{
public:
	explicit StringReader(const string &inputString);

	// 通过 DataReader 继承
	char getChar() override;

private:
	string inputString;
	int nowPos = 0;
};

