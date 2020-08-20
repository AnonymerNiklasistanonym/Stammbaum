#include <structopt/app.hpp>


struct Options {
    /**
     * The server port
     */
    std::optional<uint16_t> port = 3000;
    std::optional<std::string> ipAddress = "0.0.0.0";
    std::optional<bool> openSourceLicenses = false;
};
STRUCTOPT(Options, ipAddress, port, openSourceLicenses);

void printOpenSourceInformation();

Options parseOptions(int argc, char *argv[]);
