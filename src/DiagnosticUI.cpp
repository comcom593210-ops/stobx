#include "DiagnosticUI.h"

#include <iostream>

void DiagnosticUI::showDiagnostic(const Diagnostic& diagnostic) {
    std::cout << "\nSTOBX ERROR\n\n";
    std::cout << diagnostic.message << '\n';
    std::cout << "Line: " << diagnostic.line << '\n';
    std::cout << "Column: " << diagnostic.column << "\n";
}

void DiagnosticUI::showCorrection(const std::string& correction) {
    std::cout << "\nSuggested correction:\n";
    std::cout << correction << '\n';
}

bool DiagnosticUI::askForCorrection() {
    char choice;

    std::cout << "\nApply correction? (Y/N): ";
    std::cin >> choice;

    return choice == 'Y' || choice == 'y';
}


