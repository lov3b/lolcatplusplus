#pragma once

#include <stdint.h>
#include <string>

#include "args.hpp"

class Rainbow {
public:
    explicit Rainbow(const cli::Options &options);

    void process(std::istream &in);

private:
    [[nodiscard]] std::string rainbow(float freq, float pos) const;

    [[nodiscard]] std::string format_color(uint8_t r, uint8_t g, uint8_t b) const;

    void print_line(const std::string &line, bool animate = false) const;

    void animate_line(const std::string &line);

    [[nodiscard]] bool is_tty() const;

private:
    float m_spread;
    float m_speed;
    float m_freq;
    int m_duration;
    int m_color_offset = 0;
    int m_line_count = 0;
    bool m_invert;
    bool m_animate;
    bool m_truecolor_mode;
    bool m_force_term;
};
