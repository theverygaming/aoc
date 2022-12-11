#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define CRT_HEIGHT 6
#define CRT_WIDTH 40

static bool pixels[CRT_HEIGHT][CRT_WIDTH];

struct cpustate {
    size_t cycles;
    ssize_t xvalue;
    ssize_t crt_position;
} static cpustate;

/*
 * addx takes two cycles
 * noop takes a single cycle
 * crt draws one pixel every cycle
 * crt draws pixel when x is in range +- 1 of crt beam
 */

static bool get_crt_pixel(ssize_t crtpos) {
    if (crtpos < 0 || crtpos >= (CRT_HEIGHT * CRT_WIDTH)) {
        return false;
    }
    ssize_t x = crtpos % CRT_WIDTH;
    ssize_t y = crtpos / CRT_WIDTH;
    if ((x < CRT_WIDTH && x >= 0) && (y < CRT_HEIGHT && y >= 0)) {
        return pixels[y][x];
    }
    return false;
}

static void gen_frame(FILE *stream) {
    for (size_t i = cpustate.cycles; cpustate.cycles < (i + 240);) {
        cpustate.crt_position = (cpustate.cycles + 2) % (CRT_WIDTH * CRT_HEIGHT); // crt position will have advanced by 2 when we executed addx
        ssize_t xpos_base = cpustate.crt_position % CRT_WIDTH;
        ssize_t needed_xvalue = xpos_base - 10; // 2, 3, 4

        // i do not like this
        if (!get_crt_pixel(cpustate.crt_position - 1) && !get_crt_pixel(cpustate.crt_position) && !get_crt_pixel(cpustate.crt_position + 1)) { // ...
            needed_xvalue = xpos_base - 10;
        } else if (!get_crt_pixel(cpustate.crt_position - 1) && !get_crt_pixel(cpustate.crt_position) && get_crt_pixel(cpustate.crt_position + 1)) { // ..#
            needed_xvalue = xpos_base + 2;
        } else if (!get_crt_pixel(cpustate.crt_position - 1) && get_crt_pixel(cpustate.crt_position) && !get_crt_pixel(cpustate.crt_position + 1)) { // .#.
            needed_xvalue = xpos_base + 1;                                                                                                           // brain donno how, it's possible but a bit hard
        } else if (!get_crt_pixel(cpustate.crt_position - 1) && get_crt_pixel(cpustate.crt_position) && get_crt_pixel(cpustate.crt_position + 1)) {  // .##
            needed_xvalue = xpos_base + 1;
        } else if (get_crt_pixel(cpustate.crt_position - 1) && !get_crt_pixel(cpustate.crt_position) && !get_crt_pixel(cpustate.crt_position + 1)) { // #..
            needed_xvalue = xpos_base - 2;
        } else if (get_crt_pixel(cpustate.crt_position - 1) && !get_crt_pixel(cpustate.crt_position) && get_crt_pixel(cpustate.crt_position + 1)) { // #.#
            needed_xvalue = xpos_base + 0;                                                                                                          // brain donno how, it's possible but a bit hard
        } else if (get_crt_pixel(cpustate.crt_position - 1) && get_crt_pixel(cpustate.crt_position) && !get_crt_pixel(cpustate.crt_position + 1)) { // ##.
            needed_xvalue = xpos_base - 1;
        } else if (get_crt_pixel(cpustate.crt_position - 1) && get_crt_pixel(cpustate.crt_position) && get_crt_pixel(cpustate.crt_position + 1)) { // ###
            needed_xvalue = xpos_base + 0;
        }

        ssize_t addvalue = needed_xvalue - cpustate.xvalue;
        if (addvalue == 0 && false) { // using noop would be required to draw anything... but my brain smol
            fputs("noop\n", stream);
            cpustate.cycles += 1;
        } else {
            fprintf(stream, "addx %lld\n", addvalue);
            cpustate.xvalue += addvalue;
            cpustate.cycles += 2;
        }
    }
}

static void drawtest(FILE *stream) {
    for (int i = 0; i < (CRT_HEIGHT * CRT_WIDTH); i++) {
        memset(pixels, false, sizeof(pixels));
        for (int i = 0; i < rand() % 40; i++) {
            pixels[rand() % CRT_HEIGHT][rand() % CRT_WIDTH] = true;
        }
        gen_frame(stream);
    }
}

int main(int argc, char *argv[]) {
    memset(pixels, false, sizeof(pixels));

    cpustate = {
        .cycles = 0,
        .xvalue = 1,
        .crt_position = 0,
    };

    FILE *out = fopen("day10_gen.txt", "w");
    if (!out) {
        fprintf(stderr, "could not open output");
        return 1;
    }

    drawtest(out);

    fclose(out);
    return 0;
}
