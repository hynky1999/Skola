def prevedCislo(cislo):
    cislo_bin = ""
    if(cislo == 0):
        return "0"

    while(cislo):
        tmp = cislo//2
        remainder = cislo-tmp*2
        cislo_bin = str(remainder) + cislo_bin
        cislo = tmp
    return cislo_bin

cislo = int(input())
print(prevedCislo(cislo))
