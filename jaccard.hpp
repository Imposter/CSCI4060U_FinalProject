/**
 * CSCI4060U: Massively Parallel Programming
 * Final Project: Parallel Jaccard Similarity
 * Date: April 15th, 2019
 * Author: Eyaz Rehman
 */

#pragma once

#include "utility.hpp"
#include <utility>

template<typename _TData>
std::vector<std::pair<std::set<_TData>, float_t>> jaccard(const std::vector<std::set<_TData>> &sets, const std::set<_TData> &set) {
    std::vector<std::pair<std::set<_TData>, float_t>> results;
    for (const auto &s : sets) {
        // Get the intersection and union of the sets
        const auto i = set_intersection(s, set);
        const auto u = set_union(s, set);

        // Compute Jaccard index
        const auto j = static_cast<float_t>(i.size()) / static_cast<float_t>(u.size());

        results.push_back({ s, j });
    }
    return results;
}