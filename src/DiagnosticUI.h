#ifndef DIAGNOSTIC_UI_H
#define DIAGNOSTIC_UI_H

#include "Diagnostic.h"
#include <string>

class DiagnosticUI {
public:
    void showDiagnostic(const Diagnostic& diagnostic);
    bool askForCorrection();
    void showCorrection(const std::string& correction);
};

#endif

