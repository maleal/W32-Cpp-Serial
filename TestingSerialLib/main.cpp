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
#include <tchar.h>
#include "Serial.h"
#include "Overlapped.h"
#include <exception>

int main(int argc, char* argv[]) {
	char buffer[1024+1];
	try{
		CAsynSerial Ardu;
		Ardu.Open(_T("COM3"),0,0);
		Ardu.Setup(CSerial::EBaud38400,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
		Ardu.Read(buffer, 1024+1);

	}catch(std::exception &ex) {
		fprintf(stderr, "Cosa:%s", ex.what()); 
	}
	return 0;
}