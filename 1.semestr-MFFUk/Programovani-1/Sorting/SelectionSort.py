def selectionSort(array):
    n = len(array)
    for i in range(n-1):
        #i tells what number of min we look for
        minimal_i = i
        for j in range(i+1, n):
            if(array[minimal_i] > array[j]):
                minimal_i = j
        if(minimal_i != i):
            array[i], array[minimal_i] = array[minimal_i], array[i]
