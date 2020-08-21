#pragma once

#include <string_view>


namespace open_source_git_url {

constexpr std::string_view catch2 = "${GIT_URL_CATCH2}";
constexpr std::string_view date = "${GIT_URL_DATE}";
constexpr std::string_view drogon = "${GIT_URL_DROGON}";
constexpr std::string_view fmt = "${GIT_URL_FMT}";
constexpr std::string_view indicators = "${GIT_URL_INDICATORS}";
constexpr std::string_view rapidjson = "${GIT_URL_RAPIDJSON}";
constexpr std::string_view spdlog = "${GIT_URL_SPDLOG}";
constexpr std::string_view sqlpp11 = "${GIT_URL_SQLPP11}";
constexpr std::string_view sqlppConnector11Mysql = "${GIT_URL_SQLPP11_CONNECTOR_MYSQL}";
constexpr std::string_view structopt = "${GIT_URL_STRUCTOPT}";

}

namespace open_source_git_hash {

constexpr std::string_view catch2 = "${GIT_HASH_CATCH2}";
constexpr std::string_view date = "${GIT_HASH_DATE}";
constexpr std::string_view drogon = "${GIT_HASH_DROGON}";
constexpr std::string_view fmt = "${GIT_HASH_FMT}";
constexpr std::string_view indicators = "${GIT_HASH_INDICATORS}";
constexpr std::string_view rapidjson = "${GIT_HASH_RAPIDJSON}";
constexpr std::string_view spdlog = "${GIT_HASH_SPDLOG}";
constexpr std::string_view sqlpp11 = "${GIT_HASH_SQLPP11}";
constexpr std::string_view sqlppConnector11Mysql = "${GIT_HASH_SQLPP11_CONNECTOR_MYSQL}";
constexpr std::string_view structopt = "${GIT_HASH_STRUCTOPT}";

}
