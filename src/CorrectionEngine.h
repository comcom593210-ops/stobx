#ifndef CORRECTION_ENGINE_H
#define CORRECTION_ENGINE_H

#include "Diagnostic.h"
#include <string>

class CorrectionEngine {
public:
    std::string suggestCorrection(int id);

    std::string createPreview(
        const std::string& sourceCode,
        const Diagnostic& diagnostic
    );
};

#endif


