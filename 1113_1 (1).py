import math as m

x = [7.2, 0.6, 0.1, 0.9, 1.7, 5.9, 0.2, 3.5, 5.8, 7.9, 0.5]

def quantil(x, q):
    xSorted = sorted(x)
    n = len(x)
    if m.ceil(n * q) == m.floor(n * q):
        return (xSorted[m.floor(n * q) - 1] + xSorted[m.floor(n * q)]) / 2
    else:
        return xSorted[m.floor(n * q)]

print(quantil(x, 0.25), quantil(x, 0.5), quantil(x, 0.75))
