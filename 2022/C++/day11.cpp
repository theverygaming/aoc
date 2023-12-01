#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

// https://stackoverflow.com/a/3418285
bool replace(std::string &str, const std::string &from, const std::string &to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

// https://stackoverflow.com/a/46931770
std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

enum class m_operator { ADD, MULTIPLY };

struct monke {
    std::vector<ssize_t> items;

    bool operation_self;     // operation is old * old or similar
    ssize_t operation_value; // unused when operation_self true
    m_operator mathop;

    ssize_t divtestnum;

    size_t monke_true; // monkey to throw item to when divtest succeeds
    size_t monke_false;

    size_t items_inspected;
};

bool monkesort(const monke &a, const monke &b) {
    return a.items_inspected < b.items_inspected;
}

static void monkeparse(std::vector<struct monke> *monkes, std::string filename) {
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

    for (size_t l = 0; l < lines.size();) {
        if (lines[l].size() == 0) {
            continue;
        }

        (*monkes).push_back({});

        if (replace(lines[l + 1], "  Starting items: ", "")) {
            std::vector<std::string> values = split(lines[l + 1], ", ");
            for (size_t i = 0; i < values.size(); i++) {
                ssize_t value;
                sscanf(values[i].c_str(), "%lld", &value);
                (*monkes).back().items.push_back(value);
            }
        }

        char c;
        char str[5];
        sscanf(lines[l + 2].c_str(), "  Operation: new = old %c %s", &c, str);
        switch (c) {
        case '*':
            (*monkes).back().mathop = m_operator::MULTIPLY;
            break;
        case '+':
            (*monkes).back().mathop = m_operator::ADD;
            break;
        default:
            fprintf(stderr, "%c ??\n", c);
            exit(1);
            break;
        }

        if (strcmp(str, "old") == 0) {
            (*monkes).back().operation_self = true;
        } else {
            sscanf(str, "%lld", &(*monkes).back().operation_value);
        }

        sscanf(lines[l + 3].c_str(), "  Test: divisible by %lld", &(*monkes).back().divtestnum);

        sscanf(lines[l + 4].c_str(), "    If true: throw to monkey %lld", &(*monkes).back().monke_true);
        sscanf(lines[l + 5].c_str(), "    If false: throw to monkey %lld", &(*monkes).back().monke_false);

        l += 7;
    }
}

static void monke_inspect(std::vector<struct monke> *monkes, size_t monkeindex, bool part2) {
    if ((*monkes)[monkeindex].items.size() == 0) {
        return;
    }

    // this is firstly not my solution to part 2 and secondly i am frustrated as fuck rn so not going to implement it in a nice way
    // solution is by https://github.com/danielabadgamero
    if (part2) {
        ssize_t monkes_mul = (*monkes)[0].divtestnum;
        for (size_t i = 1; i < monkes->size(); i++) {
            monkes_mul *= (*monkes)[i].divtestnum;
        }
        (*monkes)[monkeindex].items[0] %= monkes_mul;
    }

    ssize_t opnum = (*monkes)[monkeindex].operation_value;
    if ((*monkes)[monkeindex].operation_self) {
        opnum = (*monkes)[monkeindex].items[0];
    }
    switch ((*monkes)[monkeindex].mathop) {
    case m_operator::ADD:
        (*monkes)[monkeindex].items[0] += opnum;
        break;
    case m_operator::MULTIPLY:
        (*monkes)[monkeindex].items[0] *= opnum;
        break;
    }
    if (!part2) {
        (*monkes)[monkeindex].items[0] /= 3;
    }

    if (((*monkes)[monkeindex].items[0] % (*monkes)[monkeindex].divtestnum) == 0) {
        (*monkes)[(*monkes)[monkeindex].monke_true].items.push_back((*monkes)[monkeindex].items[0]);
    } else {
        (*monkes)[(*monkes)[monkeindex].monke_false].items.push_back((*monkes)[monkeindex].items[0]);
    }

    (*monkes)[monkeindex].items.erase((*monkes)[monkeindex].items.begin()); // pop item from start
    (*monkes)[monkeindex].items_inspected++;

    monke_inspect(monkes, monkeindex, part2);
}

static void idfk(std::vector<struct monke> monkes, size_t rounds, bool part2 = false) {
    for (size_t i = 0; i < rounds; i++) {
        for (size_t j = 0; j < monkes.size(); j++) {
            monke_inspect(&monkes, j, part2);
        }
    }

    for (size_t i = 0; i < monkes.size(); i++) {
        printf("monke %llu (inspected %llu): ", i, monkes[i].items_inspected);
        for (size_t j = 0; j < monkes[i].items.size(); j++) {
            printf("%lld, ", monkes[i].items[j]);
        }
        printf("\n");
    }

    std::sort(monkes.begin(), monkes.end(), monkesort);

    if (!part2) {
        printf("solution part1: %lld\n", monkes[monkes.size() - 1].items_inspected * monkes[monkes.size() - 2].items_inspected);
    } else {
        printf("solution part2: %lld\n", monkes[monkes.size() - 1].items_inspected * monkes[monkes.size() - 2].items_inspected);
    }
}

int main(int argc, char *argv[]) {
    std::vector<struct monke> monkes_input;
    monkeparse(&monkes_input, "day11_input.txt");

    idfk(monkes_input, 20);
    idfk(monkes_input, 10000, true);
    return 0;
}
