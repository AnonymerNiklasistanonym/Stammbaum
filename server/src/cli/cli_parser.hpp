#include <structopt/app.hpp>


struct Options {
    /**
     * The server port
     */
    std::optional<unsigned int> port = 3000;
    std::optional<bool> openSourceLicenses = true;
};
STRUCTOPT(Options, port, openSourceLicenses);

void printOpenSourceInformation();

Options parseOptions(int argc, char *argv[]);
