/*
 * Copyright 2025 Love Billenius

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 *  1. Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.

 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.

 *  3. Neither the name of the copyright holder nor the names of its contributors
 *   may be used to endorse or promote products derived from this software without
 *   specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "rainbow.hpp"
#include "ansiparser.hpp"
#include "terminal.hpp"

#include <cmath>
#include <cstdint>
#include <format>
#include <iostream>
#include <numbers>
#include <random>
#include <string>
#include <sys/types.h>
#include <thread>
#include "format.hpp"

constexpr int rgb_to_256(const uint8_t r, const uint8_t g, const uint8_t b) {
    return 16 + (36 * (r / 51) + 6 * (g / 51) + (b / 51));
}

Rainbow::Rainbow(const cli::Options &options) :
    m_spread(options.spread), m_speed(options.speed), m_freq(options.freq), m_duration(options.duration),
    m_color_offset(options.seed ? options.seed : static_cast<int>(std::random_device{}() % 256)),
    m_invert(options.invert), m_animate(options.animate), m_truecolor_mode(options.truecolor || term::is_truecolor()),
    m_force_term(options.force) {}

void Rainbow::process(std::istream &in) {
    if (m_animate && is_a_real_terminal())
        std::cout << term::hide_cursor;

    std::string line;
    while (std::getline(in, line)) {
        print_line(line);
        m_line_count++;
    }

    if (is_a_real_terminal())
        std::cout << term::show_cursor << term::reset;
}


class Color {
public:
    Color(const float freq, const float pos) {
        constexpr float pi = std::numbers::pi_v<float>;
        r = static_cast<uint8_t>(std::sin(freq * pos) * 127 + 128);
        g = static_cast<uint8_t>(std::sin(freq * pos + 2 * pi / 3) * 127 + 128);
        b = static_cast<uint8_t>(std::sin(freq * pos + 4 * pi / 3) * 127 + 128);
    }

    [[nodiscard]] std::string format(const bool invert, const bool truecolor) const {
        const auto bg_fg = invert ? 48 : 38;
        if (truecolor)
            return ff::format("\x1b[{};2;{};{};{}m", bg_fg, r, g, b);
        return ff::format("\x1b[{};5;{}m", bg_fg, rgb_to_256(r, g, b));
    }

private:
    uint8_t r, g, b;
};


void Rainbow::print_line(std::string_view line) const {
    int char_index = 0;
    auto get_position = [&]() {
        float total_position = static_cast<float>(m_color_offset + m_line_count + char_index);
        return total_position / m_spread;
    };

    AnsiParser parser(line);
    for (const auto &token: parser) {
        if (token.is_escape) {
            std::cout << token.content;
            continue;
        }
        Color color(m_freq, get_position());

        std::cout << color.format(m_invert, m_truecolor_mode) << token.content
                  << (m_invert ? term::RESET_BACKGROUND : term::RESET_FOREGROUND);

        if (m_animate) {
            std::cout << std::flush;
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(1000000 / m_speed)));
        }
        char_index++;
    }

    std::cout << '\n';
}

bool Rainbow::is_a_real_terminal() const { return m_force_term || term::is_tty(stdout); }
