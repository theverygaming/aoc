#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

class fsentry { // should have used a struct
public:
    std::string name;
    enum class type { FILE, DIRECTORY } type;
    std::vector<fsentry> dirent; // only used if directory
    size_t size = 0;             // directory + subdir size for a directory, filesize for a file
};

static bool strcontains(std::string a, std::string b) {
    if (a.find(b) != std::string::npos) {
        return true;
    }
    return false;
}

static void printpath(std::vector<std::string> p) {
    printf("path: ");
    for (size_t i = 0; i < p.size(); i++) {
        printf(" |%s| ", p[i].c_str());
    }
    printf("\n");
}

static void print_tree(fsentry *fs) {
    printf("entering: %s (%u)\n", fs->name.c_str(), fs->size);
    for (size_t i = 0; i < fs->dirent.size(); i++) {
        if (fs->dirent[i].type == fsentry::type::DIRECTORY) {
            print_tree(&fs->dirent[i]);
        } else {
            printf("file: %s (%u)\n", fs->dirent[i].name.c_str(), fs->dirent[i].size);
        }
    }
    printf("exiting: %s\n", fs->name.c_str());
}

fsentry *getEntryForPath(std::vector<std::string> p, fsentry *fs) {
    fsentry *current_entry = fs;
    for (size_t i = 0; i < p.size(); i++) {
        if (current_entry->type == fsentry::type::DIRECTORY) {
            for (size_t j = 0; j < current_entry->dirent.size(); j++) {
                if (current_entry->dirent[j].name == p[i]) {
                    current_entry = &current_entry->dirent[j];
                    if (current_entry->name == p[i] && (i == (p.size() - 1))) {
                        return current_entry;
                    }
                    goto c; // https://xkcd.com/292/
                }
            }
            goto b;
        c:
            continue;
        b:
            break; // couldn't find
        } else {
            if (current_entry->name == p[i] && i == (p.size() - 1)) {
                return current_entry;
            }
        }
    }
    if (p.size() == 0) {
        return current_entry;
    }
    return nullptr;
}

size_t accumulate_dir_size_max(fsentry *fs, size_t max, bool recurse = false, size_t *accum = nullptr) {
    size_t totalsize = 0;
    if (fs->size <= max && recurse) {
        *accum += fs->size;
    }
    for (size_t i = 0; i < fs->dirent.size(); i++) {
        if (fs->dirent[i].type == fsentry::type::DIRECTORY) {
            if (!recurse) {
                accumulate_dir_size_max(&fs->dirent[i], max, true, &totalsize);
            } else {
                accumulate_dir_size_max(&fs->dirent[i], max, true, accum);
            }
        }
    }
    return totalsize;
}

size_t find_smallest_dir_min_size(fsentry *fs, size_t minsize, bool recurse = false, std::vector<size_t> *sizes_p = nullptr) {
    std::vector<size_t> sizes;

    if (fs->size >= minsize && recurse) {
        sizes_p->push_back(fs->size);
    }
    for (size_t i = 0; i < fs->dirent.size(); i++) {
        if (fs->dirent[i].type == fsentry::type::DIRECTORY) {
            if (!recurse) {
                find_smallest_dir_min_size(&fs->dirent[i], minsize, true, &sizes);
            } else {
                find_smallest_dir_min_size(&fs->dirent[i], minsize, true, sizes_p);
            }
        }
    }

    std::sort(sizes.begin(), sizes.end());
    if (sizes.size() > 0) {
        return sizes[0];
    }
    return 0;
}

size_t compute_directory_size(fsentry *fs) {
    size_t totalsize = 0;
    for (size_t i = 0; i < fs->dirent.size(); i++) {
        if (fs->dirent[i].type == fsentry::type::DIRECTORY) {
            totalsize += compute_directory_size(&fs->dirent[i]);
        } else {
            totalsize += fs->dirent[i].size;
        }
    }
    fs->size = totalsize;
    return totalsize;
}

static void iamgoodatnamingfunctions(std::string filename) {
    fsentry filesystem = {"root", fsentry::type::DIRECTORY};
    std::vector<std::string> path; // well this is an interesting design, but should allow for simpler code later on - this is about coding speed not performance
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

    for (size_t l = 0; l < lines.size(); l++) {
        printf("| %s\n", lines[l].c_str());
        if (lines[l][0] == '$') {
            if (lines[l] == "$ ls") {
            } else if (lines[l] == "$ cd ..") {
                path.pop_back();
            } else if (lines[l] == "$ cd /") {
                path.clear();
            } else if (strcontains(lines[l], "$ cd ")) {
                fsentry *prev = getEntryForPath(path, &filesystem);
                if (prev == nullptr) { // previous directory doesn't exist
                    exit(1);
                }
                char buf[100];
                sscanf(lines[l].c_str(), "$ cd %s", buf);
                path.push_back(buf);
                if (getEntryForPath(path, &filesystem) == nullptr) {
                    exit(1);
                    prev->dirent.push_back({buf, fsentry::type::DIRECTORY});
                    printpath(path);
                }
            }
        } else {
            if (strcontains(lines[l], "dir")) {
                char buf[100];
                sscanf(lines[l].c_str(), "dir %s", buf);
                fsentry *e = getEntryForPath(path, &filesystem);
                if (e == nullptr) { // previous directory doesn't exist
                    exit(1);
                }
                e->dirent.push_back({buf, fsentry::type::DIRECTORY});
                printf("created dir %s/%s/ in (below)\n", e->name.c_str(), buf);
                printpath(path);
            } else {
                char buf[100];
                size_t size;
                sscanf(lines[l].c_str(), "%llu %s", &size, buf);
                fsentry *e = getEntryForPath(path, &filesystem);
                if (e == nullptr) { // previous directory doesn't exist
                    exit(1);
                }
                e->dirent.push_back({buf, fsentry::type::FILE, {}, size});
                printf("created file %s (%u)\n", buf, size);
            }
        }
    }

    compute_directory_size(&filesystem);

    print_tree(&filesystem);

    printf("solution part1: %u\n", accumulate_dir_size_max(&filesystem, 100000));

    size_t neededspace = 30000000 - (70000000 - filesystem.size);
    printf("size required: %u\n", neededspace);
    printf("solution part2: %u\n", find_smallest_dir_min_size(&filesystem, neededspace));
}

int main(int argc, char *argv[]) {
    iamgoodatnamingfunctions("day7_input.txt");
    return 0;
}
