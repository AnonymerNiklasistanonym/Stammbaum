#include <memory>

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/spdlog.h"

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include "TabSample.h"

#include <drogon/drogon.h>

#include "cli/cli_parser.hpp"
#include "std_fix/std_format_fix.hpp"
#include "logging/logging_globals.hpp"


int main(int argc, char *argv[])
{
    std::shared_ptr<spdlog::logger> main_logger {};
    try {
        // Create a log sink for daily log files (a new file is created every day on 23:59)
        auto sharedDailyFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/daily.log", 23,
                                                                                       59);
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
        main_logger = std::make_shared<spdlog::logger>(main_logger_name.data(), std::begin(sinks),
                                                       std::end(sinks));
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

    std::shared_ptr<Options> options {};
    try {
        // Parse CLI args
        options = std::make_shared<Options>(parseOptions(argc, argv));
        // Print parsed arguments:
        main_logger->info("port = {}", options->port.value());
    } catch (structopt::exception &e) {
        main_logger->error("{}",  e.what());
        std::cout << e.help() << std::endl;
        return EXIT_FAILURE;
    }

    // Setup MySQL database
    sqlpp::mysql::global_library_init();

    auto config = std::make_shared<sqlpp::mysql::connection_config>();
    config->user = "cpptestuser";
    config->database = "cpptestdb";
    config->debug = true;
    try {
        sqlpp::mysql::connection db(config);
    } catch (const sqlpp::exception &e) {
        std::cerr <<
                  "For testing, you'll need to create a database yourdb for user root (no password)" <<
                  std::endl;
        std::cerr << e.what() << std::endl;
        return 1;
    }
    try {
        sqlpp::mysql::connection db(config);
        db.execute(R"(DROP TABLE IF EXISTS tab_sample)");
        db.execute(R"(CREATE TABLE tab_sample (
			alpha bigint(20) AUTO_INCREMENT,
			beta varchar(255) DEFAULT NULL,
			gamma bool DEFAULT NULL,
			PRIMARY KEY (alpha)
			))");
        db.execute(R"(DROP TABLE IF EXISTS tab_foo)");
        db.execute(R"(CREATE TABLE tab_foo (
		omega bigint(20) DEFAULT NULL
			))");

        assert(not db(sqlpp::select(sqlpp::value(false).as(sqlpp::alias::a))).front().a);

        const auto tab = TabSample{};
        // clear the table
        db(sqlpp::remove_from(tab).unconditionally());

        // Several ways of ensuring that tab is empty
        assert(not db(sqlpp::select(exists(sqlpp::select(tab.alpha).from(tab).unconditionally())))
               .front()
               .exists);  // this is probably the fastest
        assert(not db(sqlpp::select(count(tab.alpha)).from(tab).unconditionally()).front().count);
        assert(db(sqlpp::select(tab.alpha).from(tab).unconditionally()).empty());

        // explicit sqlpp::all_of(tab)
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
        sqlpp::select(sqlpp::all_of(tab)).from(tab);
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
        db(sqlpp::select(sqlpp::all_of(tab)).from(tab).unconditionally());
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
        for (const auto &row : db(sqlpp::select(sqlpp::all_of(tab)).from(tab).unconditionally())) {
            std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
            std::cerr << "row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " <<
                      row.gamma
                      << std::endl;
        };
        // sqlpp::selecting two multicolumns
        for (const auto &row :
                db(sqlpp::select(tab.alpha, sqlpp::multi_column(tab.alpha, tab.beta, tab.gamma).as(sqlpp::alias::left),
                          sqlpp::multi_column(sqlpp::all_of(tab)).as(tab))
                   .from(tab)
                   .unconditionally())) {
            std::cerr << "row.left.alpha: " << row.left.alpha << ", row.left.beta: " << row.left.beta
                      << ", row.left.gamma: " << row.left.gamma << std::endl;
            std::cerr << "row.tabSample.alpha: " << row.tabSample.alpha << ", row.tabSample.beta: " <<
                      row.tabSample.beta
                      << ", row.tabSample.gamma: " << row.tabSample.gamma << std::endl;
        };

        // insert
        db(sqlpp::insert_into(tab).default_values());
        const auto x = sqlpp::select(sqlpp::all_of(tab)).from(tab).unconditionally();
        const auto y = db.prepare(x);
        for (const auto &row : db(db.prepare(sqlpp::select(sqlpp::all_of(tab)).from(tab).unconditionally()))) {
            std::cerr << "alpha: " << row.alpha.is_null() << std::endl;
            std::cerr << "beta: " << row.beta.is_null() << std::endl;
            std::cerr << "gamma: " << row.gamma.is_null() << std::endl;
        }
        db(sqlpp::insert_into(tab).set(tab.beta = "kaesekuchen", tab.gamma = true));
        db(sqlpp::insert_into(tab).default_values());
        db(sqlpp::insert_into(tab).set(tab.beta = "", tab.gamma = true));

        // sqlpp::update
        db(sqlpp::update(tab).set(tab.gamma = false).where(tab.alpha.in(sqlpp::value_list(std::vector<int> {1, 2, 3, 4}))));
        db(sqlpp::update(tab).set(tab.gamma = true).where(tab.alpha.in(1)));

        // remove
        db(sqlpp::remove_from(tab).where(tab.alpha == tab.alpha + 3));

        std::cerr << "+++++++++++++++++++++++++++" << std::endl;
        for (const auto &row : db(sqlpp::select(sqlpp::all_of(tab)).from(tab).unconditionally())) {
            std::cerr << __LINE__ << " row.beta: " << row.beta << std::endl;
        }
        std::cerr << "+++++++++++++++++++++++++++" << std::endl;
        decltype(db(sqlpp::select(sqlpp::all_of(tab)).from(tab).unconditionally())) result;
        result = db(sqlpp::select(sqlpp::all_of(tab)).from(tab).unconditionally());
        std::cerr << "Accessing a field directly from the result (using the current row): " <<
                  result.begin()->alpha
                  << std::endl;
        std::cerr << "Can do that again, no problem: " << result.begin()->alpha << std::endl;

        auto tx = start_transaction(db);
        if (const auto &row =
                    *db(sqlpp::select(sqlpp::all_of(tab), sqlpp::select(max(tab.alpha)).from(tab)).from(tab).unconditionally()).begin()) {
            int a = row.alpha;
            int m = row.max;
            std::cerr << __LINE__ << " row.alpha: " << a << ", row.max: " << m << std::endl;
        }
        tx.commit();

        TabFoo foo;
        for (const auto &row : db(sqlpp::select(tab.alpha).from(tab.join(foo).on(tab.alpha ==
                                                                          foo.omega)).unconditionally())) {
            std::cerr << row.alpha << std::endl;
        }

        for (const auto &row :
                db(sqlpp::select(tab.alpha).from(tab.left_outer_join(foo).on(tab.alpha == foo.omega)).unconditionally())) {
            std::cerr << row.alpha << std::endl;
        }

        auto ps = db.prepare(sqlpp::select(sqlpp::all_of(tab))
                             .from(tab)
                             .where(tab.alpha != sqlpp::parameter(tab.alpha) and tab.beta != sqlpp::parameter(tab.beta) and
                                    tab.gamma != sqlpp::parameter(tab.gamma)));
        ps.params.alpha = 7;
        ps.params.beta = "wurzelbrunft";
        ps.params.gamma = true;
        for (const auto &row : db(ps)) {
            std::cerr << "bound result: alpha: " << row.alpha << std::endl;
            std::cerr << "bound result: beta: " << row.beta << std::endl;
            std::cerr << "bound result: gamma: " << row.gamma << std::endl;
        }

        std::cerr << "--------" << std::endl;
        ps.params.gamma = "false";
        for (const auto &row : db(ps)) {
            std::cerr << "bound result: alpha: " << row.alpha << std::endl;
            std::cerr << "bound result: beta: " << row.beta << std::endl;
            std::cerr << "bound result: gamma: " << row.gamma << std::endl;
        }

        std::cerr << "--------" << std::endl;
        ps.params.beta = "kaesekuchen";
        for (const auto &row : db(ps)) {
            std::cerr << "bound result: alpha: " << row.alpha << std::endl;
            std::cerr << "bound result: beta: " << row.beta << std::endl;
            std::cerr << "bound result: gamma: " << row.gamma << std::endl;
        }

        auto pi = db.prepare(sqlpp::insert_into(tab).set(tab.beta = sqlpp::parameter(tab.beta), tab.gamma = true));
        pi.params.beta = "prepared cake";
        std::cerr << "Inserted: " << db(pi) << std::endl;

        auto pu = db.prepare(sqlpp::update(tab).set(tab.gamma = sqlpp::parameter(tab.gamma)).where(
                                 tab.beta == "prepared cake"));
        pu.params.gamma = false;
        std::cerr << "Updated: " << db(pu) << std::endl;

        auto pr = db.prepare(sqlpp::remove_from(tab).where(tab.beta != sqlpp::parameter(tab.beta)));
        pr.params.beta = "prepared cake";
        std::cerr << "Deleted lines: " << db(pr) << std::endl;

        for (const auto &row : db(sqlpp::select(sqlpp::case_when(tab.gamma).then(tab.alpha).else_(foo.omega).as(
                                             tab.alpha))
                                  .from(tab.cross_join(foo))
                                  .unconditionally())) {
            std::cerr << row.alpha << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }


    //Set HTTP listener address and port
    drogon::app().addListener(options->ipAddress.value(), options->port.value());
    //Load config file
    //drogon::app().loadConfigFile("../config.json");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
}
