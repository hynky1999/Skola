length, *permutation = map(int,input().split())
inverz = [0] * length
for start,end in enumerate(permutation):
    inverz[end-1] = start+1
print(" ".join(map(str,inverz)))

