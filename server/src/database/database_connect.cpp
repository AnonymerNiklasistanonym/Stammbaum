#include "database_connect.hpp"
#include <sqlpp11/sqlpp11.h>


sqlpp::mysql::connection connectToDb(const std::string &db_name, const std::string &db_user_name,
                                     const std::optional<std::string> &db_user_password, const std::optional<bool> &debug)
{
    sqlpp::mysql::global_library_init();

    auto config = std::make_shared<sqlpp::mysql::connection_config>();
    config->database = db_name;
    config->user = db_user_name;

    if (db_user_password.has_value()) {
        config->password = db_user_password.value();
    }
    if (debug.has_value()) {
        config->debug = debug.value();
    }

    sqlpp::mysql::connection db(config);

    // Temporary setup database before returning it
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

    return db;
}
