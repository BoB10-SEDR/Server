#pragma once
#include <regex>

static std::regex regexNumber("\\d+");
static std::regex regexLimit("\\d{2}");
static std::regex regexDate("\\d{4}-(0[1-9]|1[012])-(0[1-9]|[12][0-9]|3[01])");