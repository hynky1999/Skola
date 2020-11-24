import random
def left(i):
    return i*2+1

def right(i):
    return i*2+2

def parent(i):
    return (i-1)//2

# def createHeap(array):
#     for i in range(1,len(array)):
#         parent_i = (i-1)//2
#         child_i = i
#         while(array[parent_i] < array[child_i]):
#             array[parent_i],array[child_i] = array[child_i], array[parent_i]

def isHeap(array):
    for i in range(len(array)):
        l = left(i)
        r = right(i)
        p = parent(i)
        if(l < len(array) and array[l] > array[p]):
            return False
        if(r < len(array) and array[r] > array[p]):
            return False
    return True

def createHeapDownTop(array):
    for i in range(len(array)-1,0,-1):
        p = parent(i)
        if(array[p] < array[i]):
            array[p],array[i] = array[i], array[p]


def isSorted(array):
    for i in range(1, len(array)-1):
        if array[i] > array[i+1]:
            return False


def test(tries, fn, check):
    size = random.randint(3, 300)
    old_array = []
    for(i in range(size)):
        old_array.append(random.randint(0,300))
    new_array = old_array.copy()
    

def bubbleDown(array, begin, end):
    i = begin
    l = left(i)
    r = right(i)
    while(i < end):
        swap = i
        if(array[l] > array[i]):
            swap = l
        if(arra[r] > array[l]):
            swap = r
        if(swap != i):
            array[swap], array[i] = array[i], array[swap]
            i = swap



