#include "StringReader.h"

StringReader::StringReader(const string &inputString)
{
    this->inputString = inputString;
}

char StringReader::getChar()
{
    if (this->nowPos >= (int)this->inputString.size()) return EOF;
    else {
        char k = this->inputString.at(nowPos);
        nowPos++;
        return k;
    }
}
