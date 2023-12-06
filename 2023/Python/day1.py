def digits(_t, p2):
    t = _t
    arr = []
    if p2:
        digits = {"one": 1, "two": 2, "three": 3, "four": 4, "five": 5, "six": 6, "seven": 7, "eight": 8, "nine": 9}
        pos = 0
        while pos < len(t):
            ppos = pos
            for k, v in digits.items():
                if t[pos].isdecimal():
                    arr.append(int(t[pos]))
                    pos += 1
                if t[pos:pos+len(k)] == k:
                    pos += len(k) - 1
                    arr.append(v)
            if ppos == pos:
                pos += 1
    else:
        for n in range(0, len(t)):
            if t[n].isdigit():
                arr.append(int(t[n]))
    return arr

with open("day1.txt") as f:
    sump1 = 0
    sump2 = 0
    for line in f.readlines():
        if line == "\n" or line == "":
            continue
        dp1 = digits(line, False)
        dp2 = digits(line, True)
        np1 = (dp1[0] * 10) + dp1[-1] if len(dp1) > 0 else 0
        np2 = (dp2[0] * 10) + dp2[-1] if len(dp2) > 0 else 0
        sump1 += np1
        sump2 += np2
        print(np2)
        print(dp2)
    print(f"solutions:\n  - part1: {sump1}\n  - part2: {sump2}")
