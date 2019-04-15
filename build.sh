g++ ./main.cpp -std=c++14 -o jaccard_sequential
g++ ./main_omp.cpp -std=c++14 -fopenmp -o jaccard_omp
g++ ./main_pthread.cpp -std=c++14 -lpthread -o jaccard_pthread
g++ ./main_pthread_buckets.cpp -std=c++14 -lpthread -o jaccard_pthread_buckets