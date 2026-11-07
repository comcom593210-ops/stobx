#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <string>
#include <cstddef>

struct Diagnostic {

    int id;
    std::string message;
    int line;
    int column;
    std::size_t offset;
};

#endif

