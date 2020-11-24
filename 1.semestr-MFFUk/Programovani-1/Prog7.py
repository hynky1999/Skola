month_list = (31,28,31,30,31,30,31,31,30,31,30,31)

def leap_from_zero(y):
    # Inclusive without y
    y -= 1
    if y < 0:
        return 0
    return y//4 - y//100 + y//400

def days_between_years(start_y, end_y):
    # days in [start_y, end_y)
    if(start_y >= end_y):
        return 0
    leap_years = leap_from_zero(end_y) - leap_from_zero(start_y)
    return leap_years*366 + (end_y - start_y - leap_years)*365

def days_to_month_day(d, m, y):
    is_leap = False
    if y % 400 == 0 or (y % 4 == 0 and y % 100 != 0):
        is_leap = True

    if(d > month_list[m-1] and not(is_leap and m == 2 and d == 29)):
        raise ValueError("Wrong day")

    days = 0
    days = sum(month_list[:(m-1)])
    if is_leap and m > 2:
        days += 1
    days += d
    return days

def days_between(start_d, start_m, start_y, end_d, end_m, end_y):
    return days_between_years(start_y, end_y)\
            - days_to_month_day(start_d, start_m, start_y)\
            + days_to_month_day(end_d, end_m, end_y)


start_d,start_m,start_y,end_d,end_m,end_y = list(map(int, input().split()))
print(days_between(start_d, start_m, start_y, end_d, end_m, end_y))
