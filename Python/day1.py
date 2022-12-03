
elves = []
with open('day1_input.txt') as f:
    lines = f.readlines()
    index = 0
    for line in lines:
        if(len(elves) < index + 1):
            elves.append(0)
        if(line == "\n"):
            index += 1
            continue
        elves[index] += int(line)

elves.sort()
elves.reverse()

print(elves[0])
print(elves[0] + elves[1] + elves[2])
