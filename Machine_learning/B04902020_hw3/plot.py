import matplotlib.pyplot as plt
import sys

def read_data(filename, y):
    fp = open(filename, "r")
    line = fp.readline()
    while line:
        y.append(float(line))
        line = fp.readline()
    fp.close()

if len(sys.argv) != 3:
    print("argc error")
    sys.exit(0)

x = [i for i in range(1,2001)]
y1 = []
y2 = []
read_data(sys.argv[1], y1)
read_data(sys.argv[2], y2)
l1 = plt.plot(x, y1, '-o', ms=1)
l2 = plt.plot(x, y2, '-o', ms=1)
plt.xlabel("t")
plt.ylabel("Ein")
#plt.legend(handles = [l1, l2], labels = ['Gradient', 'Stochastic'], loc = 'best')
plt.show()
