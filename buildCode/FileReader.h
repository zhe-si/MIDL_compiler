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

    // Í¨¹ý DataReader ¼Ì³Ð
    virtual char getChar() override;

private:
    ifstream inputFile;
};

