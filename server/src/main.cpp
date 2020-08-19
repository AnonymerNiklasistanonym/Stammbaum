#include "cli/cli_parser.hpp"


int main(int argc, char *argv[])
{
    try {
        auto options = parseOptions(argc, argv);

        // Print parsed arguments:
        std::cout << "port = " << options.port.value() << "\n";

    } catch (structopt::exception &e) {
        std::cout << e.what() << "\n";
        std::cout << e.help();
    }
}
