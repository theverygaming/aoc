#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

class tile_map {
public:
    tile_map() {
        _width = 0;
        _height = 0;
        size_t map_bytes = (((_width * _height) * 2) / 8) + 1;
        ptr = (uint8_t *)malloc(map_bytes);
        memset(ptr, 0, map_bytes);
    }

    tile_map(size_t width, size_t height) {
        _width = width;
        _height = height;
        size_t map_bytes = (((_width * _height) * 2) / 8) + 1;
        ptr = (uint8_t *)malloc(map_bytes);
        memset(ptr, 0, map_bytes);
    }

    tile_map(const tile_map &c) {
        _width = c._width;
        _height = c._height;
        size_t map_bytes = (((_width * _height) * 2) / 8) + 1;
        ptr = (uint8_t *)malloc(map_bytes);
        memcpy(ptr, c.ptr, map_bytes);
    }

    tile_map(tile_map &&mv) {
        _width = mv._width;
        _height = mv._height;
        ptr = mv.ptr;
        mv.ptr = nullptr;
    }

    tile_map &operator=(const tile_map &c) {
        free(ptr);

        _width = c._width;
        _height = c._height;
        size_t map_bytes = (((_width * _height) * 2) / 8) + 1;
        ptr = (uint8_t *)malloc(map_bytes);
        memcpy(ptr, c.ptr, map_bytes);

        return *this;
    }

    ~tile_map() {
        free(ptr);
    }

    bool get_obstructed(size_t x, size_t y) {
        check_bounds(x, y);
        size_t idx = (y * _width) + x;
        size_t byte = ((idx * 2) / 8) + 0;
        size_t bit = ((idx * 2) % 8) + 0;
        return bitget(ptr[byte], bit) != 0;
    }

    bool get_stepped(size_t x, size_t y) {
        check_bounds(x, y);
        size_t idx = (y * _width) + x;
        size_t byte = ((idx * 2) / 8) + 1;
        size_t bit = ((idx * 2) % 8) + 1;
        return bitget(ptr[byte], bit) != 0;
    }

    void set_obstructed(size_t x, size_t y, bool state) {
        check_bounds(x, y);
        size_t idx = (y * _width) + x;
        size_t byte = ((idx * 2) / 8) + 0;
        size_t bit = ((idx * 2) % 8) + 0;
        bitset(&ptr[byte], bit, state ? 1 : 0);
    }

    void set_stepped(size_t x, size_t y, bool state) {
        check_bounds(x, y);
        size_t idx = (y * _width) + x;
        size_t byte = ((idx * 2) / 8) + 1;
        size_t bit = ((idx * 2) % 8) + 1;
        bitset(&ptr[byte], bit, state ? 1 : 0);
    }

    size_t width() {
        return _width;
    }

    size_t height() {
        return _height;
    }

private:
    uint8_t bitget(uint8_t byte, uint8_t bitnum) {
        return (byte >> bitnum) & 1;
    }

    void bitset(uint8_t *byte, uint8_t bitnum, uint8_t value) {
        *byte ^= (-value ^ *byte) & (1 << bitnum);
    }

    void check_bounds(size_t x, size_t y) {
        if (x >= _width) {
            fprintf(stderr, "tile_map x out of bounds\n");
            exit(1);
        }
        if (y >= _height) {
            fprintf(stderr, "tile_map y out of bounds\n");
            exit(1);
        }
    }

    uint8_t *ptr;

    size_t _width;
    size_t _height;
};

struct __attribute__((packed)) tile {
    enum class type_t {
        OBSTRUCTED,
        PATH,
        SLOPE_UP,
        SLOPE_DOWN,
        SLOPE_LEFT,
        SLOPE_RIGHT,
    } type : 4;
    bool stepped : 1;
};

struct path {
    unsigned long n_steps;
    tile_map tmap;
    int pos_x;
    int pos_y;
    bool deadend;
    bool completed;
};

