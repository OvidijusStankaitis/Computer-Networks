def iqv(xs, uniqueCount):
    counts = {}
    totalCount = 0
    for x in xs:
        totalCount += 1
        xCount = counts.get(x)
        if xCount is None:
            counts[x] = 1
        else:
            counts[x] = xCount + 1
    multi = 1
    for xCount in counts.values():
        multi -= (xCount / totalCount) ** 2
    res = uniqueCount / (uniqueCount - 1) * multi
    print(counts, totalCount)
    return res

x = "BACCCBCCAEB"

print(iqv(x, 5))
