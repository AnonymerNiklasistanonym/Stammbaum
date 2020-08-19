#include <structopt/app.hpp>


struct Options {
    /**
     * The server port
     */
    std::optional<unsigned int> port = 3000;
};
STRUCTOPT(Options, port);

Options parseOptions(int argc, char *argv[]);
