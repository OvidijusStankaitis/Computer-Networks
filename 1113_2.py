import numpy as np

P = 22
x = [1.9, 9.8, 5.7, 7.6, 8.7, 4.3, 2.2, 8.1, 4.5, 9.4, 6.3, 1.9, 8.4, 2, 5.1, 3.9]

def variance(xs):
    result = 0
    n = len(xs)
    mean = sum(xs) / n
    for x in xs:
        result += (x - mean) ** 2
    return result / (n - 1)

n = len(x)
p = P / 100.0
mean = np.mean(x)

# (new - mean) / (n + 1) + mean = newMean
new = mean * (n * p + p + 1)
newMean = mean * (1.0 + p)
newX = x.copy()
newX.append(new)

print(new, variance(newX) - variance(x))