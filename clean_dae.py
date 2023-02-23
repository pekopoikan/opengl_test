import os
import re

file = [f for f in os.listdir(os.curdir + "/models") if f.__contains__(".dae")]

print("Cleaning", file)

for f in file:
    dae = open(os.curdir + "/models/" + f, 'r')
    Lines = dae.readlines()
    dae.close()

    dae = open(os.curdir + "/models/" + f, 'w')
    for line in Lines:
       match = re.search(r'<\w+[\" "+]?\/>', line) 
       if match is None:
        dae.write(line)
    dae.close()