#include <linux_x86/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char get_common_char(char *str1, char *str2) {
    char *ptr1 = str1;
    while (*ptr1) {
        for (size_t i = 0; i < strlen(str2); i++) {
            if (*ptr1 == str2[i]) {
                return *ptr1;
            }
        }
        ptr1++;
    }
    return 'h';
}

char get_common_char_3(char *str1, char *str2, char *str3) {
    char *ptr1 = str1;
    while (*ptr1) {
        for (size_t i = 0; i < strlen(str2); i++) {
            if (*ptr1 == str2[i]) {
                for (size_t i = 0; i < strlen(str3); i++) {
                    if (*ptr1 == str3[i]) {
                        return *ptr1;
                    }
                }
            }
        }
        ptr1++;
    }
    return 'h';
}

int get_priority(char c) {
    if (c >= 'a' && c <= 'z') {
        return (c - 'a') + 1;
    }
    if (c >= 'A' && c <= 'Z') {
        return (c - 'A') + 27;
    }
    return 0;
}

void part1() {
    int fd = sys_open("day3_input.txt", O_RDONLY, 0);
    if (fd < 0) {
        printf("could not open input file\n");
        exit(1);
    }
    char line[100];
    int total_priority = 0;
    while (getline(fd, line, 100) != -1) {
        int half = strlen(line) / 2;
        char h1[half + 1];
        char h2[half + 1];
        h1[half] = '\0';
        h2[half] = '\0';
        memcpy(h1, line, half);
        memcpy(h2, &line[half], half);

        char common = get_common_char(h1, h2);
        int priority = get_priority(common);

        // printf("%u %s %s common: %c pri: %u\n", half, h1, h2, common, priority);
        total_priority += priority;
    }
    printf("part 1 -- total priority: %u\n", total_priority);
}

void part2() {
    int fd = sys_open("day3_input.txt", O_RDONLY, 0);
    if (fd < 0) {
        printf("could not open input file\n");
        exit(1);
    }
    char line1[100];
    char line2[100];
    char line3[100];
    int total_priority = 0;
    while (1) {
        if ((getline(fd, line1, 100) + getline(fd, line2, 100) + getline(fd, line3, 100)) != 0) {
            break;
        }
        char common = get_common_char_3(line1, line2, line3);
        int priority = get_priority(common);
        total_priority += priority;
    }
    printf("part 2 -- total priority: %u\n", total_priority);
}

int main(int argc, char *argv[]) {
    part1();
    part2();
    return 0;
}
