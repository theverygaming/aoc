#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

static std::vector<std::vector<char>> stacks;

static size_t parsestack(std::vector<std::string> *lines) { // returns index of first command line
    stacks.clear();
    int stackcount = ((*lines)[0].size() + 1) / 4;
    stacks.resize(stackcount);
    size_t numline = 0;
    for (size_t l = 0; l < (*lines).size(); l++) {
        if (std::any_of((*lines)[l].begin(), (*lines)[l].end(), ::isdigit)) {
            numline = l + 2; // stack numbers + empty line
            break;
        }

        size_t stacki = 0;
        for (size_t i = 1; i < (*lines)[l].length();) {
            char id = (*lines)[l][i];
            if (id != ' ') {
                stacks[stacki].push_back(id);
            }
            i += 4;
            stacki++;
        }
    }
    for (size_t i = 0; i < stacks.size(); i++) { // reverse stacks because we parsed the wrong way around
        std::reverse(stacks[i].begin(), stacks[i].end());
    }
    return numline;
}

static void printstacks() {
    size_t largeststack = 0;
    for (size_t i = 0; i < stacks.size(); i++) {
        largeststack = std::max(stacks[i].size(), largeststack);
    }

    for (ssize_t i = largeststack - 1; i >= 0; i--) {
        for (size_t j = 0; j < stacks.size(); j++) {
            if (i < stacks[j].size()) {
                printf("[%c] ", stacks[j][i]);
            } else {
                printf("    ");
            }
        }
        printf("\n");
    }
}

static void iamgoodatnamingfunctions(std::string filename, bool part2 = false) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        fprintf(stderr, "could not open file %s\n", filename.c_str());
        throw std::runtime_error("could not open file");
    }

    std::vector<std::string> lines;
    {
        std::string currentLine;
        while (std::getline(infile, currentLine)) {
            lines.push_back(currentLine);
        }
    }

    for (size_t l = parsestack(&lines); l < lines.size(); l++) {
        int count, source, target;
        sscanf(lines[l].c_str(), "move %d from %d to %d", &count, &source, &target);
        for (int i = 0; i < count; i++) {
            char c = stacks[source - 1].back();
            stacks[source - 1].pop_back();
            stacks[target - 1].push_back(c);
        }
        if (part2) {
            std::reverse(stacks[target - 1].begin() + (stacks[target - 1].size() - count), stacks[target - 1].end()); // the cratemover:tm 9001 is epicly stronk
        }
    }

    printstacks();

    if (part2) {
        printf("solution part2: ");
    } else {
        printf("solution part1: ");
    }
    for (size_t i = 0; i < stacks.size(); i++) {
        printf("%c", stacks[i].back());
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    iamgoodatnamingfunctions("day5_example.txt");
    iamgoodatnamingfunctions("day5_example.txt", true);
    return 0;
}
