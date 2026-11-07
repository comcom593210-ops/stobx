#include "CorrectionEngine.h"

std::string CorrectionEngine::suggestCorrection(int id) {
    switch (id) {
        case 1:
            return "Insert a closing parenthesis ')'.";

        case 2:
            return "Insert a closing quotation mark '\"'.";

        case 3:
            return "Insert the statement terminator '..'.";
            case 4:
            return "Insert an opening parenthesis '('.";

        default:
            return "No correction available.";
    }
}

std::string CorrectionEngine::createPreview(
    const std::string& sourceCode,
    const Diagnostic& diagnostic
) {
    std::string correctedCode = sourceCode;

    if (diagnostic.id == 1) {
        if (diagnostic.offset > correctedCode.size()) {
            return "";
        }

        correctedCode.insert(diagnostic.offset, ")");
        return correctedCode;
    }

    
    if (diagnostic.id == 4) {
        if (diagnostic.offset > correctedCode.size()) {
            return "";
        }

        correctedCode.insert(diagnostic.offset, "(");
        return correctedCode;
    }
    return "";
}

