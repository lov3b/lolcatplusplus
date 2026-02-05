#pragma once

#include <optional>
#include <string>
#include <vector>


namespace cli {

struct Options {
    float spread;
    float freq;
    float speed;
    int seed;
    int duration;
    bool animate;
    bool invert;
    bool truecolor;
    bool force;
};

struct Args {
    Options args;
    std::vector<std::string> files;
};

std::optional<Args> parse_args(int argc, char *argv[]) noexcept;

}; // namespace cli
