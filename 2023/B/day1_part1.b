/* compile with https://github.com/Spydr06/BCause/tree/main */

main() {
    extrn open, nread, printf;
    auto f;
    f = open("day1.txt");

    auto char;
    char = 0;
    auto read;

    auto n1;
    auto n2;
    auto sum;
    n1 = -1;
    n2 = -1;
    sum = 0;

    while ((read = nread(f, &char, 1)) != 0) {
        if (char == '*n') {
            auto num;
            num = (n1 * 10) + n2;
            sum =+ num;
            printf("%d*n", num);
            /* reset numbers */
            n1 = -1;
            n2 = -1;
            goto continue;
        }

        /* ignore all non-digits */
        if ((char < 48) | (char > 57)) {
            goto continue;
        }
        
        /* convert ASCII character in char to digit */
        char = 48 - char;

        if (n1 == -1) {
            n1 = char;
        }
        n2 = char;

        continue:
        auto x; /* label without code after it breaks compiler */
    }
    printf("solution part1: %d*n", sum);
}
