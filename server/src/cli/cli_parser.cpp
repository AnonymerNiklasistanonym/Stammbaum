#include "cli_parser.hpp"
#include "config.hpp"


Options parseOptions(int argc, char *argv[])
{
    return structopt::app(name.data(), version.data()).parse<Options>(argc, argv);
}
