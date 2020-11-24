def left(i):
    return 2*i+1

def right(i):
    return 2*i+2

def parent(i):
    return (i-1)//2

def createHeap1(array):
    n = len(array)
    for i in range(1,n):
        bubbleUp(array, i)

def createHeap2(array):
    n = len(array)


def bubbleUp(array, i):
    parent_i = parent(i)
    while array[parent_i] < array[i] and parent_i >= 0:
        array[parent_i], array[i] = array[i],array[parent_i]
        i = parent_i
        parent_i = parent(i)


def bubbleDown(array, i, end):
    while i < end:
        l = left(i)
        r = right(i)
        swap = i
        if l < end and array[i] < array[l]:
            swap = l
        if r < end and array[swap] < array[r]:
            swap = r

        if swap == i:
            break
        array[swap], array[i] = array[i], array[swap]
        i = swap


def checkHeap(array, end):
    for i in range(end):
        
        if left(i) < end and array[left(i)] > array[i]:
            return False
        if right(i) < end and array[right(i)] > array[i]:
            return False
    return True

def removeHeap(array):
    n = len(array)
    for i in range(n-1):
        array[n-1-i],array[0] = array[0], array[n-1-i]
        bubbleDown(array, 0, n-1-i)


def heapSort(array):
    createHeap1(array)
    removeHeap(array)

arr = [193, 408, 41, 178, 2, 82, 188, 166, 148, 69]
