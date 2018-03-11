//
// Created by wansong on 11/03/2018.
//

#define CATCH_CONFIG_MAIN
#include <third-party/catch/catch.hpp>
#include <cacheproxy/http/header.h>
#include <string>
#include <iostream>

using namespace std;

SCENARIO( "header manipulation", "[vary]" ) {
    GIVEN("user-agent") {
      string vary = "User-Agent";
      vector<string> parsed;
      smartstudy::parse_vary(vary, parsed);
      REQUIRE(parsed.size() == 1);
      REQUIRE(parsed[0] == "user_agent");
    }

    GIVEN("user-agent,") {
      string vary = "User-Agent,";

      vector<string> parsed;
      smartstudy::parse_vary(vary, parsed);
      REQUIRE(parsed.size() == 1);
      REQUIRE(parsed[0] == "user_agent");
    }

    GIVEN("user-agent ") {
      string vary = "User-Agent ";

      vector<string> parsed;
      smartstudy::parse_vary(vary, parsed);
      REQUIRE(parsed.size() == 1);
      REQUIRE(parsed[0] == "user_agent");
    }

    GIVEN("user-agent, accept-language") {
      string vary = "User-Agent, accept-language";
      vector<string> parsed;
      smartstudy::parse_vary(vary, parsed);
      REQUIRE(parsed.size() == 2);
      REQUIRE(parsed[0] == "user_agent");
      REQUIRE(parsed[1] == "accept_language");
    }
}
