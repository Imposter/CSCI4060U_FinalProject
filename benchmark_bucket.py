"""
CSCI4060U: Massively Parallel Programming
Final Project: Parallel Jaccard Similarity
Date: April 15th, 2019
Author: Eyaz Rehman
"""

import subprocess as sp
from time import time
from os.path import abspath
from matplotlib import pyplot as plt

def run_jaccard(dataset_path, target_set, atype="openmp", num_threads=0, num_buckets=0):
    program_name = None
    if atype is "openmp":
        program_name = "jaccard_omp"
    elif atype is "pthread":
        program_name = "jaccard_pthread_buckets"
    if program_name is None: 
        return -1
    start = time()
    ret_code = sp.call([ 
        abspath(program_name), 
        "-dataset", abspath(dataset_path), 
        "-set", ",".join(map(str, target_set)), 
        "-threads", str(num_threads),
        "-buckets", str(num_buckets)
    ])
    elapsed = time() - start

    return ret_code, elapsed

def run_jaccard_avg(n, dataset_path, target_set, atype="openmp", num_threads=0, num_buckets=0):
    results = []
    for i in range(n):
        res = run_jaccard(dataset_path, target_set, atype, num_threads, num_buckets)
        if res is -1 or res[0] is not 0:
            return []
        results.append(res[1])
    return sum(results) / len(results)

if __name__ == "__main__":
    DATASET = "./retail.dat"
    SET = [ 142, 32, 75 ]
    THREADS = 4
    BUCKETS = list(range(1, 100, 1)) # Range of buckets to test
    TESTS = 3

    # Get averages for both OpenMP and Pthreads
    omp_avgs = [ run_jaccard_avg(TESTS, DATASET, SET, "openmp", THREADS, b) for b in BUCKETS ]
    pthread_avgs = [ run_jaccard_avg(TESTS, DATASET, SET, "pthread", THREADS, b) for b in BUCKETS ]

    # Print results
    print(BUCKETS)
    print(omp_avgs)
    print(pthread_avgs)

    # Create plot
    plt.title("Jaccard Performance (OpenMP vs. Pthreads)")
    plt.xlabel("Buckets")
    plt.ylabel("Time (s)")

    # Add to plot
    plt.plot(BUCKETS, omp_avgs, "r--", label="OpenMP")
    plt.plot(BUCKETS, pthread_avgs, "b--", label="Pthread")

    # Show the plot
    plt.legend()
    plt.show()