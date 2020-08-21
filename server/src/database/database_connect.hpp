#pragma once
#include <optional>
#include <sqlpp11/mysql/mysql.h>


sqlpp::mysql::connection connectToDb(const std::string &db_name, const std::string &db_user_name,
                                     const std::optional<std::string> &db_user_password = { "" },
                                     const std::optional<bool> &debug = { false } );
