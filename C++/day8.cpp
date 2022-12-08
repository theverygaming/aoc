#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

struct tree {
    size_t height;
};

void calculateViewingDistance(ssize_t x, ssize_t y, std::vector<std::vector<struct tree>> *forest, std::vector<size_t> *sort) {
    struct tree *treeptr = &(*forest)[y][x];

    // up
    size_t dist = 0;
    for (ssize_t i = y - 1; i >= 0; i--) {
        dist++;
        if ((*forest)[i][x].height >= treeptr->height) {
            break;
        }
    }
    size_t score = dist;

    // left
    dist = 0;
    for (ssize_t i = x - 1; i >= 0; i--) {
        dist++;
        if ((*forest)[y][i].height >= treeptr->height) {
            break;
        }
    }
    score *= dist;

    // right
    dist = 0;
    for (ssize_t i = x + 1; i < (*forest)[y].size(); i++) {
        dist++;
        if ((*forest)[y][i].height >= treeptr->height) {
            break;
        }
    }
    score *= dist;

    // down
    dist = 0;
    for (ssize_t i = y + 1; i < (*forest).size(); i++) {
        dist++;
        if ((*forest)[i][x].height >= treeptr->height) {
            break;
        }
    }
    score *= dist;

    (*sort).push_back(score);
}

bool isTreeVisible(size_t x, size_t y, std::vector<std::vector<struct tree>> *forest) {
    // try from top/bottom
    ssize_t maxHeight = -1;
    for (ssize_t i = 0; i < (*forest)[y].size(); i++) {
        if ((ssize_t)(*forest)[y][i].height > maxHeight) {
            maxHeight = (*forest)[y][i].height;
            if (i == x) {
                return true;
            }
        }
    }
    maxHeight = -1;
    for (ssize_t i = (*forest)[y].size() - 1; i >= 0; i--) {
        if ((ssize_t)(*forest)[y][i].height > maxHeight) {
            maxHeight = (*forest)[y][i].height;
            if (i == x) {
                return true;
            }
        }
    }
    // try from side
    maxHeight = -1;
    for (ssize_t i = 0; i < (*forest).size(); i++) {
        if ((ssize_t)(*forest)[i][x].height > maxHeight) {
            maxHeight = (*forest)[i][x].height;
            if (i == y) {
                return true;
            }
        }
    }
    maxHeight = -1;
    for (ssize_t i = (*forest).size() - 1; i >= 0; i--) {
        if ((ssize_t)(*forest)[i][x].height > maxHeight) {
            maxHeight = (*forest)[i][x].height;
            if (i == y) {
                return true;
            }
        }
    }
    return false;
}

static void iamgoodatnamingfunctions(std::string filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        fprintf(stderr, "could not open file %s\n", filename.c_str());
        throw std::runtime_error("could not open file");
    }

    std::vector<std::vector<struct tree>> forest;

    std::vector<std::string> lines;
    {
        std::string currentLine;
        while (std::getline(infile, currentLine)) {
            lines.push_back(currentLine);
        }
    }

    for (size_t l = 0; l < lines.size(); l++) {
        if (lines[l].size() == 0) {
            continue;
        }
        forest.push_back({});
        forest.back().reserve(lines[l].length());
        char str[2];
        str[1] = '\0';
        for (size_t i = 0; i < lines[l].length(); i++) {
            str[0] = lines[l][i];
            forest.back().push_back({strtoul(str, nullptr, 10)});
        }
    }

    std::vector<size_t> sort;

    size_t visibleCount = 0;
    for (size_t y = 0; y < forest.size(); y++) {
        for (size_t x = 0; x < forest[y].size(); x++) {
            calculateViewingDistance(x, y, &forest, &sort);
            if (isTreeVisible(x, y, &forest)) {
                visibleCount += 1;
                printf("tree(%llu,%llu) is visible\n", x, y);
            } else {
                printf("tree(%llu,%llu) is NOT visible\n", x, y);
            }
        }
    }
    printf("solution part1: %u\n", visibleCount);
    std::sort(sort.begin(), sort.end());
    printf("solution part2: %llu\n", sort[sort.size() - 1]);
}

int main(int argc, char *argv[]) {
    iamgoodatnamingfunctions("day8_input.txt");
    return 0;
}
