//
// Created by wansong on 11/03/2018.
//

#define CATCH_CONFIG_MAIN
#include <third-party/catch/catch.hpp>
#include <cacheproxy/http/header.h>
#include <cacheproxy/http/constants.h>
#include <string>
#include <iostream>

using namespace std;

SCENARIO( "header manipulation", "[vary]" ) {
    GIVEN("vary=user-agent") {
      string header = "User-Agent";
      smartstudy::field_value parsed;
      smartstudy::parse_http_field(header, parsed);
      REQUIRE(parsed.size() == 1);
      REQUIRE(parsed[0][smartstudy::SS_HTTP_HEADER_FIELD_VALUE] == "User-Agent");
    }

    GIVEN("vary=user-agent ") {
      string header = "User-Agent ";
      smartstudy::field_value parsed;
      smartstudy::parse_http_field(header, parsed);
      REQUIRE(parsed.size() == 1);
      REQUIRE(parsed[0][smartstudy::SS_HTTP_HEADER_FIELD_VALUE] == "User-Agent");
    }

    GIVEN("vary=user-agent, accept-language") {
      string header = "User-Agent, accept-language";
      smartstudy::field_value parsed;
      smartstudy::parse_http_field(header, parsed);
      REQUIRE(parsed.size() == 2);
      REQUIRE(parsed[0][smartstudy::SS_HTTP_HEADER_FIELD_VALUE] == "User-Agent");
      REQUIRE(parsed[1][smartstudy::SS_HTTP_HEADER_FIELD_VALUE] == "accept-language");
    }


    GIVEN("content-type=text/plain; q = 0.5; charset=\"utf-8\"") {
      std::string header = "text/plain; q = 0.5; charset=\"utf-8\"";
      smartstudy::field_value fv;
      smartstudy::parse_http_field(header, fv);
      REQUIRE(fv.size() == 1);
      std::map<std::string, std::string> plain_params = fv[0];
      REQUIRE(plain_params[smartstudy::SS_HTTP_HEADER_FIELD_VALUE] == "text/plain");
      REQUIRE(plain_params["q"] == "0.5");
      REQUIRE(plain_params["charset"] == "utf-8");
    }

    GIVEN("content-type=text/plain; q = 0.5; charset=\"utf-8\", text/html; q = 1; charset=\"utf16\"") {
      std::string header =
        "text/plain; q = 0.5; charset=\"utf-8\", "
          "text/html; q = 1; charset=\"utf16\"";
      smartstudy::field_value fv;
      smartstudy::parse_http_field(header, fv);
      REQUIRE(fv.size() == 2);
      std::map<std::string, std::string> plain_params = fv[0];
      std::map<std::string, std::string> html_params = fv[1];
      REQUIRE(plain_params[smartstudy::SS_HTTP_HEADER_FIELD_VALUE] == "text/plain");
      REQUIRE(plain_params["q"] == "0.5");
      REQUIRE(plain_params["charset"] == "utf-8");

      REQUIRE(html_params[smartstudy::SS_HTTP_HEADER_FIELD_VALUE] == "text/html");
      REQUIRE(html_params["q"] == "1");
      REQUIRE(html_params["charset"] == "utf16");
    }
}
