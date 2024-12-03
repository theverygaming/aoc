import re

INPUT = "input.txt"

# part 1
with open(INPUT, "r") as f:
    contents = str(f.read())
    #print(contents)
    match = re.finditer(r"mul\(((\d+),(\d+))\)", contents, re.M)
    total = 0
    for m in match:
        #print(m)
        #print(m.groups())
        total += int(m.group(2)) * int(m.group(3))
    print(total)

# part 2
with open(INPUT, "r") as f:
    contents = str(f.read())
    #print(contents)
    match = re.finditer(r"(mul\(((\d+),(\d+))\))|(do\(\))|(don't\(\))", contents, re.M)
    total = 0
    enabled = True
    for m in match:
        #print(m)
        #print(m.groups())
        if m.group(5):
            enabled = True
            continue
        if m.group(6):
            enabled = False
            continue
        if enabled:
            total += int(m.group(3)) * int(m.group(4))
    print(total)
