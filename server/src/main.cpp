#include "cli/cli_parser.hpp"
#include "spdlog/sinks/daily_file_sink.h"
#include "std_fix/std_format_fix.hpp"



int main(int argc, char *argv[])
{
    try {
        // Create a daily logger - a new file is created every day on 00:00
        auto logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.log", 0, 0);
        logger->warn("text warn");
        logger->info("text info");
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << format::format("Log init failed: {}", ex.what()) << std::endl;
        return EXIT_FAILURE;
    }

    try {
        // Parse CLI args
        auto options = parseOptions(argc, argv);
        // Print parsed arguments:
        std::cout << format::format("port = {}", options.port.value()) << std::endl;
    } catch (structopt::exception &e) {
        std::cout << format::format("{}\n{}", e.what(), e.help());
        return EXIT_FAILURE;
    }
}
