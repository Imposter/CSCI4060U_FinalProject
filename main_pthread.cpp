/**
 * CSCI4060U: Massively Parallel Programming
 * Final Project: Parallel Jaccard Similarity
 * Date: April 15th, 2019
 * Author: Eyaz Rehman
 */

#include "jaccard.hpp"
#include "thread.hpp"
#include "command_line.hpp"

#ifndef _NUM_THREADS
#define _NUM_THREADS 4
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
struct perform_jaccard_worker_args {
    int offset;
    int size;
    std::string file_path;
    std::string seperator;
    _TConvFunc conv_func;
    std::set<_TData> set;
    std::vector<std::pair<std::set<_TData>, float_t>> *results;
    pthread_mutex_t *results_mutex;
};

typedef void *_worker_data_t;
typedef void *(*_worker_conv_func_t)(const std::string &);
typedef perform_jaccard_worker_args<_worker_data_t, _worker_conv_func_t> _worker_args_t;

void perform_jaccard_worker(void *args) {
    // Convert args
    const auto a = reinterpret_cast<_worker_args_t *>(args);

#ifdef DEBUG
        // Print info
        printf("Reading %s from %d to %d\r\n", a->file_path.c_str(), a->offset, a->offset + a->size);
#endif

    // Read file
    const auto lines = read_lines(a->file_path, a->offset, a->size);

    // Perform Jaccard and append results
    const auto t = process_lines<_worker_data_t, _worker_conv_func_t>(lines, a->seperator, a->conv_func);
    const auto res = jaccard(t, a->set);

    mutex_lock(a->results_mutex);
    a->results->insert(a->results->end(), res.begin(), res.end());
    mutex_unlock(a->results_mutex);
}

template<typename _TData, typename _TConvFunc = _TData(*)(const std::string &)>
std::vector<std::pair<std::set<_TData>, float_t>> perform_jaccard(const std::string &file_path, const std::set<_TData> &set, 
    const std::string &seperator, _TConvFunc conv_func, const uint32_t num_threads) {
    // Jaccard results
    std::vector<std::pair<std::set<_TData>, float_t>> results;

    // Create results mutex
    pthread_mutex_t results_mutex;
    mutex_create(&results_mutex);

    // Get line count
    const auto total_count = get_line_count(file_path);

#ifdef DEBUG
    printf("Num threads: %d\r\n", num_threads);
#endif

    // Perform Jaccard
    const auto line_count = total_count / num_threads;
    const auto threads = new pthread_t[num_threads];
    const auto thread_args = new perform_jaccard_worker_args<_TData, _TConvFunc>[num_threads];
    for (auto i = 0; i < num_threads; i++) {
        // Get arguments
        auto &args = thread_args[i];

        // Get offset to read file from
        args.offset = i * line_count;

        // Get size to read
        args.size = line_count;
        if ((args.offset + line_count) > total_count) {
            args.size = total_count - args.offset;
        }

        // Set other args
        args.file_path = file_path;
        args.seperator = seperator;
        args.conv_func = conv_func;
        args.set = set;
        args.results = &results;
        args.results_mutex = &results_mutex;

        // Start thread
        thread_create(&threads[i], perform_jaccard_worker, &args, true);
    }

    // Join threads
    for (auto i = 0; i < num_threads; i++) {
        thread_join(&threads[i]);
#ifdef DEBUG
        printf("Thread %d completed\r\n", i);
#endif
    }

    // Destroy thread ids and args
    delete[] threads;
    delete[] thread_args;

    // Destroy mutex
    mutex_destroy(&results_mutex);

    return results;
}

int main(int argc, char **argv) {
    // Parse command line
    command_line cmd(argc, argv);

    // Ensure required arguments are present
    if (!cmd.flag_exists("dataset")) { printf("Dataset not specified\r\n"); return 1; }
    if (!cmd.flag_exists("set")) { printf("Target set not specified\r\n"); return 1; }

    // Get number of threads
    const auto num_threads = cmd.get_integer("threads", _NUM_THREADS);

    const auto set = convert_to_set<uint32_t>(cmd.get_string("set"), str_to_ulong);
    const auto results = perform_jaccard(cmd.get_string("dataset"), set, " ", str_to_ulong, num_threads);

    return 0;
}