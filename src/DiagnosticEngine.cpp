#include "DiagnosticEngine.h"
#include "DiagnosticCatalog.h"

Diagnostic DiagnosticEngine::analyze(
    int id,
    int line,
    int column,
    std::size_t offset
) {
    Diagnostic diagnostic = DiagnosticCatalog::getDiagnostic(id);

    diagnostic.line = line;
    diagnostic.column = column;
    diagnostic.offset = offset;

    return diagnostic;
}


