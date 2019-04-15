/**
 * CSCI4060U: Massively Parallel Programming
 * Final Project: Parallel Jaccard Similarity
 * Date: April 15th, 2019
 * Author: Eyaz Rehman
 */

#include "jaccard.hpp"
#include "command_line.hpp"
#include <iostream>

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

int main(int argc, char **argv) {
    // Parse command line
    command_line cmd(argc, argv);

    // Ensure required arguments are present
    if (!cmd.flag_exists("dataset")) { printf("Dataset not specified\r\n"); return 1; }
    if (!cmd.flag_exists("set")) { printf("Target set not specified\r\n"); return 1; }

    const auto lines = read_lines(cmd.get_string("dataset"));
    const auto t = process_lines<uint32_t>(lines, " ", str_to_ulong);
    const auto set = convert_to_set<uint32_t>(cmd.get_string("set"), str_to_ulong);

    // Get jaccard similarity between t and set
    const auto results = jaccard<uint32_t>(t, set);

    return 0;
}