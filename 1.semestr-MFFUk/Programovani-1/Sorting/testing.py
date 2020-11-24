import random as rd
import BubbleSort as bb
import InsertionSort as ins
import SelectionSort as sel
import HeapSort as hp

def isSorted(array):
    for i in range(len(array)-1):
        if(array[i] > array[i+1]):
            return False

    return True

def testAlgo(m,func):
    for i in range(m):
        length = rd.randint(1,500)
        array = []
        for j in range(length):
            array.append(rd.randint(0,500))
        old_array = array.copy()
        func(array)
        if not isSorted(array):
            print("old: ",old_array," new: ", array)
            return False

    return True

# print(testAlgo(500, bb.bubbleSort))
# print(testAlgo(500, ins.insertionSort))
# print(testAlgo(500, sel.selectionSort))
print(testAlgo(500, hp.heapSort))
