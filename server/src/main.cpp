#include "cli/cli_parser.hpp"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/spdlog.h"

#include "std_fix/std_format_fix.hpp"
#include "logging/logging_globals.hpp"


int main(int argc, char *argv[])
{
    std::shared_ptr<spdlog::logger> main_logger {};
    try {
        // Create a log sink for daily log files (a new file is created every day on 23:59)
        auto sharedDailyFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/daily.log", 23, 59);
        sharedDailyFileSink->set_level(spdlog::level::debug);
        sharedDailyFileSink->set_pattern("[%Y-%m-%d %T:%e] [%l] [%n] [%P - %t] %v");
        // Create a console sink for console outputs (no fancy formatting)
        auto sharedConsoleSink = std::make_shared<spdlog::sinks::stdout_sink_st>();
        sharedConsoleSink->set_level(spdlog::level::info);
        sharedConsoleSink->set_pattern("%v");

        // Create loggers that only write to the file
        auto firstLogger = std::make_shared<spdlog::logger>("logger1", sharedDailyFileSink);
        auto secondLogger = std::make_shared<spdlog::logger>("logger2", sharedDailyFileSink);

        // Create a combined logger that writes to the file and console
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(sharedConsoleSink);
        sinks.push_back(sharedDailyFileSink);
        main_logger = std::make_shared<spdlog::logger>(main_logger_name.data(), std::begin(sinks), std::end(sinks));
        // Register the logger such that it can be reused across compilation units
        spdlog::register_logger(main_logger);

        // These messages are only in the file
        firstLogger->set_level(spdlog::level::debug);
        secondLogger->set_level(spdlog::level::debug);
        main_logger->set_level(spdlog::level::debug);

        firstLogger->debug("program was started [file_only]");
        secondLogger->debug("program was started [file_only]");
        main_logger->debug("program was started [file_only]");
        // These messages are in the file and the console
        main_logger->info("program was started [file_and_console]");
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << format::format("Log init failed: {}", ex.what()) << std::endl;
        return EXIT_FAILURE;
    }

    try {
        // Parse CLI args
        auto options = parseOptions(argc, argv);
        // Print parsed arguments:
        main_logger->info("port = {}", options.port.value());
    } catch (structopt::exception &e) {
        main_logger->error("{}",  e.what());
        std::cout << e.help() << std::endl;
        return EXIT_FAILURE;
    }
}
