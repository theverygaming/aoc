#include <algorithm>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

struct tile {
    enum class type_t {
        OBSTRUCTED,
        PATH,
        SLOPE_UP,
        SLOPE_DOWN,
        SLOPE_LEFT,
        SLOPE_RIGHT,
    } type;
    bool stepped;
};

struct step {
    enum class type_t {
        UP,
        DOWN,
        LEFT,
        RIGHT,
    } type;
};

struct path {
    std::vector<struct step> steps;
    std::vector<std::vector<struct tile>> map;
    int pos_x;
    int pos_y;
    bool deadend;
    bool completed;
};

static step::type_t tile_type_to_step_type(tile::type_t t) {
    switch (t) {
    case tile::type_t::SLOPE_UP:
        return step::type_t::UP;
    case tile::type_t::SLOPE_DOWN:
        return step::type_t::DOWN;
    case tile::type_t::SLOPE_LEFT:
        return step::type_t::LEFT;
    case tile::type_t::SLOPE_RIGHT:
        return step::type_t::RIGHT;
    default:
        throw std::runtime_error("tile_type_to_step_type impossible conversion");
    }
}

static tile::type_t char_to_tile_type(char c) {
    switch (c) {
    case '.':
        return tile::type_t::PATH;
    case '^':
        return tile::type_t::SLOPE_UP;
    case 'v':
        return tile::type_t::SLOPE_DOWN;
    case '<':
        return tile::type_t::SLOPE_LEFT;
    case '>':
        return tile::type_t::SLOPE_RIGHT;
    default:
        return tile::type_t::OBSTRUCTED;
    }
}

static char type_t_to_char(tile::type_t t) {
    switch (t) {
    case tile::type_t::PATH:
        return '.';
    case tile::type_t::SLOPE_UP:
        return '^';
    case tile::type_t::SLOPE_DOWN:
        return 'v';
    case tile::type_t::SLOPE_LEFT:
        return '<';
    case tile::type_t::SLOPE_RIGHT:
        return '>';
    default:
        return '#';
    }
}

static void print_map(std::vector<std::vector<struct tile>> &map) {
    for (std::vector<struct tile> &t : map) {
        for (struct tile &tl : t) {
            printf("%c", tl.stepped ? 'O' : type_t_to_char(tl.type));
        }
        printf("\n");
    }
}

static bool path_is_obstructed(struct path &p, tile::type_t direction) {
    switch (direction) {
    case tile::type_t::SLOPE_UP: {
        // can we check above?
        if (p.pos_y > 0) {
            // is it obstructed or has it been stepped on?
            return p.map[p.pos_y - 1][p.pos_x].type == tile::type_t::OBSTRUCTED || p.map[p.pos_y - 1][p.pos_x].stepped;
        }
        return true;
    }

    case tile::type_t::SLOPE_DOWN: {
        // can we check below?
        if ((p.pos_y + 1) < p.map.size()) {
            // is it obstructed or has it been stepped on?
            return p.map[p.pos_y + 1][p.pos_x].type == tile::type_t::OBSTRUCTED || p.map[p.pos_y + 1][p.pos_x].stepped;
        }
        return true;
    }

    case tile::type_t::SLOPE_LEFT: {
        // can we check left?
        if (p.pos_x > 0) {
            // is it obstructed or has it been stepped on?
            return p.map[p.pos_y][p.pos_x - 1].type == tile::type_t::OBSTRUCTED || p.map[p.pos_y][p.pos_x - 1].stepped;
        }
        return true;
    }

    case tile::type_t::SLOPE_RIGHT: {
        // can we check right?
        if ((p.pos_x + 1) < p.map[p.pos_y].size()) {
            // is it obstructed or has it been stepped on?
            return p.map[p.pos_y][p.pos_x + 1].type == tile::type_t::OBSTRUCTED || p.map[p.pos_y][p.pos_x + 1].stepped;
        }
        return true;
    }
    default: {
        return true;
    }
    }
}

static bool path_is_deadend(struct path &p) {
    switch (p.map[p.pos_y][p.pos_x].type) {
    case tile::type_t::PATH: {
        return path_is_obstructed(p, tile::type_t::SLOPE_UP) && path_is_obstructed(p, tile::type_t::SLOPE_DOWN) &&
               path_is_obstructed(p, tile::type_t::SLOPE_LEFT) && path_is_obstructed(p, tile::type_t::SLOPE_RIGHT);
    }

    // fallthrough
    case tile::type_t::SLOPE_UP:
    case tile::type_t::SLOPE_DOWN:
    case tile::type_t::SLOPE_LEFT:
    case tile::type_t::SLOPE_RIGHT: {
        return path_is_obstructed(p, p.map[p.pos_y][p.pos_x].type);
    }

    default: {
        return true;
    }
    }
}

