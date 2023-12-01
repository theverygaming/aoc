types_opp = {
    "A": "rock",
    "B": "paper",
    "C": "scissors",
}

types_me = {
    "X": "lose",
    "Y": "draw",
    "Z": "win",
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

def winshape(oshape):
    if(oshape == "rock"):
        return "paper"
    if(oshape == "paper"):
        return "scissors"
    if(oshape == "scissors"):
        return "rock"

def looseshape(oshape):
    if(oshape == "rock"):
        return "scissors"
    if(oshape == "paper"):
        return "rock"
    if(oshape == "scissors"):
        return "paper"

totalscore = 0

with open('day2_input.txt') as f:
    lines = f.readlines()
    for line in lines:
        opp = types_opp[line[0]]
        outcome  = types_me[line[2]]
        me = ""
        if(outcome == "win"):
            me = winshape(opp)
        if(outcome == "draw"):
            me = opp
        if(outcome == "lose"):
            me = looseshape(opp)
        totalscore += scores[me]
        if(opp == me):
            totalscore += 3
        elif(iswin(opp, me)):
            totalscore += 6
        print(opp + " X " + me)

print(totalscore)
