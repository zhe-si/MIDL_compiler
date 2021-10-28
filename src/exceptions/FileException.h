//
// Created by lq on 2021/10/28.
//

#pragma once
#include <stdexcept>

class FileException : public std::runtime_error
{
public:
    explicit FileException(const std::string& msg): std::runtime_error(msg) {};
};
