digits = '0123456789ABCDEF'

def convertTo(num, base):
    converted_num = ''
    global digits
    while(num != 0):
        num_tmp = num//base
        remainder = num-base*num_tmp
        converted_num = digits[remainder] + converted_num
        num = num_tmp

    if len(converted_num) == 0:
        return '0'

    return converted_num

print(convertTo(3, 2))