static void walk_path_dir(struct path &p, tile::type_t direction) {
    if (path_is_obstructed(p, direction)) {
        throw std::runtime_error("attempted walk_path_dir in obstructed direction");
    }
    p.map[p.pos_y][p.pos_x].stepped = true;
    struct step step = {tile_type_to_step_type(direction)};
    p.steps.push_back(step);
    switch (direction) {
    case tile::type_t::SLOPE_UP: {
        p.pos_y--;
        break;
    }
    case tile::type_t::SLOPE_DOWN: {
        p.pos_y++;
        break;
    }
    case tile::type_t::SLOPE_LEFT: {
        p.pos_x--;
        break;
    }
    case tile::type_t::SLOPE_RIGHT: {
        p.pos_x++;
        break;
    }
    default: {
        throw std::runtime_error("attempted walk_path_dir in impossible direction");
    }
    }
    p.map[p.pos_y][p.pos_x].stepped = true;
}

static std::vector<struct path> find_paths(std::vector<std::vector<struct tile>> map) {
    struct path path_initial = {
        {},    // steps
        map,   // map
        0,     // pos_x
        0,     // pos_y
        false, // deadend
        false, // completed
    };
    // find entry point
    for (struct tile &tl : map[path_initial.pos_y]) {
        if (tl.type == tile::type_t::PATH) {
            break;
        }
        path_initial.pos_x++;
    }
    std::vector<struct path> paths;
    paths.push_back(path_initial);

    while (true) {
        int ncompleted = 0;
        std::vector<struct path> added_paths;
        for (struct path &p : paths) {
            // check if the path is completed and skip if it is
            if (p.pos_y == (map.size() - 1)) {
                ncompleted++;
                if (!p.completed) {
                    for (struct tile &tl : p.map[p.pos_y]) {
                        if (tl.type == tile::type_t::PATH) {
                            tl.stepped = true;
                        }
                    }
                    p.completed = true;
                }
                continue;
            }
            // check for a dead end
            if (path_is_deadend(p)) {
                p.deadend = true;
                continue;
            }

            // check in all possible directions
            bool foundp = false;
            tile::type_t foundd;

            tile::type_t possible_dirs[4] = {tile::type_t::SLOPE_UP, tile::type_t::SLOPE_DOWN, tile::type_t::SLOPE_LEFT, tile::type_t::SLOPE_RIGHT};

            for (int i = 0; i < (sizeof(possible_dirs) / sizeof(possible_dirs[0])); i++) {
                if (!path_is_obstructed(p, possible_dirs[i])) {
                    if (!foundp) {
                        foundd = possible_dirs[i];
                        foundp = true;
                    } else {
                        added_paths.push_back(p);
                        walk_path_dir(added_paths.back(), possible_dirs[i]);
                    }
                }
            }

            walk_path_dir(p, foundd);
        }
        for (struct path &p : added_paths) {
            paths.push_back(p);
        }
        // remove dead end paths
        paths.erase(std::remove_if(paths.begin(), paths.end(), [](const struct path &p) { return p.deadend; }), paths.end());
        /*for (struct path &p : paths) {
            printf("path:\n");
            print_map(p.map);
        }
        printf("--------------------------------------------------------\n");*/
        if (ncompleted == paths.size()) {
            break;
        }
    }

    return paths;
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

    std::vector<std::vector<struct tile>> map;

    //printf("raw input map:\n");
    for (size_t li = 0; li < lines.size(); li++) {
        if (lines[li].size() == 0) {
            continue;
        }
        std::vector<struct tile> mapline;
        for (char c : lines[li]) {
            tile::type_t t = char_to_tile_type(c);
            struct tile tl = {t, false};
            mapline.push_back(tl);
        }
        map.push_back(mapline);
        //printf("%s\n", lines[li].c_str());
    }
    //printf("parsed map:\n");
    //print_map(map);
    std::vector<struct path> paths = find_paths(map);
    std::sort(paths.begin(), paths.end(), [](const auto &lhs, const auto &rhs) { return lhs.steps.size() < rhs.steps.size(); });
    printf("found %zu possible paths\n", paths.size());
    /*for (struct path &p : paths) {
        printf("    %zu steps\n", p.steps.size());
    }*/
    printf("solution part 1: %zu", paths.back().steps.size());
}

int main() {
    part1("day23.txt");
    return 0;
}
