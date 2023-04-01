//
// Created by andrei on 14.03.23.
// @see https://stackoverflow.com/questions/3152241/case-insensitive-stdstring-find
//
#ifndef RCR_STRING_HELPER_H
#define RCR_STRING_HELPER_H

#include <string>

std::string toUpperCase(const std::string &value);
std::wstring utf82wstring(const std::string& value);

#endif //RCR_STRING_HELPER_H
