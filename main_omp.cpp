/**
 * CSCI4060U: Massively Parallel Programming
 * Final Project: Parallel Jaccard Similarity
 * Date: April 15th, 2019
 * Author: Eyaz Rehman
 */

#include "jaccard.hpp"
#include "command_line.hpp"
#include <omp.h>

#ifndef _NUM_THREADS
#define _NUM_THREADS 4
#endif

#ifdef __GNUC__
// From: https://stackoverflow.com/questions/11071116/i-got-omp-get-num-threads-always-return-1-in-gcc-works-in-icc
int omp_get_thread_count() {
    int n = 0;
    #pragma omp parallel reduction(+:n)
    n += 1;
    return n;
}
#else
#define omp_get_thread_count() omp_get_num_threads()
#endif

template<typename _TData, typename _TConvFunc = _TData(*)(const std::string &)>
std::vector<std::set<_TData>> process_lines(const std::vector<std::string> &lines, const std::string &separator, _TConvFunc conv_func) {
    std::vector<std::set<_TData>> results;
    for (const auto &line : lines) {
        const auto line_split = split_string(line, separator);
        std::set<_TData> set;
        for (const auto &s : line_split) {
            if (!s.empty()) {
                set.insert(conv_func(s));
            }
        }
        results.push_back(set);
    }
    return results;
}

template<typename _TData, typename _TConvFunc = _TData(*)(const std::string &)>
std::vector<std::pair<std::set<_TData>, float_t>> perform_jaccard(const std::string &file_path, const std::set<_TData> &set, 
    const std::string &seperator, _TConvFunc conv_func, const uint32_t buckets = 0) {
    // Jaccard results
    std::vector<std::pair<std::set<_TData>, float_t>> results;

    // Get line count
    const auto total_count = get_line_count(file_path);

    // Determine bucket size
    const auto task_buckets = buckets == 0 ? omp_get_thread_count() : buckets;
    const auto task_bucket_size = total_count / task_buckets;

#ifdef DEBUG
    printf("Num threads: %d\r\n", omp_get_thread_count());
    printf("Buckets: %d\r\n", task_buckets);
#endif

    // Perform Jaccard for all the buckets
    #pragma omp parallel for
    for (auto i = 0; i < task_buckets; i++) {
        // Get offset to read file from
        const auto offset = i * task_bucket_size;

        // Get size to read
        auto size = task_bucket_size;
        if ((offset + task_bucket_size) > total_count) {
            size = total_count - offset;
        }

#ifdef DEBUG
        // Print info
        printf("Reading %s from %d to %d\r\n", file_path.c_str(), offset, offset + size);
#endif

        // Read file
        const auto lines = read_lines(file_path, offset, size);

        // Perform Jaccard and append results
        const auto t = process_lines<_TData, _TConvFunc>(lines, seperator, conv_func);
        const auto res = jaccard(t, set);

        #pragma omp critical
        results.insert(results.end(), res.begin(), res.end());

#ifdef DEBUG
        printf("Bucket %d completed (thread id: %d)\r\n", i, omp_get_thread_num());
#endif
    }

    return results;
}

int main(int argc, char **argv) {
    // Parse command line
    command_line cmd(argc, argv);

    // Ensure required arguments are present
    if (!cmd.flag_exists("dataset")) { printf("Dataset not specified\r\n"); return 1; }
    if (!cmd.flag_exists("set")) { printf("Target set not specified\r\n"); return 1; }

    const auto num_threads = cmd.get_integer("threads", _NUM_THREADS);
    const auto buckets = cmd.get_integer("buckets", num_threads);
    const auto set = convert_to_set<uint32_t>(cmd.get_string("set"), str_to_ulong);

    // Set number of threads
    omp_set_num_threads(num_threads);

    const auto results = perform_jaccard(cmd.get_string("dataset"), set, " ", str_to_ulong, buckets);

    return 0;
}