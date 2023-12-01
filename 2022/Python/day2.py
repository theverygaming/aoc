types_opp = {
    "A": "rock",
    "B": "paper",
    "C": "scissors",
}

types_me = {
    "X": "rock",
    "Y": "paper",
    "Z": "scissors",
}

scores = {
    "rock": 1,
    "paper": 2,
    "scissors": 3,
}

def iswin(opponent, me):
    if(opponent == "rock" and me == "scissors"):
        return False
    if(opponent == "scissors" and me == "paper"):
        return False
    if(opponent == "paper" and me == "rock"):
        return False
    return True
    

totalscore = 0

with open('day2_input.txt') as f:
    lines = f.readlines()
    for line in lines:
        opp = types_opp[line[0]]
        me = types_me[line[2]]
        totalscore += scores[me]
        if(opp == me):
            totalscore += 3
        elif(iswin(opp, me)):
            totalscore += 6
        print(opp + " X " + me)

print(totalscore)
