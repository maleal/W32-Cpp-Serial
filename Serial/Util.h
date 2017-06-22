#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <string>
//#include <ctime>
#include <locale>         // std::locale, std::isdigit
#include <ctime>
#include <ratio>
#include <chrono>




bool is_number(const std::string& s);

void my_split(const std::string& str, std::vector<std::string>& tokens, const std::string& delim);

int GetYPRValuesFromVector(std::vector<std::string> &tokens, float&fYaw, float&fPitch, float&fRoll, float&fdeltaT);

int la_puta_madre();

//using namespace std;
extern std::chrono::steady_clock::time_point beforeT;
double msTimeElapsed();



#endif //UTIL_H