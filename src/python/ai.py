from subprocess import Popen, PIPE

p = Popen('./bin/dominion', shell=True, stdout=PIPE, stdin=PIPE)

for line in p.stdout:
    decoded = line.decode('utf-8')[:-1]
    # if decoded[0] == '*':
    print(decoded)
    if decoded == "response?":
        break

value = "-1" + '\n'
value = bytes(value, 'UTF-8')  # Needed in Python 3.
p.stdin.write(value)
p.stdin.flush()

for line in p.stdout:
    decoded = line.decode('utf-8')[:-1]
    print(decoded)
    if decoded == "response?":
        break

print("get here")
value = "copper" + '\n'
value = bytes(value, 'UTF-8')  # Needed in Python 3.
p.stdin.write(value)
p.stdin.flush()

for line in p.stdout:
    decoded = line.decode('utf-8')[:-1]
    print(decoded)
    if decoded == "response?":
        break

print("get here")
value = "copper" + '\n'
value = bytes(value, 'UTF-8')  # Needed in Python 3.
p.stdin.write(value)
p.stdin.flush()

for line in p.stdout:
    decoded = line.decode('utf-8')[:-1]
    print(decoded)
    if decoded == "response?":
        break

print("get here")
value = "copper" + '\n'
value = bytes(value, 'UTF-8')  # Needed in Python 3.
p.stdin.write(value)
p.stdin.flush()

for line in p.stdout:
    decoded = line.decode('utf-8')[:-1]
    print(decoded)
    if decoded == "response?":
        break
