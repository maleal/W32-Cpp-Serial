//	Overlapped.cpp - Using CSerial in overlapped mode (single class to use Serial in overlaped mode).
//
//	Copyright (C) 1999-2017 Mario Leal Fuentes (mariolealfuentes@gmail.com)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include <windows.h>
#include <cstdio>
#include <algorithm>	// remove and remove_if
#include <vector>
#include <tchar.h>
#include <exception>
#include <string>
#include "Overlapped.h"

#include <ctype.h>

std::chrono::steady_clock::time_point beforeT = std::chrono::steady_clock::now();



/*
bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit( *it )) ++it;
	return !s.empty() && it == s.end();
}
*/



int main(int argc, char* argv[]) {
	char buffer[1024+1];

	try{
		
		CAsynSerial Ardu;
		Ardu.Open(_T("COM4"),0,0);
		Ardu.Setup(CSerial::EBaud115200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);

		//Ardu.Write("ready");
		//Ardu.Read(buffer, 1024+1);
		
		std::string myStr;
		std::string delim("|");
				
		float fYaw, fPitch, fRoll, fdeltaT;
		std::vector<std::string> tokens;
		int i = 0;
		do{
			if (Ardu.Readii(buffer, 1024 + 1)) {
#ifdef TC_PRINTF_DEBUG
				fprintf(stdout, "We are reading:[%s]\n", buffer);
#endif//TC_PRINTF_DEBUG

				myStr.assign(buffer);
				//remuving spaces
				myStr.erase(remove_if(myStr.begin(), myStr.end(), isspace), myStr.end());

#ifdef TC_PRINTF_DEBUG
				fprintf(stdout, "WithOut Space[%s]\n", myStr.c_str());
#endif//TC_PRINTF_DEBUG

				my_split(myStr, tokens, delim);
				GetYPRValuesFromVector(tokens, fYaw, fPitch, fRoll, fdeltaT);
			}
			memset(buffer, '\0', 1024+1);
			myStr.clear();
			tokens.clear();
			i++;

		}while(i<500);
		std::cin >> i;
	}catch(std::exception &ex) {
		fprintf(stderr, "Que Cosa:%s", ex.what()); 
	}
	return 0;
};