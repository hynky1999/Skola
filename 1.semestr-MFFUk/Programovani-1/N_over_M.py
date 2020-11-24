import math
import random

def divide(num, divisor):
    shift = 1
    while(num >= shift):
        shift = shift*10
    shift = shift//10

    result_whole = 0
    result_tenth = 1
    result_tmp = 0
    while(shift):
        rem = num//shift
        result_tmp = 0
        while(rem >= divisor):
            rem -= divisor
            result_tmp += 1

        result_whole = result_whole*10
        result_whole += result_tmp
        num -= (result_tmp * divisor) * shift
        shift = shift//10

    if(not num):
        return str(result_whole)
    index = 0
    know_rems = {num: index}

    if(num == 0):
        return str(result_whole)

    while(num != 0):
        index += 1
        num = num*10
        result_tmp = 0
        while(num >= divisor):
            num -= divisor
            result_tmp += 1

        result_tenth = result_tenth*10
        result_tenth += result_tmp
        if(num in know_rems):
            index = know_rems[num]
            break
        know_rems[num] = index

    result_tenth = str(result_tenth)
    if(num != 0):
        return str(result_whole) + '.' + result_tenth[1:index+1] + '(' + result_tenth[index+1:] + ')'

    return str(result_whole) + '.' + result_tenth[1:]

a,b = list(map(int, input().split()))
print(divide(a,b))
