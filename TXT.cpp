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
Definition of cWebshotsTXT members.
Headers are defined in cWebshots.h
*/

#include "TXT.h"

#include <fstream>
#include <cstdio>

using namespace std;

namespace cWebshots
{
	/**
	Constructor
	*/
	TXT::TXT(void)
	{
		Count = 0;
	}

	/**
	Constructor

	\param[in] directory Complete path to the directory which contains photos.txt and/or album.txt file(s).
	*/
	TXT::TXT(string directory)
	{
		Count = 0;
		load(directory);
	}

	/**
	Destructor
	*/
	TXT::~TXT(void)
	{
		if (Count > 0)
			delete[] Pictures;
	}


	/**
	Load photos.txt and/or album.txt from a directory

	\param[in] directory Complete path to the directory which contains photos.txt and/or album.txt file(s).
	*/
	void TXT::load(string directory)
	{
		// File name and pointer
		ifstream in_file;
		string file;

		// Buffer
		char buffer[500];
		string line;

		// Field name & value, and index
		string field, value;
		int index;

		// Position of the separator
		size_t separator;


		// Clear previous data
		if (Count > 0)
			delete[] Pictures;


		// First, "album.txt" file
		file = directory + "album.txt";

		in_file.open(file.c_str(), ios_base::in);

		// If a "album.txt" file is available
		if (in_file.is_open())
		{
			// If the file is not empty
			if (!in_file.eof())
			{
				// Get a line
				in_file.getline(buffer,500,'\n');
				line = buffer;

				// If the line is not empty and contains a field and its value
				if ((line.length() > 0) && (separator = line.find("||")))
				{
					// Get the field name and the value from the line
					field = line.substr(0,separator);
					value = line.substr(separator + 2);

					if (field == "count")
						Count = atoi(value.c_str());
					else
						Count = 0;
				}
				else
					Count = 0;
			}
			else
				Count = 0;


			// If the files contains just 1 record (an album *must* contain only one record)
			if (Count == 1)
			{
				// Until the end of the file
				while (!in_file.eof())
				{
					// Get a line
					in_file.getline(buffer,500,'\n');
					line = buffer;

					// If the line is not empty and contains a field and its value
					if ((line.length() > 0) && (separator = line.find("||")))
					{
						// Get the field name and the value from the line
						field = line.substr(0,separator);
						value = line.substr(separator + 2);

						// Remove the index from the field name
						field = field.substr(0,field.rfind("_"));

						if (field == "ID")
						{
							if (value.substr(0,1) == "-")
								Album.id = value.substr(1, string::npos); // Ignore the leading "-" if any
							else
								Album.id = value;
						}
						else if (field == "TITLE")
							Album.title = value;
						else if (field == "IN_USE")
							Album.in_use = (atoi(value.c_str()) == 1);
						else if (field == "DESCRIPTION")
							Album.description = value;
						else if (field == "TOPIC_NAME")
							Album.topic_name = value;
						else if (field == "CREATE_DATE")
							Album.create_date = atol(value.c_str());
						else if (field == "EDIT_DATE")
							Album.edit_date = atol(value.c_str());
						else if (field == "PHOTO_COUNT")
							Album.photo_count = atoi(value.c_str());
					}
				}
			}

			in_file.close();
		}


		// Then, "photos.txt" file
		file = directory + "photos.txt";

		in_file.clear();
		in_file.open(file.c_str(), ios_base::in);

		// If a "photos.txt" file is available
		if (in_file.is_open())
		{
			// If the file is not empty
			if (!in_file.eof())
			{
				// Get a line
				in_file.getline(buffer,500,'\n');
				line = buffer;

				// If the line is not empty and contains a field and its value
				if ((line.length() > 0) && (separator = line.find("||")))
				{
					// Get the field name and the value from the line
					field = line.substr(0,separator);
					value = line.substr(separator + 2);

					if (field == "count")
						Count = atoi(value.c_str());
					else
						Count = 0;
				}
				else
					Count = 0;
			}
			else
				Count = 0;


			// If the files contains at least 1 record
			if (Count > 0)
			{
				Pictures = new sPhotosTXT[Count];

				// Until the end of the file
				while (!in_file.eof())
				{
					// Get a line
					in_file.getline(buffer,500,'\n');
					line = buffer;

					// If the line is not empty and contains a field and its value
					if ((line.length() > 0) && (separator = line.find("||")))
					{
						// Get the field name and the value from the line
						field = line.substr(0,separator);
						value = line.substr(separator + 2);

						// Get the index of the record
						index = atoi(field.substr(field.rfind("_") + 1).c_str());

						// Remove the index from the field name
						field = field.substr(0,field.rfind("_"));

						if (field == "ID")
						{
							if (value.substr(0,1) == "-")
								Pictures[index].id = value.substr(1, string::npos); // Ignore the leading "-"
							else
								Pictures[index].id = value;
						}
						else if (field == "POSITION")
							Pictures[index].position = value;
						else if (field == "TITLE")
							Pictures[index].title = value;
						else if (field == "CAPTION")
							Pictures[index].caption = value;
						else if (field == "CREDIT")
							Pictures[index].credit = value;
						else if (field == "URL_PHOTO")
							Pictures[index].url_photo = value;
						else if (field == "URL_THUMB")
							Pictures[index].url_thumb = value;
						else if (field == "SOURCE")
							Pictures[index].source = value;
					}
				}
			}

			in_file.close();
		}
	}


	/**
	Return a record, given its index

	\param[in] index Index of the record

	\return The record number <i>index</i>
	*/
	sPhotosTXT& TXT::picture(int index)
	{
		return Pictures[index];
	}

	/**
	Return the number of records

	\return The number of records
	*/
	unsigned int TXT::pictureCount()
	{
		return Count;
	}

	/**
	Return the index of a record, given the filename 

	\param[in] filename File name of the picture
	\param[in] case_sensitive Define if the search is case-sensitive or not

	\return The index of the record or cWebshotsTXT::IndexNotFound if no record was found with the given filename

	\todo Make a case insensitive string comparison
	*/
	int TXT::indexFromFileName(string filename, bool case_sensitive)
	{
		unsigned int i;

		for (i = 0; i < Count; i++)
		{
			if (case_sensitive)
			{
				if (Pictures[i].url_photo == filename)
					return i;
			}
			else
			{
				// This comparation should be case insensitive in order to work with some strange files
				if (Pictures[i].url_photo == filename)
					return i;
			}
		}

		return IndexNotFound;
	}

}
