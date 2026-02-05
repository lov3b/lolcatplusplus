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

#include "args.hpp"

#include <boost/program_options.hpp>
#include <cstdio>
#include <iostream>
#include <sstream>

#include "rainbow.hpp"
#include "terminal.hpp"
#include "version.hpp"

namespace po = boost::program_options;

std::optional<cli::Args> cli::parse_args(const int argc, char *argv[]) noexcept {
    Args args{};
    Options &options = args.args;

    po::options_description desc("lolcat++ " PROJECT_VERSION "\n"
                                 "\n"
                                 "Lolcat++ is a rewrite, in C++, of the popular program lolcat.\n"
                                 "It is aimed to provide higher speed, and thus be more usable in\n"
                                 "init scripts, and such\n"
                                 "\n"
                                 "Options");

    std::string_view footer("Bugs/feature-requests: <https://github.com/lolcatpp/lolcatpp/issues>\n"
                            "Lolcat++ home page:    <https://github.com/lolcatpp/lolcatpp>\n");

    try {
        // clang-format off
        desc.add_options()
                ("help,h", "Show help")
                ("version,v", "Show version")
                ("spread,p", po::value<float>(&options.spread)->default_value(3.0f, "3.0"),
                 "Rainbow spread")
                ("freq,F", po::value<float>(&options.freq)->default_value(0.1f, "0.1"),
                 "Rainbow frequency")
                ("seed,S", po::value<int>(&options.seed)->default_value(0),
                 "Rainbow seed (0=random)")
                ("animate,a", po::bool_switch(&options.animate),
                 "Enable animation")
                ("duration,d", po::value<int>(&options.duration)->default_value(12),
                 "Animation duration")
                ("speed,s", po::value<float>(&options.speed)->default_value(20.0f, "20.0"),
                 "Animation speed")
                ("invert,i", po::bool_switch(&options.invert),
                 "Invert colors")
                ("truecolor,t", po::bool_switch(&options.truecolor),
                 "Force truecolor mode")
                ("force,f", po::bool_switch(&options.force),
                 "Force color output")
                ("files", po::value<std::vector<std::string> >(),
                 "Input files (also positional)");
        // clang-format on

        po::positional_options_description p;
        p.add("files", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // We don't want to colorize the output here in automated testing
        // or make it hard for other programs.
        auto show_info = [&](std::stringstream &ss) {
            if (options.force || term::is_tty(stdout)) {
                Rainbow rainbow(options);
                rainbow.process(ss);
            } else {
                std::cout << ss.str();
            }
        };

        if (vm.contains("help")) {
            std::stringstream ss;
            ss << desc << "\n\n" << footer;
            show_info(ss);
            return std::nullopt;
        }

        if (vm.contains("version")) {
            std::stringstream ss;
            ss << "lolcat++ version " << PROJECT_VERSION << '\n';
            show_info(ss);
            return std::nullopt;
        }
        args.files = vm.contains("files") ? vm["files"].as<std::vector<std::string>>() : std::vector<std::string>{"-"};
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return std::nullopt;
    }

    return {args};
}
