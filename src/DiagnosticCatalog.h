#ifndef DIAGNOSTIC_CATALOG_H
#define DIAGNOSTIC_CATALOG_H

#include "Diagnostic.h"
#include <string>

class DiagnosticCatalog {
public:
    static Diagnostic getDiagnostic(int id);
};

#endif


