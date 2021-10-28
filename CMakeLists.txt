cmake_minimum_required(VERSION 3.20)
project(build_code)

# c++ 11
set(CMAKE_CXX_STANDARD 11)

add_executable(build_code
        buildCode/main.cpp

        buildCode/LexicalAnalyzer.h
        buildCode/LexicalAnalyzer.cpp

        buildCode/SyntacticAnalyzer.h
        buildCode/SyntacticAnalyzer.cpp

        buildCode/DataReader.h
        buildCode/StringReader.h
        buildCode/StringReader.cpp
        buildCode/FileReader.h
        buildCode/FileReader.cpp

        buildCode/exceptions/FileException.h
        )
