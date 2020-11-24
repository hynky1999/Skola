num, length = map(int, input().split(" "))
map_strings = {}
strings = []
for i in range(num):
    a = input()
    strings.append(a)
    map_strings[a] = i

mid = ""
mid_set = 0
final_str=""
count = 0
for i in range(num):
    result = map_strings.get(strings[i][::-1], -1)
    if result == -1:
        continue
    if result == i:
        if not mid_set:
            mid_set = 1
            mid =strings[i]
            count +=1
    else:
        final_str += strings[i]
        map_strings[strings[i]] = -1
        count+=2

print("{0}".format(count*length))
print("{0}{1}{2}".format(final_str, mid, final_str[::-1]))
