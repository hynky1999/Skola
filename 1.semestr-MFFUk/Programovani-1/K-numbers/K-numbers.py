def k_perms(actual_num, rem_digit_sum, rem):
    if(rem*9 < rem_digit_sum):
        return

    if(rem == 0):
        print(actual_num)
        return

    actual_num = actual_num * 10
    if(actual_num != 0):
        k_perms(actual_num, rem_digit_sum, rem-1)

    for i in range(1, min(10, rem_digit_sum+1)):
        k_perms(actual_num + i, rem_digit_sum - i, rem-1)

k, n = map(int, input().split())
k_perms(0, n, k)
        
        
        






