/*
    cWebshots - Webshots files handling classes
    Copyright (C) 2005-2006  Hervé "Setaou" BRY <uwc at apinc dot org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _CWEBSHOTS_MISC_H
#define _CWEBSHOTS_MISC_H

#define _CRT_SECURE_NO_DEPRECATE

#include <vector>
#include <string>
#include <fstream>

#include "Exceptions.h"

namespace cWebshots
{

	using namespace std;

	//----------------------------------------------------------
	// Constants
	//----------------------------------------------------------

	// File type marker
	const unsigned long WBC_File_Marker    = 0x95FA16AB;	///< First 4 bytes of a WBC file
	const unsigned long WBC_Unit_Marker    = 0xF071CDE2;	///< First 4 bytes of a WBC unit
	const unsigned long WBZ_File_Marker    = 0x6791AB43;	///< First 4 bytes of a WBZ file
	const unsigned long WBZ_Element_Marker = 0x1082CDE1;	///< First 4 bytes of a WBZ element
	const unsigned long JFIF_File_Marker   = 0xE0FFD8FF;	///< First 4 bytes of a JPEG JFIF File
	const unsigned long EXIF_File_Marker   = 0xE1FFD8FF;	///< First 4 bytes of a JPEG EXIF File
	const unsigned long WB1_File_Marker    = 0x42425757;	///< First 4 bytes of a webshots encrypted JPG file ("WWBB")
	const unsigned long TXT_File_Marker    = 0x6E756F63;	///< First 4 bytes of an album.txt / photos.txt file (="coun")

	/// Webshots file formats
	enum Webshots_Formats
	{
		WB1,		///< Webshots encrypted JPEG
		WBC,		///< Webshots WBC Collection
		WBZ,		///< Webshots WBZ Collection
		JPG,		///< JPEG (JFIF or EXIF)
		Folder,		///< Webshots Album
		Unknown		///< Unknown format
	};

	//----------------------------------------------------------
	// Misc
	//----------------------------------------------------------

	string getFileName(string path);
	string getDirectory(string path);

	Webshots_Formats identifyFile(string File);
	Webshots_Formats identifyFile(ifstream &in_file);

	void tokenize(const string& str, vector<string>& tokens, const string& delimiters);
	string getINI(const string &Data, const string &Section, const string &Field);
	string longToStr(const long &i);
	long strToLong(const string &s);
	unsigned long dateYYYYMMDD(const unsigned long &date);
}


#endif
