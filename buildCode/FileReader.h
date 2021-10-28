#pragma once
#include "DataReader.h"
#include <string>
#include <fstream>

using namespace std;

class FileReader : public DataReader
{
public:
    explicit FileReader(const string& path);
    ~FileReader() override;

    // Í¨¹ý DataReader ¼Ì³Ð
    char getChar() override;

private:
    ifstream inputFile;
};

