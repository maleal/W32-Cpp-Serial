#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <locale>         // std::locale, std::isdigit

bool is_number(const std::string& s);

void my_split(const std::string& str, std::vector<std::string>& tokens, const std::string& delim);

int GetYPRValuesFromVector(std::vector<std::string> &tokens, float&fYaw, float&fPitch, float&fRoll);

#endif //UTIL_H