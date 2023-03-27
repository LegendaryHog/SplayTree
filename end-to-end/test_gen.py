#!/usr/bin/env python3
import random, sys, math

num_of_keys = int(sys.argv[1])
num_of_reqs = int(sys.argv[2])
file_name   = sys.argv[3]
mode = ""
if len(sys.argv) == 5:
    mode = sys.argv[4]

def generate_keys():
    keys_set = set()
    keys_list = []
    while len(keys_set) < num_of_keys:
        sz = len(keys_set)
        new_elem = int(random.randint(0, 4*num_of_keys))
        keys_set.add(new_elem)
        if len(keys_set) > sz:
            keys_list.append(new_elem)
    return keys_list

def generate_uniform_reqs(min, max):
    reqs = []
    for _ in range(0, num_of_reqs):
        first  = random.randint(min, max - 1)
        second = random.randint(first, max)
        reqs.append(first)
        reqs.append(second)
    return reqs


def generate_triangular_reqs(min, max):
    reqs = []
    for _ in range(0, num_of_reqs):
        first  = int(random.triangular(min, max - 1, (min + max -1)/2))
        second = int(random.triangular(first, max, (first + max)/2))
        reqs.append(first)
        reqs.append(second)
    return reqs

def generate_normal_reqs(min, max):
    reqs = []
    sigma = (max - min)/2
    for _ in range(0, num_of_reqs):
        loc_set = set()
        while len(loc_set) != 2:
            loc_set.add(int(random.normalvariate(max/2, sigma)))
        second = loc_set.pop()
        first  = loc_set.pop() 
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
    if mode == "--triangular":
        reqs = generate_triangular_reqs(min(keys), max(keys))
    elif mode == "--normal":
        reqs = generate_normal_reqs(min(keys), max(keys))
    else:
        reqs = generate_uniform_reqs(min(keys), max(keys))
    print_task(keys, reqs)

main()

