import numpy as np
from numpy import random
import matplotlib.pyplot as plt

def sign(data):
    result = np.ones(len(data))
    for i in range(len(data)):
        if data[i] < 0:
            result[i] = -1
    return result

def generate_theta(data):
    sort_data = np.sort(data, axis=None)
    theta = np.zeros(len(data)+1)
    for i in range(len(theta)):
        if i == 0:
            theta[i] = (-1 + data[i])/2
        elif i == len(theta)-1:
            theta[i] = (data[i-1] + 1)/2
        else:
            theta[i] = (data[i-1] + data[i])/2
    return theta

def compute_error():
    turns = 1000
    data_num = 20
    E_in = 0
    E_out = 0
    for t in range(turns):
        data = random.uniform(-1, 1, data_num)
        theta = generate_theta(data)
        noise = sign(random.uniform(-0.2, 0.8, data_num))
        noise_y = noise*sign(data)
        all_ein = np.zeros((2,len(theta)))              #分成s=1和s=-1的e_in
        for i in range(len(theta)):
            result1 = noise_y*sign(data-theta[i])                
            all_ein[0][i] = (data_num-np.sum(result1))/(2*data_num)     #算出-1在裡面的比例
            all_ein[1][i] = (data_num-np.sum(-result1))/(2*data_num)
        min1, min2 = min(all_ein[0]), min(all_ein[1])
        s = 0
        best_theta = 0
        if min1 < min2:
            s = 1
            best_theta = theta[np.argmin(all_ein[0])]
        else:
            s = -1
            best_theta = theta[np.argmin(all_ein[1])]
        e_out = 0.5 + 0.3*s*(abs(best_theta)-1)
        e_in = np.min(all_ein)
        in_out.append(e_in - e_out)
        E_in += e_in
        E_out += e_out
    avg_in = E_in/turns
    avg_out = E_out/turns
    print(avg_in, avg_out)

if __name__=='__main__':
    in_out = []
    compute_error()
    in_out = np.array(in_out)
    plt.hist(in_out, bins=30)
    plt.title("Ein - Eout")
    plt.show()

