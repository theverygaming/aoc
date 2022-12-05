
#include <linux_x86/syscall.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static int getchar_fd(int fd, char *c) {
    if (sys_read(fd, c, 1) <= 0) {
        return -1;
    }
    return 0;
}

static int getline(int fd, char *buf, size_t bufsize) {
    bufsize -= 1;
    char c;
    size_t count = 0;
    do {
        if (getchar_fd(fd, &c) == -1) {
            break;
        }
        if (bufsize > 0 && c != '\n') {
            buf[count] = c;
            bufsize--;
            count++;
        }
    } while (c != '\n');
    if (count == 0) {
        return -1;
    }
    buf[count] = '\0';
    return 0;
}

bool rangefullycontains(unsigned long int a1, unsigned long int a2, unsigned long int b1, unsigned long int b2) {
    return (a1 <= b1 && a2 >= b2) || (b1 <= a1 && b2 >= a2);
}

unsigned long int max(unsigned long int a, unsigned long int b) {
    if (a > b) {
        return a;
    }
    return b;
}

unsigned long int min(unsigned long int a, unsigned long int b) {
    if (a < b) {
        return a;
    }
    return b;
}

bool rangeoverlap(unsigned long int a1, unsigned long int a2, unsigned long int b1, unsigned long int b2) {
    return min(a2, b2) >= max(a1, b1);
}

void part1(const char *file) {
    int fd = sys_open(file, O_RDONLY, 0);
    if (fd < 0) {
        printf("could not open input file\n");
        exit(1);
    }
    char line[100];
    uint32_t ranges_contained = 0;
    while (getline(fd, line, 100) != -1) {
        unsigned long int nums[4];
        sscanf(line, "%lu-%lu,%lu-%lu", &nums[0], &nums[1], &nums[2], &nums[3]);

        if (rangefullycontains(nums[0], nums[1], nums[2], nums[3])) {
            //printf("%u-%u,%u-%u\n", nums[0], nums[1], nums[2], nums[3]);
            ranges_contained++;
        }
    }
    printf("total part1: %u\n", ranges_contained);
}

void part2(const char *file) {
    int fd = sys_open(file, O_RDONLY, 0);
    if (fd < 0) {
        printf("could not open input file\n");
        exit(1);
    }
    char line[100];
    uint32_t ranges_overlapped = 0;
    while (getline(fd, line, 100) != -1) {
        unsigned long int nums[4];
        sscanf(line, "%lu-%lu,%lu-%lu", &nums[0], &nums[1], &nums[2], &nums[3]);

        if (rangeoverlap(nums[0], nums[1], nums[2], nums[3])) {
            //printf("%u-%u,%u-%u\n", nums[0], nums[1], nums[2], nums[3]);
            ranges_overlapped++;
        }
    }
    printf("total part2: %u\n", ranges_overlapped);
}

int main(int argc, char *argv[]) {
    part1("day4_input.txt");
    part2("day4_input.txt");
    return 0;
}
