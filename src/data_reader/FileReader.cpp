#include "FileReader.h"
#include "../exceptions/FileException.h"
#include <fstream>

using namespace std;


FileReader::FileReader(const string& path)
    :inputFile(path, ios::in)
{
    if (!inputFile.is_open()) {
        throw FileException("文件打开失败！");
    }
}

FileReader::~FileReader()
{
    this->inputFile.close();
}

char FileReader::getChar()
{
    return (char) this->inputFile.get();
}
