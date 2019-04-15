/**
 * CSCI4060U: Massively Parallel Programming
 * Final Project: Parallel Jaccard Similarity
 * Date: April 15th, 2019
 * Author: Eyaz Rehman
 */

#include "jaccard.hpp"
#include "thread.hpp"
#include "command_line.hpp"
#include <math.h>

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
    bool running;
    bool complete;
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

    // Set as complete
    a->complete = true;
}

template<typename _TData, typename _TConvFunc = _TData(*)(const std::string &)>
std::vector<std::pair<std::set<_TData>, float_t>> perform_jaccard(const std::string &file_path, const std::set<_TData> &set, 
    const std::string &seperator, _TConvFunc conv_func, const uint32_t num_threads, const uint32_t buckets) {
    // Jaccard results
    std::vector<std::pair<std::set<_TData>, float_t>> results;

    // Create results mutex
    pthread_mutex_t results_mutex;
    mutex_create(&results_mutex);

    // Get line count
    const auto total_count = get_line_count(file_path);

    // Determine bucket size
    const auto task_buckets = buckets == 0 ? num_threads : buckets;
    const auto task_bucket_size = static_cast<uint32_t>(ceil(total_count / static_cast<double_t>(task_buckets)));

#ifdef DEBUG
    printf("Num threads: %d\r\n", num_threads);
    printf("Buckets: %d\r\n", task_buckets);
#endif

    // Perform Jaccard for all the buckets
    const auto tasks = new perform_jaccard_worker_args<_TData, _TConvFunc>[task_buckets];
    std::map<pthread_t, perform_jaccard_worker_args<_TData, _TConvFunc> *> thread_tasks;
    
    // Setup tasks
    for (auto i = 0; i < task_buckets; i++) {
        // Get arguments
        auto &args = tasks[i];

        // Get offset to read file from
        args.offset = i * task_bucket_size;

        // Get size to read
        args.size = task_bucket_size;
        if ((args.offset + task_bucket_size) > total_count) {
            args.size = total_count - args.offset;
        }

        // Set other args
        args.file_path = file_path;
        args.seperator = seperator;
        args.conv_func = conv_func;
        args.set = set;
        args.results = &results;
        args.results_mutex = &results_mutex;
        args.running = false;
        args.complete = false;

        // Set N initial tasks
        if (i < num_threads) {
            thread_tasks.insert({ static_cast<pthread_t>(i), &args });
        }
    }

    // Keep running unless done
    auto done = false;
    while (!done) {
        // Ensure N threads are running
        for (auto &p : thread_tasks) {
            if (p.second->running && p.second->complete) {
                // Join thread, mark as not running
                thread_join(const_cast<pthread_t *>(&p.first));
                p.second->running = false;                
            } 
            
            if (!p.second->running) {
                // Get next available task
                auto t = -1;
                for (auto i = 0; i < task_buckets; i++) {
                    if (!tasks[i].running && !tasks[i].complete) {
                        t = i;
                        break;
                    }
                }

                // There are no more tasks available, we're done
                if (t == -1) { 
                    done = true;
                    break;
                }

                // Create new thread for next available task
                p.second = &tasks[t];
                p.second->running = true;
                thread_create(const_cast<pthread_t *>(&p.first), perform_jaccard_worker, p.second, true);
            }
        }
    }

    // Join any remaining threads
    for (const auto &p : thread_tasks) {
        thread_join(const_cast<pthread_t *>(&p.first));
    }

    // Destroy tasks
    delete[] tasks;

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

    const auto num_threads = cmd.get_integer("threads", _NUM_THREADS);
    const auto buckets = cmd.get_integer("buckets", num_threads);
    const auto set = convert_to_set<uint32_t>(cmd.get_string("set"), str_to_ulong);

    const auto results = perform_jaccard(cmd.get_string("dataset"), set, " ", str_to_ulong, num_threads, buckets);

    return 0;
}