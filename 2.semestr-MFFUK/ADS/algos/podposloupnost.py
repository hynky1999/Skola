import unittest
import random
import math
from bisect import bisect
class Bin_Iterator():
    def __init__(self, length):
        self.length = length


    def __iter__(self):
        self.arr = [0 for _ in range(self.length)]
        return self

    def __next__(self):
        i = 0
        for i in range(self.length):

            if self.arr[i] == 0:
                self.arr[i] = 1
                return self.arr

            if self.arr[i] == 1:
                self.arr[i] = 0

        raise StopIteration


def dumb1(seq):
    maximum = 0
    for test_seq in Bin_Iterator(len(seq)):
        last = -math.inf
        local_maximum = 0
        for seq_i in range(len(test_seq)):
            if test_seq[seq_i] == 0:
                continue

            if last >= seq[seq_i]:
                break

            local_maximum += 1
            last = seq[seq_i]
        maximum = max(maximum, local_maximum)
    return maximum


def average_with_memo(seq):
    memo_list = [0 for _ in range(len(seq))]
    maximum = 0

    def max_from(index):
        d = 1
        for i in range(index + 1, len(seq)):
            new_d = d
            if seq[index] < seq[i]:
                if memo_list[i] > 0:
                    new_d = memo_list[i]

                else:
                    new_d = max_from(i) + 1
                    memo_list[i] = new_d

            d = max(new_d, d)

        return d

    for i in range(len(seq)):
        #TODO 
        maximum = max(max_from(i), maximum)

    return maximum


def fast(seq):
    if not seq:
        return 0
    length_min = [math.inf for _ in range(len(seq))]
    for num in seq:
        insertion_point = bisect(length_min, num)
        length_min[insertion_point] = num
    
    length_sub = 0
    for num in length_min:
        if num == math.inf:
            break
        length_sub += 1
    return length_sub




def average_no_memo(seq):
    arr_max_start = [1 for _ in range(len(seq))]
    maximum = 0

    for cur_i in range(len(seq)-1, -1, -1):
        d = 1
        for next_i in range(cur_i + 1, len(seq)):
            if seq[cur_i] < seq[next_i]:
                d = max(d, arr_max_start[next_i] + 1)
        arr_max_start[cur_i] = d
        maximum = max(maximum, d)

    return maximum


class TestAll(unittest.TestCase):
    def test_first(self):
        seq = [1,4,5,1,3,6,5,9]
        self.assertEqual(dumb1(seq), 5)

    def test_second(self):
        seq = [9,1, 7, 2, 1,4,5,1,3,6,5,9]
        self.assertEqual(average_with_memo(seq), 6)

    def test_third(self):
        seq = [9,1, 7, 2, 1,4,5,1,3,6,5,9]
        self.assertEqual(average_no_memo(seq), 6)

    def test_fourth(self):
        seq = [9,1, 7, 2, 1,4,5,1,3,6,5,9]
        self.assertEqual(fast(seq), 6)


#TODO beftter testing

if __name__ == '__main__':
    unittest.main()
