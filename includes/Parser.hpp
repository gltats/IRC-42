#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cctype>


// utils
void replaceString(std::string &subject, const std::string &search, const std::string &replace);
std::string toIrcUpperCase(std::string s);
bool validatePassword(std::string password);