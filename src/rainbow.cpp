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
#include "terminal.hpp"

#include <cmath>
#include <cstring>
#include <format>
#include <iostream>
#include <numbers>
#include <random>
#include <regex>
#include <string>
#include <thread>
#include "format.hpp"

constexpr int rgb_to_256(const uint8_t r, const uint8_t g, const uint8_t b) {
    return 16 + (36 * (r / 51) + 6 * (g / 51) + (b / 51));
}

static bool is_truecolor() {
    const char *colorterm = std::getenv("COLORTERM");
    return colorterm && (std::strstr(colorterm, "truecolor") || std::strstr(colorterm, "24bit"));
}

Rainbow::Rainbow(const cli::Options &options) :
    m_spread(options.spread), m_speed(options.speed), m_freq(options.freq), m_duration(options.duration),
    m_color_offset(options.seed ? options.seed : static_cast<int>(std::random_device{}() % 256)),
    m_invert(options.invert), m_animate(options.animate), m_truecolor_mode(options.truecolor || is_truecolor()),
    m_force_term(options.force) {}

void Rainbow::process(std::istream &in) {
    if (m_animate && is_tty())
        std::cout << term::hide_cursor;

    std::string line;
    while (std::getline(in, line)) {
        if (m_animate) {
            animate_line(line);
        } else {
            print_line(line);
            ++m_line_count;
        }
    }

    if (is_tty())
        std::cout << term::show_cursor << term::reset;
}

std::string Rainbow::rainbow(const float freq, const float pos) const {
    constexpr float pi = std::numbers::pi_v<float>;
    const auto r = static_cast<uint8_t>(std::sin(freq * pos) * 127 + 128);
    const auto g = static_cast<uint8_t>(std::sin(freq * pos + 2 * pi / 3) * 127 + 128);
    const auto b = static_cast<uint8_t>(std::sin(freq * pos + 4 * pi / 3) * 127 + 128);
    return format_color(r, g, b);
}

std::string Rainbow::format_color(const uint8_t r, const uint8_t g, const uint8_t b) const {
    const auto bg_fg = m_invert ? 48 : 38;
    if (m_truecolor_mode)
        return ff::format("\x1b[{};2;{};{};{}m", bg_fg, r, g, b);
    return ff::format("\x1b[{};5;{}m", bg_fg, rgb_to_256(r, g, b));
}


void Rainbow::print_line(const std::string &line, bool /*animate*/) const {
    static const std::regex pattern(R"((\x1B\[[0-?]*[ -/]*[@-~])|([^\x1B]))");
    std::sregex_iterator it(line.begin(), line.end(), pattern);
    int char_index = 0;
    for (const std::sregex_iterator end; it != end; ++it) {
        const auto &match = *it;
        if (match[1].matched) {
            std::cout << match[1].str();
        } else if (match[2].matched) {
            const float pos = (static_cast<float>(m_color_offset + m_line_count + char_index)) / m_spread;
            std::cout << rainbow(m_freq, pos) << match[2].str()
                      << (m_invert ? term::RESET_BACKGROUND : term::RESET_FOREGROUND);
            ++char_index;
        }
    }
    std::cout << '\n';
}

void Rainbow::animate_line(const std::string &line) {
    std::cout << term::save_pos;
    const int original_os = m_color_offset;
    for (int i = 0; i < m_duration; ++i) {
        std::cout << term::restore_pos;
        m_color_offset += static_cast<int>(m_spread);
        print_line(line, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000 / m_speed)));
    }
    m_color_offset = original_os;
    ++m_line_count;
}

bool Rainbow::is_tty() const { return m_force_term || term::is_tty(fileno(stdout)); }
