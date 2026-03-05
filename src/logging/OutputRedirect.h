#pragma once
#include "LogBuffer.h"
#include <iostream>
#include <streambuf>

class OutputRedirect : public std::streambuf {
public:
    OutputRedirect(LogBuffer& buffer) : logBuffer(buffer) {
       // oldBuf = std::cout.rdbuf(this);
       // oldErrBuf = std::cerr.rdbuf(this);
    }

    ~OutputRedirect() {
       // std::cout.rdbuf(oldBuf);
       // std::cerr.rdbuf(oldErrBuf);
    }

protected:
    int overflow(int ch) override {
        if (ch == '\n') {
            logBuffer.Add(currentLine);
            currentLine.clear();
        }
        else {
            currentLine += (char)ch;
        }
        return ch;
    }

private:
    LogBuffer& logBuffer;
    std::string currentLine;
    std::streambuf* oldBuf;
    std::streambuf* oldErrBuf;
};
