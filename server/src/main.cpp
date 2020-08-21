#include <memory>

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/spdlog.h"

#include "database/database_connect.hpp"
#include <sqlpp11/sqlpp11.h>

#include <drogon/drogon.h>

#include "cli/cli_parser.hpp"
#include "std_fix/std_format_fix.hpp"
#include "logging/logging_globals.hpp"


int main(int argc, char *argv[])
{
    std::shared_ptr<spdlog::logger> main_logger {};
    try {
        // Create a log sink for daily log files (a new file is created every day on 23:59)
        auto sharedDailyFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/daily.log",
                                                                                       23, 59);
        sharedDailyFileSink->set_level(spdlog::level::debug);
        sharedDailyFileSink->set_pattern("[%Y-%m-%d %T:%e] [%l] [%n] [%P - %t] %v");
        // Create a console sink for console outputs (no fancy formatting)
        auto sharedConsoleSink = std::make_shared<spdlog::sinks::stdout_sink_st>();
        sharedConsoleSink->set_level(spdlog::level::info);
        sharedConsoleSink->set_pattern("%v");

        // Create a combined logger that writes to the file and console
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(sharedConsoleSink);
        sinks.push_back(sharedDailyFileSink);
        main_logger = std::make_shared<spdlog::logger>(main_logger_name.data(),
                                                       std::begin(sinks), std::end(sinks));
        main_logger->set_level(spdlog::level::debug);
        // Register the logger such that it can be reused across compilation units
        spdlog::register_logger(main_logger);

        main_logger->debug("Loggers were successfully initialized");
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << format::format("Log init failed: {}", ex.what()) << std::endl;
        return EXIT_FAILURE;
    }

    std::shared_ptr<Options> options {};
    try {
        options = std::make_shared<Options>(parseOptions(argc, argv));
        main_logger->debug("CLI arguments were successfully parsed");
    } catch (structopt::exception &e) {
        main_logger->error("{}",  e.what());
        std::cout << e.help() << std::endl;
        return EXIT_FAILURE;
    }

    // Setup MySQL database
    std::shared_ptr<sqlpp::mysql::connection> db {};
    try {
        db = std::make_shared<sqlpp::mysql::connection>(connectToDb(options->mySqlDbName,
                                                                    options->mySqlUserName,
                                                                    options->mySqlUserPassword,
                                                                    options->mySqlDebug));
        main_logger->debug("MySQL database connection was successfully initialized on {}@{}",
                           options->mySqlUserName, options->mySqlDbName);
    } catch (const sqlpp::exception &e) {
        main_logger->error("There were problems when trying to connect to the MySQL database: \"{}\"",
                           e.what());
        return EXIT_FAILURE;
    }

    //Set HTTP listener address and port
    drogon::app().addListener(options->ipAddress.value(), options->port.value());
    //Load config file
    //drogon::app().loadConfigFile("../config.json");
    //Run HTTP framework,the method will block in the internal event loop
    main_logger->debug("Drogon http framework was successfully initialized and will be run on {}:{}",
                       options->ipAddress.value(), options->port.value());
    main_logger->flush();
    drogon::app().run();
    main_logger->debug("Drogon http framework was stopped");
    // Drogon will stop the program itself when an error happens in the server logic

    // Shutdown all registered loggers.
    spdlog::shutdown();
}
