/**
 * CSCI4060U: Massively Parallel Programming
 * Final Project: Parallel Jaccard Similarity
 * Date: April 15th, 2019
 * Author: Eyaz Rehman
 */

#pragma once

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <algorithm>

uint32_t get_line_count(const std::string &path) {
    uint32_t count = 0;
    std::ifstream file(path);
    if (file.is_open()) {
        std::string line;
        while (!file.eof()) {
            std::getline(file, line);
            count++;
        }
        file.close();
    }
    return count;
}

std::vector<std::string> read_lines(const std::string &path, uint32_t offset = 0, uint32_t count = 0) {
    std::vector<std::string> lines;
    std::ifstream file(path);
    if (file.is_open()) {
        std::string line;
        uint32_t i = 0;
        while (!file.eof()) {
            if (count > 0 && i++ == offset + count) break;
            std::getline(file, line);
            if (i < offset || line.empty()) continue;
            lines.push_back(line);
        }
        file.close();
    }
    return lines;
}

std::vector<std::string> split_string(std::string source, const std::string &split) {
	std::string input = std::move(source);
	std::vector<std::string> output;

	size_t i;
	while ((i = input.find(split)) != std::string::npos) {
		output.push_back(input.substr(0, i));
		input.erase(0, i + split.size());
	}

	output.push_back(input);
	return output;
}

std::string replace_string(std::string source, std::string from, const std::string &to, bool ignore_case = false) {
    std::string output = std::move(source);

    for (size_t x = 0; x < output.size(); x++) {
        bool same = true;
        for (size_t y = 0; y < from.size(); y++) {
            char s = output[x + y];
            char f = from[y];

            if (ignore_case) {
                s = tolower(s);
                f = tolower(f);
            }

            if (s != f) {
                same = false;
                break;
            }
        }

        if (same) output.replace(x, from.size(), to);
    }

    return output;
}

uint32_t str_to_ulong(const std::string &s) {
    return strtoul(s.c_str(), 0, 10);
}

template<typename _TData, typename _TConvFunc = _TData(*)(const std::string &)>
std::set<_TData> convert_to_set(const std::string &s, _TConvFunc conv_func) {
    std::set<_TData> result;
    const auto split_str = split_string(s, ",");
    for (const auto &split : split_str) {
        if (split.empty()) continue;
        if (split[0] == ' ') result.insert(conv_func(split.substr(1)));
        else result.insert(conv_func(split));
    }
    return result;
}

template<typename _TData>
std::set<_TData> set_union(const std::set<_TData> &a, const std::set<_TData> &b) {
    std::set<_TData> result;
    std::insert_iterator<std::set<_TData>> it(result, result.begin());
    std::set_union(a.begin(), a.end(), b.begin(), b.end(), it);
    return result;
}

template<typename _TData>
std::set<_TData> set_intersection(const std::set<_TData> &a, const std::set<_TData> &b) {
    std::set<_TData> result;
    std::insert_iterator<std::set<_TData>> it(result, result.begin());
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), it);
    return result;
}