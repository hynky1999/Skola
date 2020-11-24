from sys import stdin
def bucket_sort(arr, k, key):
    occurances = [0] * (k+1)
    for p in arr:
        occurances[key(p)] += 1

    sumup = 0
    for i in range(len(occurances)):
        occurances[i], sumup = sumup, sumup + occurances[i]

    b = [0] * len(arr)
    for p in arr:
        b[occurances[key(p)]] = p
        occurances[key(p)] += 1
    return b

def key_wrapper(shift):

    def key(x):
        for i in range(shift):
             x = x // 10

        return x % 10

    return key

def radix_sort(arr, d, k, key_wrapper):
    for shift in range(d):
        arr = bucket_sort(arr, k, key_wrapper(shift))
        print(' '.join(map(str, arr)))
    return arr


array = [1,10,2,1000]
array = radix_sort(array, 7, 10, key_wrapper)
