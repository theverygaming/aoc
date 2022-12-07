#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

bool ismarker(const char *str, int count) {
    std::string chars = "";
    for (int i = 0; (i < count) && *str; i++) {
        if (chars.find(std::string(1, *str)) != std::string::npos) {
            return false;
        }
        chars += *str;
        str++;
    }
    return true;
}

int findmarker(std::string str, int length) {
    int counter = 0;
    const char *cstr = str.c_str();
    while (*cstr) {
        if (ismarker(cstr, length)) {
            break;
        }
        cstr++;
        counter++;
    }
    return counter + length;
}

static void process(std::string filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        fprintf(stderr, "could not open file %s\n", filename.c_str());
        throw std::runtime_error("could not open file");
    }
    
    std::stringstream in;
    in << infile.rdbuf();

    printf("result part1: %u\nresult part2: %u\n", findmarker(in.str(), 4), findmarker(in.str(), 14));
}

int main(int argc, char *argv[]) {
    process("day6_input.txt");
    return 0;
}
