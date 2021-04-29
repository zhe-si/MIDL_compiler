#include "FileReader.h"
#include <fstream>

using namespace std;

FileReader::FileReader(string path)
    :inputFile(path, ios::in)
{
}

FileReader::~FileReader()
{
    this->inputFile.close();
}

char FileReader::getChar()
{
    return this->inputFile.get();
}
