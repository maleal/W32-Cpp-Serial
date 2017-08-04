//	Serial.cpp - Implementation of the CSerial class
//
//	Copyright (C) 1999-2017 Mario, Leal Fuentes (mariolealfuentes@gmail.com)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#include "Util.h"


/* Mario, Leal Fuentes
*  Controla que la cadena solo contenga digitos o el signo '-' o el '.'
*  de otra forma retorna false.
*/
bool is_number(const std::string& s)
{
	unsigned char c;
    std::string::const_iterator it = s.begin();
	while (it != s.end()){
		c = *it;
		if( (c < '0') || (c > '9'))
			if( c != '-' && c != '.' )
				break;
		++it;
	}
	return ( it == s.end());
}

/*	Mario, Leal Fuentes
*	Obtiene un vector de string, usa el caracter delim para obtener los tokens
*/
void my_split(const std::string& str, std::vector<std::string>& tokens, const std::string& delim)
{
	//El formato del msg que recibo desde Arduino esta armado de la siguiente forma:
	//043Stx|yaw= 80.56|pitch= 18.43|roll= 10.29|End
	std::string tmp;
	size_t prev = 0, pos, lenght = str.length(); 
	do
	{
		pos = str.find(delim, prev);
		
		if((pos == prev) && (prev == 0)) //string begin with delimiter, so ..
			prev+=1;

		if (pos == std::string::npos) {
			pos = lenght;
		}
		if(pos != prev && pos > prev) {
			tmp= str.substr(prev, pos-prev);
			tokens.push_back(tmp);
#ifdef C_PRINTF_DEBUG
			//fprintf(stdout, "Tokens[%s]\n", tmp.c_str());
#endif//C_PRINTF_DEBUG
			prev=pos+1;
		}
		
	}while ( prev < lenght );
}

float GetYPRValuesFromVector(std::vector<std::string> &tokens, float&fYaw, float&fPitch, float&fRoll, float&fdeltaT) {
	size_t pos;
	
	std::string strToken = tokens.at(1);
	pos = strToken.find("yaw=");
	if(pos  != std::string::npos ) {
		std::string Yaw = strToken.substr(pos+4);
		if( is_number(Yaw) ) {
			fYaw = std::stof(Yaw);
#ifdef C_PRINTF_DEBUG
			printf("Yaw[%s]\n",Yaw.c_str());
#endif//C_PRINTF_DEBUG
					//	}
		}
	}else{
			fYaw=0.0f;
	}
	
	strToken = tokens.at(2);
	pos = strToken.find("pitch=");
	if(pos  != std::string::npos ) {
		std::string Pitch = strToken.substr(pos+6);
		if( is_number(Pitch) ) {
			fPitch = std::stof(Pitch);
#ifdef C_PRINTF_DEBUG
			printf("Pitch[%s]\n",Pitch.c_str());
#endif//C_PRINTF_DEBUG
					//	}
		}
	}else{
		fPitch=0.0f;
	}
	
	strToken = tokens.at(3);
	pos = strToken.find("roll=");
	if(pos  != std::string::npos ) {
		std::string Roll = strToken.substr(pos+5);
		if( is_number(Roll) ) {
			fRoll = std::stof(Roll);
#ifdef C_PRINTF_DEBUG
			printf("Yaw[%s]\n",fRoll.c_str());
#endif//C_PRINTF_DEBUG
					//	}
		}
	}else{
			fRoll=0.0f;
	}
	
	strToken = tokens.at(4);
	pos = strToken.find("delta=");
	if(pos  != std::string::npos ) {
		std::string DeltaT = strToken.substr(pos+6);
		if( is_number(DeltaT) ) {
			fdeltaT = std::stof(DeltaT);
#ifdef C_PRINTF_DEBUG
			printf("DeltaT[%s]\n",DeltaT.c_str());
#endif//C_PRINTF_DEBUG
					//	}
		}
	}else{
			fdeltaT=0.0f;
	}
	return 0.0f;
}


/* Mario, Leal Fuentes
 * Get yaw pitch and roll values from previus tokens (string vector)
 * /
int GetYPRValuesFromVector(std::vector<std::string> &tokens, float&fYaw, float&fPitch, float&fRoll, float&fdeltaT) {
	std::vector<std::string>::iterator it;
	size_t pos;
	for(it = tokens.begin(); it != tokens.end(); it++)
	{
			std::string str = *it;
#ifdef C_PRINTF_DEBUG
			fprintf(stdout, "Token[%s]\n",str.c_str());
#endif//C_PRINTF_DEBUG

			
			pos = str.find("yaw=");
			if(pos  != std::string::npos ) {
					//if(str.length() >= 5 ) {
						std::string Yaw = str.substr(pos+4);
						if( is_number(Yaw) ) {
							fYaw = std::stof(Yaw);
#ifdef C_PRINTF_DEBUG
							//fprintf(stdout, "Yaw[%s]\n",Yaw.c_str());
#endif//C_PRINTF_DEBUG
					//	}
					}
			}
			pos = str.find("pitch=");
			if(pos  != std::string::npos) {
				if(str.length() >= 7 ) {
					std::string Pitch = str.substr(pos+6);
					if(is_number(Pitch) ) {
						
						fPitch = std::stof(Pitch);
					}
				}
			}
			pos = str.find("roll=");
			if(pos  != std::string::npos) {
				if(str.length() >= 6 ) {
					std::string Roll = str.substr(pos+5);
					if( is_number(Roll) ){
						
						fRoll = std::stof(Roll);
					}
				}
			}
			pos = str.find("delta=");
			if (pos != std::string::npos) {
				if (str.length() >= 7) {
					std::string DeltaT = str.substr(pos + 6);
					if (is_number(DeltaT)) {

						fdeltaT = std::stof(DeltaT);
#ifdef C_PRINTF_DEBUG
						printf("fdeltaT[%s]\n", DeltaT.c_str());
#endif//C_PRINTF_DEBUG
					}
				}
			}
			
	}
	return 0;
}
*/


double msTimeElapsed() {
	
	double deltaT;
	
	std::chrono::steady_clock::time_point nowT = std::chrono::steady_clock::now();//system_clock();
	deltaT = std::chrono::duration_cast<std::chrono::milliseconds>( nowT- beforeT ).count();
	beforeT = nowT;
	fprintf(stdout, "Time elapsed = %f\n", deltaT);
	return deltaT;
	
}


