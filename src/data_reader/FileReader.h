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

    // 通过 DataReader 继承
    char getChar() override;

private:
    ifstream inputFile;
};

