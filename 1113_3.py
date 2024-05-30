x = [1.3, 2.6, 1.7, 0, 0.1, 2.7, 2.6, 2.6, 4.1, 0.7, 2.7, 4.8, 0.2]
y = [1.5, 0, 2.5, 0.7, 2.6, 2.7, 2, 1, 0, 1.7, 1.9, 2, 2.8]

zipped = zip(x, y)

def dispersion(xs):
    result = 0
    n = len(xs)
    mean = sum(xs) / n
    for x in xs:
        result += (x - mean) ** 2
    return result / (n - 1)

def correlation(xs, ys):
    xys = zip(xs, ys)
    n = 0
    S = 0
    Sx = 0
    Sy = 0
    for xy in xys:
        n += 1
        S += xy[0] * xy[1]
        Sx += xy[0]
        Sy += xy[1]
    S -= Sx / n * Sy
    S /= (n - 1) * (dispersion(xs) * dispersion(ys)) ** 0.5
    return S

print(round(correlation(x, y), 4))
