#pragma once
#include "DataReader.h"
#include <string>
#include <fstream>

using namespace std;

class FileReader : public DataReader
{
public:
    FileReader(string path);
    ~FileReader();

    // ͨ�� DataReader �̳�
    virtual char getChar() override;

private:
    ifstream inputFile;
};

