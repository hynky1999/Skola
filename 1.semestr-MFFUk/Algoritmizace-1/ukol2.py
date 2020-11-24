from math import sqrt

"""
Snad nevadí, že jsem napsal kód v Pythonu, Python mi přijde, že se hodně podobá pseudokódu a je to v něm poměrně názorné.

Problém budeme řešit pomocí Eratosthenova síta.
Na vstupu dostaneme rozsah N. Nejprve si vytvoříme síto tak, že místo začernování násobků daného prvočísla(nastavením False),
budeme na neprovočíselná políčka zapisovat první číslo, které je začernilo.
Tím dostaneme na konci seznam, ve kterém budou místo neprovočíselných čísel jejich nejmenší dělitel.
Poté už pouze stačí vyhledat dané číslo v seznamu a vypsat jeho hodnotu(nejmenší dělitel prvočísla je provočíslo samo, to se nám nikdy nepřepíše(není kdo by ho přepsal, nemá dělitele).
Naopak z kontrukce síta vidíme, že na indexech neprovočísel budou jejich nejmenší dělitelé(přepisujeme nejdříve nejmenším číslem = nejmenší dělitel).

Takovýto algoritmus však nemá dostatečně dobrou pamětovou složitost. Tento problém však vyřešíme tím, že nebudeme dělat síto pro všechna čísla od 1 do n, ale bude stačit vzít pouze lichá čísla od 1 do n.
Síto jak víme z přednášky má složitost n*log(log(n)). Jelikož máme pouze liché síto, sníží se nám složitost o n/2(První kolo nemusíme procházet sudé prvky těch je max n/2. Random access by měl být v konstantním čase.

Dohromady tedy časová složitost N*log(log(n))(síto)*k - N/2(nedělení 2) + K(počet testovaných prvků) \in O(N*log(log(n)))

Pamětová složitost je N/2 + 1 + C. Nejdelší pole, která inicializujeme Je N/2 + 1.
"""


def createOddSieve(n):
    length = n//2 + 1
    sieve = [1] + [1]*(length-1)
    # sive[0] = 1 sieve[1] = 3....
    for i in range(1, int(sqrt(length))):
        if sieve[i] == 1:
            odd_num = 2*i + 1
            start = (odd_num**2 -1)//2
            for j in range(start, length, odd_num):
                if(sieve[j] == 1):
                    sieve[j] = odd_num
    return sieve


def testNum(num, sieve):
    if(num%2 == 0):
        return 2

    odd_num_i = (num-1)//2
    if(sieve[odd_num_i] == 1):
        return num

    return sieve[odd_num_i]




tested_nums = (17, 7, 3, 4231443, 5, 103, 27, 35, 49)
sieve = createOddSieve(max(tested_nums))
for num in tested_nums:
    print(testNum(num,sieve))
