g++ ./main.cpp -std=c++14 -g -DDEBUG -o jaccard_sequential
g++ ./main_omp.cpp -std=c++14 -g -DDEBUG -fopenmp -o jaccard_omp
g++ ./main_pthread.cpp -std=c++14 -g -DDEBUG -lpthread -o jaccard_pthread
g++ ./main_pthread_buckets.cpp -std=c++14 -g -DDEBUG -lpthread -o jaccard_pthread_buckets