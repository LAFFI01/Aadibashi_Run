with open("web/game.js", "r") as f:
    lines = f.readlines()

for i, line in enumerate(lines):
    if "function playerCaught()" in line:
        for j in range(i, i+30):
            print(lines[j], end="")
        break
