#include "DiagnosticCatalog.h"

Diagnostic DiagnosticCatalog::getDiagnostic(int id) {
    switch (id) {
        case 1:
            return {1, " Missing closing parenthesis ')'", 0, 0};

        case 2:
            return {2, "Missing closing quotation mark", 0, 0};

        case 3:
            return {3, "Missing statement terminator '..'", 0, 0};
            case 4:
            return {4,"Missing opening parenthesis '('",0, 0};

        default:
            return {0, "Unknown diagnostic error", 0, 0};
    }
}


