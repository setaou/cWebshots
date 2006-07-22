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


/** \file
Support functions for cWebshots.
Headers are defined in cWebshots.h
*/

#include "Misc.h"

#include <cstdio>
#include <sstream>
#include <ctime>

using namespace std;

namespace cWebshots
{

	/**
	Extracts the file name from a path.
	This function assumes that in the path, a directory always ends with a "/"

	\param[in] path The path you want to extract the file name

	\return The file name
	*/
	string getFileName(string path)
	{
		size_t start = path.rfind("/") + 1;

		return path.substr(start,path.length() - start);
	}

	/**
	Extracts the directory from a path.
	This function assumes that in the path, a directory always ends with a "/"

	\param[in] path The path you want to extract the directory

	\return The directory
	*/
	string getDirectory(string path)
	{
		size_t start = path.rfind("/") + 1;

		return path.substr(0,start);
	}

	/**
	Identifies the type of a file.

	\param[in] File The file to identify the type.

	\return The type of the file (WB1, WBZ, WBC, Unknown)
	*/
	Webshots_Formats identifyFile(string File)
	{
		ifstream in_file;

		// Open the file
		in_file.open(File.c_str(), ios_base::in | ios_base::binary);

		if (in_file.is_open())
		{
			return identifyFile(in_file);
		}
		else
		{
			Exceptions::FileSystemError Err("Unable to open the file \"" + File + "\" for reading.");
			throw Err;
		}
	}

	/**
	Identifies the type of a file.

	\param[in] in_file Reference to the stream of the file to identify the type.

	\return The type of the file (WB1, WBZ, WBC, Unknown)
	*/
	Webshots_Formats identifyFile(ifstream &in_file)
	{
		long File_Type;
		Webshots_Formats Type;

		// Read the first 4 bytes of the file
		in_file.seekg(0);
		in_file.read((char *)&File_Type, 4);

		switch (File_Type)
		{
		// WBC file
		case WBC_File_Marker:
			Type = WBC;
			break;

		// WBZ file
		case WBZ_File_Marker:
			Type = WBZ;
			break;

		// JPG File / WB1 File
		case WB1_File_Marker:
		case JFIF_File_Marker: 
		case EXIF_File_Marker:
			Type = WB1;
			break;

		// Unknown file format
		default:
			Type = Unknown;
		}

		return Type;
	}


	/**
	Explode a string into a vector of strings, according to a delimiter.

	\param[in] str String to split
	\param[in] tokens Destination vector
	\param[in] delimiters Delimiter
	*/
	void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ")
	{
		// Skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);
		// Find first "non-delimiter".
		string::size_type pos = str.find_first_of(delimiters, lastPos);

		while (string::npos != pos || string::npos != lastPos)
		{
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);
			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}
	}

	/**
	Extracts the value of a couple section+field of an INI-like string.

	\param[in] Data INI-like data
	\param[in] Section Section of the data to read
	\param[in] Field Field to read

	\return The value of the field "Field" of the section "Section"

	\exception Exceptions::CorruptedData Thrown if the Field/Section has not been found
	*/
	string getINI(const string &Data, const string &Section, const string &Field)
	{
		vector<string> Lines;

		// Fill the vector Lines with each line of Data
		if (Data.find("\r\n") != string::npos)     // \r\n = Windows line break
			tokenize(Data, Lines, "\r\n");
		else if (Data.find("\n") == string::npos)  // \r = Mac line break
			tokenize(Data, Lines, "\r");
		else                                       // \n = UNIX line break
			tokenize(Data, Lines, "\n");

		unsigned int l;
		bool header_found = false;

		// For each line
		for (l = 0; l < Lines.size(); l++)
		{
			// Found the header of the section
			if (Lines[l] == "[" + Section + "]")
				header_found = true;
			// Found the beginning of the next section
			else if (header_found && (Lines[l].find("[") == 0))
			{
				Exceptions::CorruptedData Err("The field \"" + Field + "\" was not foud in the section \"" + Section + "\".");
				throw Err;
			}
			// Found the field
			else if (header_found && (Lines[l].find(Field + "=") != string::npos))
			{
				return Lines[l].substr(Field.size() + 1);
			}
		}

		Exceptions::CorruptedData Err("The section \"" + Section + "\" was not found.");
		throw Err;
	}

	/**
	Converts a long integer into a string.

	\param[in] i Long integer to convert

	\return A string which represents the integer.
	*/
	string longToStr(const long &i)
	{
		stringstream str;
		str << i;
		return str.str();
	}

	/**
	Converts a string into a long integer.

	\param[in] s String to convert

	\return A long integer which represents the number in the string, if there is one.
	*/
	long strToLong(const string &s)
	{
		stringstream str(s);
		long l = 0;

		str >> l;

		return l;
	}

	/**
	Converts a date from a number in the format YYYYMMDD to a timestamp.

	\param[in] date The date to convert
	
	\return A timestamp of the given date.
	*/
	unsigned long dateYYYYMMDD(const unsigned long &date)
	{
		int year, month, day;

		year = date / 10000;
		month = (date - (year * 10000)) / 100;
		day = date - (year * 10000) - (month * 100);

		struct tm time;
		time.tm_year = year - 1900;
		time.tm_mon = month - 1;
		time.tm_mday = day;
		time.tm_hour = 0;
		time.tm_min = 0;
		time.tm_sec = 0;

		return (unsigned long)mktime(&time);
	}
}