static tile::type_t char_to_tile_type(char c) {
    switch (c) {
    case '^':
    case 'v':
    case '<':
    case '>':
    case '.':
        return tile::type_t::PATH;
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

static void print_map(tile_map &map) {
    for (int y = 0; y < map.height(); y++) {
        for (int x = 0; x < map.width(); x++) {
            printf("%c", map.get_stepped(x, y) ? 'O' : (map.get_obstructed(x, y) ? '#' : '.'));
        }
        printf("\n");
    }
}

static bool map_direction_is_obstructed(tile_map &map, size_t pos_x, size_t pos_y, tile::type_t direction) {
    switch (direction) {
    case tile::type_t::SLOPE_UP: {
        // can we check above?
        if (pos_y > 0) {
            // is it obstructed or has it been stepped on?
            return map.get_obstructed(pos_x, pos_y - 1) || map.get_stepped(pos_x, pos_y - 1);
        }
        return true;
    }

    case tile::type_t::SLOPE_DOWN: {
        // can we check below?
        if ((pos_y + 1) < map.height()) {
            // is it obstructed or has it been stepped on?
            return map.get_obstructed(pos_x, pos_y + 1) || map.get_stepped(pos_x, pos_y + 1);
        }
        return true;
    }

    case tile::type_t::SLOPE_LEFT: {
        // can we check left?
        if (pos_x > 0) {
            // is it obstructed or has it been stepped on?
            return map.get_obstructed(pos_x - 1, pos_y) || map.get_stepped(pos_x - 1, pos_y);
        }
        return true;
    }

    case tile::type_t::SLOPE_RIGHT: {
        // can we check right?
        if ((pos_x + 1) < map.width()) {
            // is it obstructed or has it been stepped on?
            return map.get_obstructed(pos_x + 1, pos_y) || map.get_stepped(pos_x + 1, pos_y);
        }
        return true;
    }
    default: {
        return true;
    }
    }
}

static bool path_is_obstructed(struct path &p, tile::type_t direction) {
    return map_direction_is_obstructed(p.tmap, p.pos_x, p.pos_y, direction);
}

static void walk_path_dir(struct path &p, tile::type_t direction) {
    if (path_is_obstructed(p, direction)) {
        throw std::runtime_error("attempted walk_path_dir in obstructed direction");
    }
    p.tmap.set_stepped(p.pos_x, p.pos_y, true);
    p.n_steps++;
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
    p.tmap.set_stepped(p.pos_x, p.pos_y, true);
}

static void dfs(tile_map i_map, std::vector<unsigned long> &found_path_stepnums, size_t pos_x, size_t pos_y) {
    std::vector<std::tuple<tile_map, unsigned long, size_t, size_t>> stack;

    stack.push_back({i_map, 0, pos_x, pos_y});

    while (stack.size() != 0) {
        std::tuple<tile_map, unsigned long, size_t, size_t> tp = stack.back();
        stack.pop_back();
        tile_map map = std::get<0>(tp);
        unsigned long depth = std::get<1>(tp);
        size_t pos_x_s = std::get<2>(tp);
        size_t pos_y_s = std::get<3>(tp);

        if (pos_y_s == (map.height() - 1)) {
            found_path_stepnums.push_back(depth);
            if (found_path_stepnums.size() % 512 == 0) {
                printf("%zu paths found.. stack size %zu\n", found_path_stepnums.size(), stack.size());
            }
        }

        tile::type_t possible_dirs[4] = {tile::type_t::SLOPE_UP, tile::type_t::SLOPE_DOWN, tile::type_t::SLOPE_LEFT, tile::type_t::SLOPE_RIGHT};
        for (int i = 0; i < (sizeof(possible_dirs) / sizeof(possible_dirs[0])); i++) {
            if (!map_direction_is_obstructed(map, pos_x_s, pos_y_s, possible_dirs[i])) {
                map.set_stepped(pos_x_s, pos_y_s, true);
                switch (possible_dirs[i]) {
                case tile::type_t::SLOPE_UP: {
                    if (!map.get_stepped(pos_x_s, pos_y_s - 1)) {
                        stack.push_back({map, depth + 1, pos_x_s, pos_y_s - 1});
                        map.set_stepped(pos_x_s, pos_y_s - 1, true);
                    }
                    break;
                }
                case tile::type_t::SLOPE_DOWN: {
                    if (!map.get_stepped(pos_x_s, pos_y_s + 1)) {
                        stack.push_back({map, depth + 1, pos_x_s, pos_y_s + 1});
                        map.set_stepped(pos_x_s, pos_y_s + 1, true);
                    }
                    break;
                }
                case tile::type_t::SLOPE_LEFT: {
                    if (!map.get_stepped(pos_x_s - 1, pos_y_s)) {
                        stack.push_back({map, depth + 1, pos_x_s - 1, pos_y_s});
                        map.set_stepped(pos_x_s - 1, pos_y_s, true);
                    }
                    break;
                }
                case tile::type_t::SLOPE_RIGHT: {
                    if (!map.get_stepped(pos_x_s + 1, pos_y_s)) {
                        stack.push_back({map, depth + 1, pos_x_s + 1, pos_y_s});
                        map.set_stepped(pos_x_s + 1, pos_y_s, true);
                    }
                    break;
                }
                }
            }
        }
    }
}

static std::vector<unsigned long> find_path_stepnums(tile_map map) {
    std::vector<unsigned long> found_path_stepnums;
    // find entry point
    size_t pos_x = 0;
    for (int x = 0; x < map.width(); x++) {
        if (!map.get_obstructed(x, 0)) {
            break;
        }
        pos_x++;
    }
    dfs(map, found_path_stepnums, pos_x, 0);
    return found_path_stepnums;
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
            if (currentLine.size() == 0) {
                continue;
            }
            lines.push_back(currentLine);
        }
    }

    tile_map map(lines[0].size(), lines.size());

    //printf("raw input map:\n");
    for (size_t li = 0; li < lines.size(); li++) {
        std::vector<struct tile> mapline;
        for (size_t ci = 0; ci < lines[li].size(); ci++) {
            tile::type_t t = char_to_tile_type(lines[li][ci]);
            map.set_obstructed(ci, li, t == tile::type_t::OBSTRUCTED);
            map.set_stepped(ci, li, false);
        }
        //printf("%s\n", lines[li].c_str());
    }
    printf("parsed map:\n");
    print_map(map);
    std::vector<unsigned long> paths = find_path_stepnums(map);
    std::sort(paths.begin(), paths.end());
    printf("found %zu possible paths\n", paths.size());
    if (paths.size() > 0) {
        printf("solution: %lu", paths.back());
    }
}

int main() {
    part2("day23.txt");
    return 0;
}
