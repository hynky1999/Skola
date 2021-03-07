import math
from collections import OrderedDict
from sys import stdin
sieve = []

def create_sieve(length):
    global sieve
    length = length+1
    sieve = [i for i in range(length)]
    prime_order = 1
    for i in range(2, length):
        if sieve[i] != 0:
            for x in range(i*i, length, i):
                if(sieve[x] % i == 0):
                    sieve[x] = 0
            sieve[i] = prime_order
            prime_order += 1

def factorise(num):
    primes = OrderedDict()
    for i in range(2, num):
        if num%i == 0:
            num =num// i
            primes[i] = 1

        while num%i == 0:
            num =num// i
            primes[i] += 1

        if num == 1:
            return primes


    if num != 1:
        primes[num] = 1
    return primes


def encode_prime(prime, offset):
    order = sieve[prime]-1
    return offset+order+1, "."*(order-offset)


def encode(num):
    primes = factorise(num)
    offset = 0
    num_str = "("
    for prime, exponent in primes.items():
        if(prime == 1):
            continue
        offset, prime_str = encode_prime(prime, offset)
        exp_str = encode(exponent)
        num_str += f"{prime_str}{exp_str}"
    num_str += ")"
    return num_str

def highest_prime(num):
    primes = factorise(num)
    h_prime = 0
    for prime, exponent in primes.items():
        if(prime == 1):
            continue
        h_prime = max(max(highest_prime(exponent), h_prime), prime)

    return h_prime

nums_to_encode = []
max_prime = 0
for line in stdin:
    l = int(line)
    nums_to_encode.append(l)
    max_prime = max(max_prime, highest_prime(l))
create_sieve(max_prime)
print(len(sieve))
for num in nums_to_encode:
    if(num == 0):
        print('.')
    elif(num == 1):
        print('()')
    else:
        print(encode(num))
