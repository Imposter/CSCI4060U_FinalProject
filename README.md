# CSCI4060U Project - Parallel Jaccard Similarity
## Introduction
Jaccard Similarity (Jaccard Index) is a measurement of the similarity between two sample sets. It's defined as the size of the intersection of the sets divided by the size of the union of the sets. This algorithm is utilized in various applications including social networks, where it is used to provide users with friend recommendations based on the similarity of their interests or mutual friends. It’s also used in online retail websites to show products that were purchased by customers, in terms of what was previously purchased, or to give customers recommendations on items that are frequently purchased together.

## Compiling
- Clone repository
- Ensure Python 3 is installed
- Ensure G++ is installed
- Ensure Python dependencies are installed (`pip install -r requirements.txt`).
- Compile C++ programs using `build.sh`, or `build_debug.sh` for debug output

## Running
- To run the threads vs. time benchmark, use `python benchmark_thread.py`
- To run the buckets vs. time benchmark, use `python benchmark_bucket.py`
- To run the applications directly, use:
    - `./jaccard_omp -dataset "./retail.dat" -set "12,242,53" -threads 4 -buckets 100`
        - dataset: the dataset to compute similarity on
        - set: the target set to compute similarity against
        - threads: number of threads to use (available on programs `jaccard_omp`, `jaccard_pthread` and `jaccard_pthread_buckets`)
        - buckets: number of partitions to create on the dataset (more consumes less memory) (available on programs `jaccard_omp` and `jaccard_pthread_buckets`)

## Author
- Eyaz Rehman ([GitHub](http://github.com/Imposter))