from subprocess import Popen, PIPE

def ResolveInput(process):
    input = []
    for line in process.stdout:
        decoded = line.decode('utf-8')[:-1]
        input.append(decoded)
        if decoded == "response?":
            return input

def find_between( s, first, last ):
    try:
        start = s.index( first ) + len( first )
        end = s.index( last, start )
        return s[start:end]
    except ValueError:
        return ""

def ParsePreliminary(gsm, index, input):
    preliminary = {}
    while index < len(input) and input[index][0] != '@':
        key = find_between(input[index], '*', ':')
        value = find_between(input[index], ': ', '*')
        preliminary[key] = value
        index += 1
    gsm["preliminary"] = preliminary
    return index

def ParseCards(gsm, index, input, name):
    cards = {}
    while index < len(input) and input[index][0] != '@':
        key = find_between(input[index], '*', ',')
        value = find_between(input[index], ', ', '*')
        cards[key] = value
        index += 1
    gsm[name] = cards
    return index


def InputToGameStateMap(input):
    gsm = {}
    for index in range(len(input)):
        if input[index] == "@PRELIMINARY@":
            index = ParsePreliminary(gsm, index + 1, input)
        if input[index] == "@PLAYER-DECK@":
            index = ParseCards(gsm, index + 1, input, "player-deck")
        if input[index] == "@PLAYER-HAND@":
            index = ParseCards(gsm, index + 1, input, "player-hand")
        if input[index] == "@PLAYER-DISCARD@":
            index = ParseCards(gsm, index + 1, input, "player-discard")
        if input[index] == "@KINGDOM@":
            index = ParseCards(gsm, index + 1, input, "kingdom")
        if input[index] == "@TRASH@":
            index = ParseCards(gsm, index + 1, input, "trash")
    return gsm

def PrintGameStateMap(gsm):
    for key1, value1 in gsm.items():
        print("**" + key1 + "**")
        for key2, value2 in value1.items():
            print(key2 + " : " + value2)

def RespondToEngine(process, move):
    value = str(move) + '\n'
    value = bytes(value, 'UTF-8')  # Needed in Python 3.
    process.stdin.write(value)
    process.stdin.flush()

def main():
    process = Popen('./bin/dominion', shell=True, stdout=PIPE, stdin=PIPE)

    input = ResolveInput(process)
    gsm = InputToGameStateMap(input)
    PrintGameStateMap(gsm)
    RespondToEngine(process, "copper") # dummy copper for now

main()
