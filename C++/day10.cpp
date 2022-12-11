#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <thread>
#include <vector>

#define CRT_HEIGHT 6
#define CRT_WIDTH 40

// you cna choose one of these three
// #define PIXEL_TIME_MS 1 // how long it takes to draw one pixel in milliseconds
// #define PIXEL_TIME_US 300 // same thing but microseconds
#define FRAME_TIME_MS 100
// #define FRAME_TIME_US 1000

#define BLANK_CHARACTER '.'

#define PRINT_FRAME_ONLY // comment this to show the screen drawing live

static bool strcontains(std::string a, std::string b) {
    if (a.find(b) != std::string::npos) {
        return true;
    }
    return false;
}

struct cpustate {
    size_t cycles;
    ssize_t xvalue;
    ssize_t crt_position;
    char crtscreen[CRT_WIDTH * CRT_HEIGHT];
};

static void print_crt(struct cpustate *state) {
    for (size_t i = 0; i < (CRT_WIDTH * CRT_HEIGHT); i++) {
        if ((i % CRT_WIDTH) == 0 && i != 0) {
            printf("\n");
        }
        printf("%c", state->crtscreen[i]);
    }
    printf("\n");
}

static void clock_cycle(struct cpustate *state, size_t *signalstrengths) {
    state->cycles++;
    state->crt_position %= CRT_WIDTH * CRT_HEIGHT;
    // printf("cycle: %llu x: %lld crtpos: %llu\n", state->cycles, state->xvalue, state->crt_position);
    if (state->cycles == 20) {
        *signalstrengths += state->cycles * state->xvalue;
    } else if (state->cycles > 20 && ((state->cycles - 20) % 40) == 0) {
        *signalstrengths += state->cycles * state->xvalue;
    }

    ssize_t crt_x = state->xvalue + ((state->crt_position / CRT_WIDTH) * CRT_WIDTH);

    if (state->crt_position >= (crt_x - 1) && state->crt_position <= (crt_x + 1)) {
        state->crtscreen[state->crt_position] = '#';
    } else {
        state->crtscreen[state->crt_position] = BLANK_CHARACTER;
    }

    state->crt_position++;

#ifndef PRINT_FRAME_ONLY
    printf("\033[H\033[JCRT screen:\n");
    print_crt(state);
#else
    if (state->crt_position == ((CRT_WIDTH * CRT_HEIGHT) - 1)) {
        printf("\033[H\033[JCRT screen:\n");
        // printf("CRT screen:\n");
        print_crt(state);
#ifdef FRAME_TIME_MS
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_TIME_MS));
#endif
#ifdef FRAME_TIME_US
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_TIME_US));
#endif
    }
#endif // PRINT_FRAME_ONLY

#ifdef PIXEL_TIME_MS
    std::this_thread::sleep_for(std::chrono::milliseconds(PIXEL_TIME_MS));
#endif
#ifdef PIXEL_TIME_US
    std::this_thread::sleep_for(std::chrono::microseconds(PIXEL_TIME_US));
#endif
}

static void execute_instruction(const char *instruction, struct cpustate *state, size_t *signalstrengths) {
    ssize_t value;
    if (sscanf(instruction, "addx %lld", &value) == 1) {
        clock_cycle(state, signalstrengths);
        clock_cycle(state, signalstrengths);
        state->xvalue += value;
    } else { // noop
        clock_cycle(state, signalstrengths);
    }
}

static void part1(std::string filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        fprintf(stderr, "could not open file %s\n", filename.c_str());
        throw std::runtime_error("could not open file");
    }

    size_t signalstrengths = 0;

    struct cpustate state = {
        .cycles = 0,
        .xvalue = 1,
        .crt_position = 0,
    };

    memset(state.crtscreen, BLANK_CHARACTER, sizeof(state.crtscreen));

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

        execute_instruction(lines[l].c_str(), &state, &signalstrengths);
    }

    printf("solution part1: %llu\n", signalstrengths);
}

int main(int argc, char *argv[]) {
    part1("day10_gen.txt");
    // part1("day10_input.txt");
    return 0;
}
