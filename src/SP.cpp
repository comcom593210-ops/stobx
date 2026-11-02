#include "SP.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>

struct GroupInfo {
    int number;
    int start;
    int end;
};

static bool isMarker(const std::string& line, int& number) {
    if (line.size() != 3 ||
        line[0] != '#' ||
        !std::isdigit(static_cast<unsigned char>(line[1])) ||
        line[2] != '#') {
        return false;
    }

    number = line[1] - '0';
    return true;
}

static bool readLines(
    const std::string& filename,
    std::vector<std::string>& lines
) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file "
                  << filename << "\n";
        return false;
    }

    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return true;
}

static bool writeLines(
    const std::string& filename,
    const std::vector<std::string>& lines
) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file "
                  << filename << "\n";
        return false;
    }

    for (std::size_t i = 0; i < lines.size(); i++) {
        file << lines[i];

        if (i + 1 < lines.size()) {
            file << "\n";
        }
    }

    return true;
}

static bool findGroup(
    const std::vector<std::string>& lines,
    int groupNumber,
    GroupInfo& group
) {
    int start = -1;
    int end = -1;

    for (std::size_t i = 0; i < lines.size(); i++) {
        int number = -1;

        if (!isMarker(lines[i], number) ||
            number != groupNumber) {
            continue;
        }

        if (start == -1) {
            start = static_cast<int>(i);
        } else {
            end = static_cast<int>(i);
            break;
        }
    }

    if (start == -1 || end == -1) {
        return false;
    }

    group.number = groupNumber;
    group.start = start;
    group.end = end;

    return true;
}

bool SP::process(const std::string& filename) {
    std::vector<std::string> lines;

    if (!readLines(filename, lines)) {
        return false;
    }

    for (std::size_t i = 0; i < lines.size(); i++) {
        const std::string& command = lines[i];

        bool isDelete = command.rfind("delete ", 0) == 0;
        bool isCopy = command.rfind("copy ", 0) == 0;
        bool isMove = command.rfind("move ", 0) == 0;

        if (!isDelete && !isCopy && !isMove) {
            continue;
        }

        std::istringstream input(command);

        std::string operation;
        std::string target;
        std::string destination;
        int targetLine = 0;

        input >> operation >> target;

        if (target.size() != 3 ||
            target[0] != '#' ||
            !std::isdigit(static_cast<unsigned char>(target[1])) ||
            target[2] != '#') {
            std::cerr << "Error: Invalid group command\n";
            return false;
        }

        int groupNumber = target[1] - '0';

        if (isMove) {
            if (!(input >> destination >> targetLine) ||
                targetLine < 1) {
                std::cerr << "Error: Invalid move command\n";
                return false;
            }
        }

        std::string extra;

        if (input >> extra) {
            std::cerr << "Error: Extra command arguments\n";
            return false;
        }

        GroupInfo group;

        if (!findGroup(lines, groupNumber, group)) {
            std::cerr << "Error: Group not found or incomplete\n";
            return false;
        }

        std::vector<std::string> contents(
            lines.begin() + group.start + 1,
            lines.begin() + group.end
        );

        if (isDelete) {
            lines.erase(
                lines.begin() + i
            );

            GroupInfo updatedGroup;

            if (!findGroup(lines, groupNumber, updatedGroup)) {
                std::cerr << "Error: Group not found\n";
                return false;
            }

            lines.erase(
                lines.begin() + updatedGroup.start,
                lines.begin() + updatedGroup.end + 1
            );

            if (!writeLines(filename, lines)) {
                return false;
            }

            std::cout << "Group deleted successfully!\n";
            return true;
        }

        if (isCopy) {
            lines.erase(lines.begin() + i);

            lines.insert(
                lines.begin() + i,
                contents.begin(),
                contents.end()
            );

            if (!writeLines(filename, lines)) {
                return false;
            }

            std::cout << "Group copied successfully!\n";
            return true;
        }

        std::vector<std::string> destinationLines;

        if (!readLines(destination, destinationLines)) {
            return false;
        }

        if (targetLine > static_cast<int>(destinationLines.size())) {
            std::cerr << "Error: Target line does not exist\n";
            return false;
        }

        if (!destinationLines[targetLine - 1].empty()) {
            std::cerr << "Error: Target line must be blank\n";
            return false;
        }

        destinationLines.erase(
            destinationLines.begin() + targetLine - 1
        );

        destinationLines.insert(
            destinationLines.begin() + targetLine - 1,
            contents.begin(),
            contents.end()
        );

        if (!writeLines(destination, destinationLines)) {
            return false;
        }

        lines.erase(lines.begin() + i);

        GroupInfo updatedGroup;

        if (!findGroup(lines, groupNumber, updatedGroup)) {
            std::cerr << "Error: Group not found after move\n";
            return false;
        }

        lines.erase(
            lines.begin() + updatedGroup.start,
            lines.begin() + updatedGroup.end + 1
        );

        if (!writeLines(filename, lines)) {
            return false;
        }

        std::cout << "Group moved successfully!\n";
        return true;
    }

    return true;
}

