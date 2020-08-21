#pragma once
#include <structopt/app.hpp>


struct Options {
    std::string mySqlDbName;
    std::string mySqlUserName;
    std::optional<std::string> mySqlUserPassword = "";
    std::optional<bool> mySqlDebug = false;
    std::optional<uint16_t> port = 3000;
    std::optional<std::string> ipAddress = "0.0.0.0";
    std::optional<bool> openSourceLicenses = false;
};
STRUCTOPT(Options, mySqlDbName, mySqlUserName, mySqlUserPassword, mySqlDebug, ipAddress, port,
          openSourceLicenses);

void printOpenSourceInformation();

Options parseOptions(int argc, char *argv[]);
