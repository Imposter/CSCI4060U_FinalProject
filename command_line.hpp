/**
 * CSCI4060U: Massively Parallel Programming
 * Final Project: Parallel Jaccard Similarity
 * Date: April 15th, 2019
 * Author: Eyaz Rehman
 */

#pragma once

#include <map>
#include <cstdint>
#include <utility>
#include <vector>
#include <string>

class command_line {
    std::map<std::string, std::string> arguments_;

    static std::vector<std::string> convert_to_argv(std::string command_line) {
        std::vector<std::string> arguments;
        std::string string;
        for (size_t i = 0; i < command_line.size(); i++) {
            const auto c = command_line[i];
            if (c == '"') {
                const auto quote_pos = command_line.find('"', i + 1);
                arguments.push_back(command_line.substr(i + 1, quote_pos - i - 1));
                i = quote_pos + 1;
                continue;
            }

            if (c == ' ') {
                arguments.push_back(string);
                string.clear();
                continue;
            }

            string += c;
        }

        return arguments;
    }

public:
    command_line() = default;

    command_line(int argc, char **argv) {
        parse(argc, argv);
    }

    command_line(std::vector<std::string> arguments) {
        parse(arguments);
    }

    command_line(std::string command_line) {
        parse(command_line);
    }

    void parse(int argc, char **argv) {
        for (int i = 0; i < argc; i++) {
            std::string argument = argv[i];
            if (!argument.empty() && argument[0] == '-') {
                auto inserted = false;
                argument = argument.substr(1);
                if (i + 1 < argc) {
                    std::string value = argv[i + 1];
                    if (!value.empty() && value[0] != '-') {
                        arguments_.insert(make_pair(argument, value));
                        inserted = true;
                    }
                }

                if (!inserted)
                    arguments_.insert(make_pair(argument, ""));
            }
        }
    }

    void parse(std::vector<std::string> arguments) {
        for (size_t i = 0; i < arguments.size(); i++) {
            auto argument = arguments[i];
            if (!argument.empty() && argument[0] == '-') {
                auto inserted = false;
                argument = argument.substr(1);
                if (i + 1 < arguments.size()) {
                    const auto value = arguments[i + 1];
                    if (!value.empty() && value[0] != '-') {
                        arguments_.insert(make_pair(argument, value));
                        inserted = true;
                    }
                }

                if (!inserted)
                    arguments_.insert(make_pair(argument, ""));
            }
        }
    }

    void parse(std::string command_line) {
        parse(convert_to_argv(std::move(command_line)));
    }

    bool flag_exists(const std::string &key) {
        return arguments_.find(key) != arguments_.end();
    }

    int64_t get_integer(const std::string &key, int64_t default_value = 0) {
        const auto value = arguments_.find(key);
        if (value == arguments_.end())
            return default_value;

        return stoull(value->second, nullptr, 0);
    }

    std::string get_string(const std::string &key, std::string default_value = "") {
        const auto value = arguments_.find(key);
        if (value == arguments_.end())
            return default_value;

        return value->second;
    }
};