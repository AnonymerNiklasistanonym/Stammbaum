#pragma once

#include <string_view>


namespace app_info {

constexpr std::string_view name = "${PROJECT_NAME}";
constexpr std::string_view version = "${VERSION}";

}

namespace open_source_license {

constexpr std::string_view catch2 = "${LICENSE_CATCH2}";
constexpr std::string_view date = "${LICENSE_DATE}";
constexpr std::string_view drogon = "${LICENSE_DROGON}";
constexpr std::string_view fmt = "${LICENSE_FMT}";
constexpr std::string_view indicators = "${LICENSE_INDICATORS}";
constexpr std::string_view rapidjson = "${LICENSE_RAPIDJSON}";
constexpr std::string_view spdlog = "${LICENSE_SPDLOG}";
constexpr std::string_view sqlpp11 = "${LICENSE_SQLPP11}";
constexpr std::string_view sqlppConnector11Mysql = "${LICENSE_SQLPP11_CONNECTOR_MYSQL}";
constexpr std::string_view structopt = "${LICENSE_STRUCTOPT}";

}
