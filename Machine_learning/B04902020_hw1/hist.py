import numpy as np
import random
from matplotlib import pyplot as plt

fp = open("out.txt", "r")
line = fp.readline()
bins = list(range(20, 80, 2))
y = []

while line:
    last = line
    y.append(int(line))
    line = fp.readline()

y = y[:-1]
plt.hist(y, bins=bins)
plt.xlabel("update number")
plt.ylabel("frequency")
plt.show()


