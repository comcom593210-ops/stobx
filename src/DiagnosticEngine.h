#ifndef DIAGNOSTIC_ENGINE_H
#define DIAGNOSTIC_ENGINE_H

#include "Diagnostic.h"

class DiagnosticEngine {
public:
    Diagnostic analyze(
        int id, 
        int line,
         int column,
    std::size_t offset
    );
};
#endif