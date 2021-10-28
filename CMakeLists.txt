cmake_minimum_required(VERSION 3.20)
project(build_code)

# c++ 11
set(CMAKE_CXX_STANDARD 11)

add_executable(build_code
        src/main.cpp

        src/lexical_analyze/LexicalAnalyzer.h
        src/lexical_analyze/LexicalAnalyzer.cpp

        src/syntactic_analyze/SyntacticAnalyzer.h
        src/syntactic_analyze/SyntacticAnalyzer.cpp

        src/data_reader/DataReader.h
        src/data_reader/StringReader.h
        src/data_reader/StringReader.cpp
        src/data_reader/FileReader.h
        src/data_reader/FileReader.cpp

        src/exceptions/FileException.h
        )
