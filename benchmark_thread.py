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

def run_jaccard(dataset_path, target_set, atype="sequential", num_threads=0):
    program_name = None
    if atype is "sequential":
        program_name = "jaccard_sequential"
    elif atype is "openmp":
        program_name = "jaccard_omp"
    elif atype is "pthread":
        program_name = "jaccard_pthread"
    if program_name is None: 
        return -1
    start = time()
    ret_code = sp.call([ 
        abspath(program_name), 
        "-dataset", abspath(dataset_path),
        "-set", ",".join(map(str, target_set)), 
        "-threads", str(num_threads) 
    ])
    elapsed = time() - start

    return ret_code, elapsed

def run_jaccard_avg(n, dataset_path, target_set, atype="sequential", num_threads=0):
    results = []
    for i in range(n):
        res = run_jaccard(dataset_path, target_set, atype, num_threads)
        if res is -1 or res[0] is not 0:
            print("ERR", res)
            return -1
        results.append(res[1])
    return sum(results) / len(results)

if __name__ == "__main__":
    DATASET = "./retail.dat"
    SET = [ 142, 32, 75 ]
    THREADS_MAX = 12
    THREADS = list(range(1, THREADS_MAX + 1)) # Range of threads to test
    TESTS = 3

    # Get averages for both OpenMP and Pthreads
    omp_avgs = [ run_jaccard_avg(TESTS, DATASET, SET, "openmp", t) for t in THREADS ]
    pthread_avgs = [ run_jaccard_avg(TESTS, DATASET, SET, "pthread", t) for t in THREADS ]

    # Print results
    print(THREADS)
    print(omp_avgs)
    print(pthread_avgs)

    # Create plot
    plt.title("Jaccard Performance (OpenMP vs. Pthreads)")
    plt.xlabel("Threads")
    plt.ylabel("Time (s)")

    # Add to plot
    plt.plot(THREADS, omp_avgs, "ro--", label="OpenMP")
    plt.plot(THREADS, pthread_avgs, "bo--", label="Pthread")

    # Show the plot
    plt.legend()
    plt.show()