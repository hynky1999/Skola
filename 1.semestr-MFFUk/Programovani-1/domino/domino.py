import sys
def domino_helper(doms):

    def domino(end):
        maximum = 0
        nonlocal doms
        poss_doms = doms[end]
        for i in range(len(poss_doms)):
            if(poss_doms[i]['count'][0] > 0):
                # Count must be pointer to propagate
                poss_doms[i]['count'][0] -= 1
                maximum = max(domino(poss_doms[i]['end'])+1, maximum)
                poss_doms[i]['count'][0] += 1

        return maximum
    
    maximum = 0
    for start, ends in doms.items():
        if(len(ends) > 0):
            maximum = max(domino(start), maximum)

    return maximum

 
def search_index(l, num):
    for i in range(len(l)):
        if l[i]['end'] == num:
            return i
    return -1


def append_domino(doms, start, end, count_ptr, create_only=False):
    l = doms[start]
    index = search_index(l, end)
    if(index == -1):
        if(count_ptr):
            doms[start].append({'end': end, 'count': count_ptr})
        else:
            doms[start].append({'end': end, 'count': [1]})
    else:
        #If we received count_ptr we already added
        if(not create_only):
            doms[start][index]['count'][0] += 1

    return doms[start][index]['count']

def create_dominos():
    doms = { i:[] for i in range(1, 39)}
    num, *doms_int = map(int,input().split())
    while(len(doms_int) < num*2):
        doms_int += map(int,input().split())
    for i in range(num):
        start = doms_int[i*2]
        end = doms_int[i*2 + 1]
        count_ptr = append_domino(doms, start, end, [])
        if(start != end):
            append_domino(doms, end, start, count_ptr, True)


    return doms

print(domino_helper(create_dominos()))
