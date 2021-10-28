#pragma once
#include "DataReader.h"
#include <string>

using namespace std;

class StringReader : public DataReader
{
public:
	explicit StringReader(const string &inputString);

	// Í¨¹ý DataReader ¼Ì³Ð
	char getChar() override;

private:
	string inputString;
	int nowPos = 0;
};

