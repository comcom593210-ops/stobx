
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

#include "SP.h"
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "DiagnosticUI.h"
#include "CorrectionEngine.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./stobx <filename.stobx>\n";
        return 1;
    }

const char* skipFile = ".stobx_skip_correction";
bool skipCorrectionPrompt = false;

std::ifstream skipFlag(skipFile);

if (skipFlag.good()) {
    skipCorrectionPrompt = true;
    skipFlag.close();
    std::remove(skipFile);
}

    SP processor;

    if (!processor.process(argv[1])) {
        if (!processor.process(argv[1])) {
            return 1;
        }

        return 0;
    }

    std::ifstream file(argv[1]);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string sourceCode = buffer.str();

    try {
        Lexer lexer(sourceCode);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto ast = parser.parse();

        if (!parser.getDiagnostics().empty() && !skipCorrectionPrompt){
            DiagnosticUI ui;
            
            CorrectionEngine correctionEngine;

            std::string currentSourceCode = sourceCode;
            std::size_t offsetAdjustment = 0;

            for (const auto& diagnostic : parser.getDiagnostics()) {
                ui.showDiagnostic(diagnostic);

                std::string correction =
                    correctionEngine.suggestCorrection(diagnostic.id);

                ui.showCorrection(correction);

                Diagnostic adjustedDiagnostic = diagnostic;
                adjustedDiagnostic.offset += offsetAdjustment;

                std::string preview =
                    correctionEngine.createPreview(
                        currentSourceCode,
                        adjustedDiagnostic
                    );

                if (preview.empty()) {
                    std::cout << "Could not create correction preview.\n";
                    continue;
                }

                if (ui.askForCorrection()) {
                    std::ofstream outputFile(
                        argv[1],
                        std::ios::out | std::ios::trunc
                    );

                    if (!outputFile.is_open()) {
                        std::cerr << "Error: Could not save corrected file.\n";
                        return 1;
                    }

                    outputFile << preview;
                    outputFile.close();

                    offsetAdjustment +=
                        preview.size() - currentSourceCode.size();

                    currentSourceCode = preview;

               } else {
    std::cout << "\nCorrection cancelled.\n";

    std::ofstream skipFlag(skipFile);

    if (!skipFlag.is_open()) {
        std::cerr << "Error: Could not save temporary state.\n";
        return 1;
    }

    skipFlag.close();

    return 1;
}

            }

                        return 1;
        }

        if (!parser.getErrors().empty()) {


            for (const auto& error : parser.getErrors()) {
                std::cerr << error << "\n";
            }

            return 1;
        }

        CodeGen codegen;
        codegen.generate(ast.get());
        codegen.printIR();

        int compileResult = std::system(
            "clang output.ll -o stobx_program"
        );

        if (compileResult != 0) {
            std::cerr << "Error: Failed to compile STOBX program\n";
            return 1;
        }

        int runResult = std::system(
            "./stobx_program"
        );

        if (runResult != 0) {
            std::cerr << "Error: Failed to run STOBX program\n";
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

