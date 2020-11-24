def insertionSort(array):
    n = len(array)
    # First i is sorted
    for i in range(1,n):
        swaped = array[i]
        for j in range(i,0,-1):
            #j is possible insertion
            if(array[j-1] > swaped):
                array[j] = array[j-1]
            else:
                array[j] = swaped
                break
