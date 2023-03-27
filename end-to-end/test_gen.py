#!/usr/bin/env python3
import random, sys

num_of_keys = int(sys.argv[1])
num_of_reqs = int(sys.argv[2])
file_name   = sys.argv[3]

def generate_keys():
    keys = set()
    while len(keys) < num_of_keys:
        keys.add(random.randint(0, 4*num_of_keys))
    return keys

def generate_reqs(min, max):
    reqs = []
    for _ in range(0, num_of_reqs):
        first  = random.randint(min, max - 1)
        second = random.randint(first, max)
        reqs.append(first)
        reqs.append(second)
    return reqs

def print_task(keys, reqs):
    file = open(file_name, 'w')
    file.write(str(num_of_keys) + ' ')
    for i in keys:
        file.write(str(i) + ' ')
    file.write(str(num_of_reqs) + ' ')
    for i in reqs:
        file.write(str(i) + ' ')
    file.write('\n')
    file.close()

def main():
    keys = generate_keys()
    reqs = generate_reqs(min(keys), max(keys))
    print_task(keys, reqs)

main()

