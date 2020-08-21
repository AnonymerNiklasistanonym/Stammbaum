#include "cli_parser.hpp"
#include "config.hpp"
#include "spdlog/spdlog.h"
#include "std_fix/std_format_fix.hpp"
#include "logging/logging_globals.hpp"

void printOpenSourceInformation ()
{
    spdlog::get(main_logger_name.data())->info("printOpenSourceInformation");

    std::cout << "Open source licenses:\n"
              << format::format("> catch2 ({} | {}):\n{}\n",
                                open_source_git_url::catch2,
                                open_source_git_hash::catch2,
                                open_source_license::catch2)
              << format::format("> drogon ({} | {}):\n{}\n",
                                open_source_git_url::drogon,
                                open_source_git_hash::drogon,
                                open_source_license::drogon)
              << format::format("> fmt ({} | {}):\n{}\n",
                                open_source_git_url::fmt,
                                open_source_git_hash::fmt,
                                open_source_license::fmt)
              << format::format("> indicators ({} | {}):\n{}\n",
                                open_source_git_url::indicators,
                                open_source_git_hash::indicators,
                                open_source_license::indicators)
              << format::format("> rapidjson ({} | {}):\n{}\n",
                                open_source_git_url::rapidjson,
                                open_source_git_hash::rapidjson,
                                open_source_license::rapidjson)
              << format::format("> spdlog ({} | {}):\n{}\n",
                                open_source_git_url::spdlog,
                                open_source_git_hash::spdlog,
                                open_source_license::spdlog)
              << format::format("> sqlpp11 ({} | {}):\n{}\n",
                                open_source_git_url::sqlpp11,
                                open_source_git_hash::sqlpp11,
                                open_source_license::sqlpp11)
              << format::format("> sqlpp11 ({} | {}):\n{}\n",
                                open_source_git_url::sqlppConnector11Mysql,
                                open_source_git_hash::sqlppConnector11Mysql,
                                open_source_license::sqlppConnector11Mysql)
              << format::format("> structopt ({} | {}):\n{}\n",
                                open_source_git_url::structopt,
                                open_source_git_hash::structopt,
                                open_source_license::structopt)
              << std::endl;
}


Options parseOptions(int argc, char *argv[])
{
    const auto options = structopt::app(app_info::name.data(),
                                        app_info::version.data()).parse<Options>(argc, argv);
    if (options.openSourceLicenses.value()) {
        printOpenSourceInformation();
        exit(EXIT_SUCCESS);
    }
    return options;
}
