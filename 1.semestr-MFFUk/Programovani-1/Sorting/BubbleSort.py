def bubbleSort(array):
    """
    Currently best implementation of bubble I know.
    """

    n = len(array)
    swaped = n
    while swaped > 1:
        new_swaped = 0
        for j in range(swaped-1):
            if(array[j] > array[j+1]):
                new_swaped = j+1
                array[j],array[j+1] = array[j+1], array[j]
        swaped = new_swaped
