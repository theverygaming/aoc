#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#define INITIAL_GRID_SIZE_Y 21
#define INITIAL_GRID_SIZE_X 26

struct point {
    int x;
    int y;
};

enum class direction { UP, DOWN, LEFT, RIGHT, UPPER_RIGHT, UPPER_LEFT, LOWER_RIGHT, LOWER_LEFT };
std::map<char, direction> direction_map = {{'R', direction::RIGHT}, {'L', direction::LEFT}, {'U', direction::UP}, {'D', direction::DOWN}};

static void print_grid(std::vector<struct point> *rope, size_t size_x, size_t size_y, std::vector<struct point> *tailpositions) {
    char(*grid)[size_x] = (char(*)[size_x])malloc(size_x * size_y);
    memset(grid, '.', size_y * size_x);

    for (ssize_t i = rope->size() - 1; i > 0; i--) {
        grid[(*rope)[i].y][(*rope)[i].x] = '0' + i;
    }

    grid[(*rope)[0].y][(*rope)[0].x] = 'H';

    if (tailpositions != nullptr) {
        for (size_t i = 0; i < tailpositions->size(); i++) {
            grid[(*tailpositions)[i].y][(*tailpositions)[i].x] = '#';
        }
    }

    for (size_t y = 0; y < size_y; y++) {
        for (size_t x = 0; x < size_x; x++) {
            printf("%c", grid[y][x]);
        }
        printf("\n");
    }

    printf("\n");

    free(grid);
}

static size_t get_point_distance(struct point *a, struct point *b) {
    return sqrt(pow(b->x - a->x, 2) + pow(b->y - a->y, 2));
}

static direction get_direction(struct point *tail, struct point *head) {
    if (tail->x == head->x) { // vertical
        if (tail->y < head->y) {
            return direction::DOWN;
        }
        return direction::UP;
    } else if (tail->y == head->y) { // horizontal
        if (tail->x < head->x) {
            return direction::RIGHT;
        }
        return direction::LEFT;
    }
    float angle = atan2(head->y - tail->y, head->x - tail->x) * (180 / M_PI);
    /*
     * negative angle -> up, 1-89° right, 91-179° left
     * positive angle -> down, 1-89° right, 91-179° left
     */
    if (angle < 0) {
        if (-angle > 90) {
            return direction::UPPER_LEFT;
        }
        return direction::UPPER_RIGHT;
    } else {
        if (angle > 90) {
            return direction::LOWER_LEFT;
        }
        return direction::LOWER_RIGHT;
    }
    exit(1);
}

static void adjust_tail(struct point *tail, struct point *head) {
    if (get_point_distance(tail, head) > 1) {
        switch (get_direction(tail, head)) {
        case direction::UP:
            tail->y--;
            break;
        case direction::DOWN:
            tail->y++;
            break;
        case direction::LEFT:
            tail->x--;
            break;
        case direction::RIGHT:
            tail->x++;
            break;
        case direction::UPPER_RIGHT:
            tail->y--;
            tail->x++;
            break;
        case direction::UPPER_LEFT:
            tail->y--;
            tail->x--;
            break;
        case direction::LOWER_RIGHT:
            tail->y++;
            tail->x++;
            break;
        case direction::LOWER_LEFT:
            tail->y++;
            tail->x--;
            break;
        }
    }
}

static void add_tail_position(struct point *tail, std::vector<struct point> *tailpositions) {
    // couldn't get the stdlib stuff to work sooo
    for (size_t i = 0; i < tailpositions->size(); i++) {
        if (memcmp(tail, &(*tailpositions)[i], sizeof(struct point)) == 0) {
            return;
        }
    }
    tailpositions->push_back(*tail);
}

static void move_head(direction dir, size_t count, std::vector<struct point> *rope, std::vector<struct point> *tailpositions) {
    for (size_t i = 0; i < count; i++) {
        switch (dir) {
        case direction::UP:
            (*rope)[0].y--;
            break;
        case direction::DOWN:
            (*rope)[0].y++;
            break;
        case direction::LEFT:
            (*rope)[0].x--;
            break;
        case direction::RIGHT:
            (*rope)[0].x++;
            break;
        }
        for (size_t i = 1; i < rope->size(); i++) {
            adjust_tail(&(*rope)[i], &(*rope)[i - 1]);
        }
        add_tail_position(&(*rope)[rope->size() - 1], tailpositions);
    }
    //print_grid(rope, INITIAL_GRID_SIZE_X, INITIAL_GRID_SIZE_Y, nullptr);
}

static void part1(std::string filename) {
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

    std::vector<struct point> rope;
    rope.resize(2);
    for (size_t i = 0; i < rope.size(); i++) {
        rope[i] = {.x = 0, .y = INITIAL_GRID_SIZE_Y - 1};
    }

    size_t size_x = INITIAL_GRID_SIZE_X;
    size_t size_y = INITIAL_GRID_SIZE_Y;

    std::vector<struct point> tailpositions;

    add_tail_position(&rope[1], &tailpositions);

    for (size_t l = 0; l < lines.size(); l++) {
        if (lines[l].size() == 0) {
            continue;
        }

        size_t count;
        char dir_c;
        sscanf(lines[l].c_str(), "%c %llu", &dir_c, &count);
        direction dir = direction_map[dir_c];
        move_head(dir, count, &rope, &tailpositions);
    }

    // print_grid(&start, &tail, &head, size_x, size_y, &tailpositions);
    printf("solution part1: %llu\n", tailpositions.size());
}

static void part2(std::string filename) {
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

    std::vector<struct point> rope;
    rope.resize(10);
    for (size_t i = 0; i < rope.size(); i++) {
        rope[i] = {.x = 11, .y = 15};
    }

    size_t size_x = INITIAL_GRID_SIZE_X;
    size_t size_y = INITIAL_GRID_SIZE_Y;

    std::vector<struct point> tailpositions;

    add_tail_position(&rope[1], &tailpositions);

    for (size_t l = 0; l < lines.size(); l++) {
        if (lines[l].size() == 0) {
            continue;
        }

        size_t count;
        char dir_c;
        sscanf(lines[l].c_str(), "%c %llu", &dir_c, &count);
        direction dir = direction_map[dir_c];
        move_head(dir, count, &rope, &tailpositions);
    }

    //print_grid(&rope, size_x, size_y, &tailpositions);
    printf("solution part2: %llu\n", tailpositions.size());
}

int main(int argc, char *argv[]) {
    part1("day9_input.txt");
    part2("day9_input.txt");
    return 0;
}
