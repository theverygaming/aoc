#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

class tile_map {
public:
  tile_map(size_t width = 0, size_t height = 0) {
    _width = width;
    _height = height;
    size_t map_bytes = ((_width * _height) / 8) + 1;
    ptr = (uint8_t *)malloc(map_bytes);
    memset(ptr, 0, map_bytes);
  }

  tile_map(size_t width, size_t height, void *raw_data) {
    _width = width;
    _height = height;
    ptr = (uint8_t *)raw_data;
    ptr_is_raw = true;
  }

  tile_map(const tile_map &c) {
    _width = c._width;
    _height = c._height;
    size_t map_bytes = ((_width * _height) / 8) + 1;
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
    _width = c._width;
    _height = c._height;
    size_t map_bytes = ((_width * _height) / 8) + 1;
    ptr = (uint8_t *)malloc(map_bytes);
    memcpy(ptr, c.ptr, map_bytes);

    return *this;
  }

  ~tile_map() {
    if (!ptr_is_raw) {
      free(ptr);
    }
  }

  bool get_obstructed(size_t x, size_t y) {
    check_bounds(x, y);
    size_t idx = (y * _width) + x;
    size_t byte = (idx / 8) + 0;
    size_t bit = (idx % 8) + 0;
    return bitget(ptr[byte], bit) != 0;
  }

  void set_obstructed(size_t x, size_t y, bool state) {
    check_bounds(x, y);
    size_t idx = (y * _width) + x;
    size_t byte = (idx / 8) + 0;
    size_t bit = (idx % 8) + 0;
    bitset(&ptr[byte], bit, state ? 1 : 0);
  }

  size_t width() { return _width; }

  size_t height() { return _height; }

  std::tuple<void *, size_t> get_raw_data() {
    size_t map_bytes = ((_width * _height) / 8) + 1;
    return {ptr, map_bytes};
  }

private:
  uint8_t bitget(uint8_t byte, uint8_t bitnum) { return (byte >> bitnum) & 1; }

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
  bool ptr_is_raw = false;
};

struct __attribute__((packed)) tile {
  enum class type_t {
    OBSTRUCTED,
    PATH,
    SLOPE_UP,
    SLOPE_DOWN,
    SLOPE_LEFT,
    SLOPE_RIGHT,
  };
};

enum class tile_type_t {
  OBSTRUCTED,
  PATH,
  SLOPE_UP,
  SLOPE_DOWN,
  SLOPE_LEFT,
  SLOPE_RIGHT,
};

struct path {
  unsigned long n_steps;
  tile_map tmap;
  int pos_x;
  int pos_y;
  bool deadend;
  bool completed;
};

static tile_type_t char_to_tile_type(char c) {
  switch (c) {
  case '^':
  case 'v':
  case '<':
  case '>':
  case '.':
    return tile_type_t::PATH;
  default:
    return tile_type_t::OBSTRUCTED;
  }
}

static char type_t_to_char(tile_type_t t) {
  switch (t) {
  case tile_type_t::PATH:
    return '.';
  case tile_type_t::SLOPE_UP:
    return '^';
  case tile_type_t::SLOPE_DOWN:
    return 'v';
  case tile_type_t::SLOPE_LEFT:
    return '<';
  case tile_type_t::SLOPE_RIGHT:
    return '>';
  default:
    return '#';
  }
}

static bool map_direction_is_obstructed(tile_map &map, size_t pos_x,
                                        size_t pos_y, tile_type_t direction) {
  switch (direction) {
  case tile_type_t::SLOPE_UP: {
    // can we check above?
    if (pos_y > 0) {
      // is it obstructed or has it been stepped on?
      return map.get_obstructed(pos_x, pos_y - 1);
    }
    return true;
  }

  case tile_type_t::SLOPE_DOWN: {
    // can we check below?
    if ((pos_y + 1) < map.height()) {
      // is it obstructed or has it been stepped on?
      return map.get_obstructed(pos_x, pos_y + 1);
    }
    return true;
  }

  case tile_type_t::SLOPE_LEFT: {
    // can we check left?
    if (pos_x > 0) {
      // is it obstructed or has it been stepped on?
      return map.get_obstructed(pos_x - 1, pos_y);
    }
    return true;
  }

  case tile_type_t::SLOPE_RIGHT: {
    // can we check right?
    if ((pos_x + 1) < map.width()) {
      // is it obstructed or has it been stepped on?
      return map.get_obstructed(pos_x + 1, pos_y);
    }
    return true;
  }
  default: {
    return true;
  }
  }
}

static std::tuple<size_t, size_t> next_pos(size_t pos_x, size_t pos_y,
                                           const tile_type_t direction) {
  switch (direction) {
  case tile_type_t::SLOPE_UP:
    pos_y--;
    break;
  case tile_type_t::SLOPE_DOWN:
    pos_y++;
    break;
  case tile_type_t::SLOPE_LEFT:
    pos_x--;
    break;
  case tile_type_t::SLOPE_RIGHT:
    pos_x++;
    break;
  }

  return {pos_x, pos_y};
}

static void dfs(tile_map &i_map,
                std::vector<unsigned long> &found_path_stepnums, size_t pos_x,
                size_t pos_y, unsigned int calldepth = 0) {
  auto [i_map_ptr, i_map_size] = i_map.get_raw_data();
  void *mapraw = __builtin_alloca(i_map_size);
  memcpy(mapraw, i_map_ptr, i_map_size);
  tile_map map(i_map.width(), i_map.height(), mapraw);
  // fprintf(stderr, "calldepth: %u buf: %zu\n", calldepth, bufsz);

  if (pos_y == (map.height() - 1)) {
    found_path_stepnums.push_back(calldepth);
    if (found_path_stepnums.size() % 512 == 0) {
      fprintf(stderr, "%zu paths found.. stack size %zu\n",
              found_path_stepnums.size(), calldepth);
    }
    return;
  }

  for (auto dir : {tile_type_t::SLOPE_UP, tile_type_t::SLOPE_DOWN,
                   tile_type_t::SLOPE_LEFT, tile_type_t::SLOPE_RIGHT}) {
    if (!map_direction_is_obstructed(map, pos_x, pos_y, dir)) {
      map.set_obstructed(pos_x, pos_y, true);

      auto [np_x, np_y] = next_pos(pos_x, pos_y, dir);
      if (!map.get_obstructed(np_x, np_y)) {
        dfs(map, found_path_stepnums, np_x, np_y, calldepth + 1);
        map.set_obstructed(np_x, np_y, true);
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

static void part2() {
  std::istream &infile = std::cin;

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

  // printf("raw input map:\n");
  for (size_t li = 0; li < lines.size(); li++) {
    for (size_t ci = 0; ci < lines[li].size(); ci++) {
      tile_type_t t = char_to_tile_type(lines[li][ci]);
      map.set_obstructed(ci, li, t == tile_type_t::OBSTRUCTED);
    }
    // printf("%s\n", lines[li].c_str());
  }
  printf("loaded map\n");
  std::vector<unsigned long> paths = find_path_stepnums(map);
  std::sort(paths.begin(), paths.end());
  printf("found %zu possible paths\n", paths.size());
  if (paths.size() > 0) {
    printf("solution: %lu", paths.back());
  }
}

int main() {
  part2();
  return 0;
}
