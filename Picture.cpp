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
Definition of cWebshotsPicture members.
Headers are defined in cWebshots.h
*/


/** \mainpage Documentation of cWebshots 2.0 beta

\section whatisit What is it ?
cWebshots is a set of classes designed to handle files from Webshots (WBZ, WBC, WB1, WBD ...).

\section quickexplanation Quick explanation
There are two main classes : Picture and  Collection.
- The first, Picture handles individual pictures.\n
  Supported formats are JPG and WB1.\n

- The last, Collection handles collections of pictures. It is a sort of vector of Picture.\n
  Supported formats are WBZ, WBC, WB1, JPG and folders (newest Webshots albums are stored in folders).

In practice, you can handle about everything with just Collection.\n
Affectation and addition operator (=, +, +=) are defined for and between the two classes, so you can easily add
a new picture to a collection by writing for example:
\code
mycollection += mypicture;
\endcode
*/


#include "Picture.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;

namespace cWebshots
{

	//----------------------------------------------------------
	// Class cWebshotsPicture
	//----------------------------------------------------------

	//------------------------------------
	// Constructors / Destructors
	//------------------------------------

	/** 
	Constructor
	*/
	Picture::Picture()
	{
		//AdditionDate = 0;
		//DailyDate = 0;

		JPGFile_Size = 0;
		BMPThumbnail_Size = 0;
	}

	/**
	Destructor
	*/
	Picture::~Picture()
	{
		if (JPGFile_Size > 0)
			delete[] JPGFile;

		if (BMPThumbnail_Size > 0)
			delete[] BMPThumbnail;
	}


	/**
	Constructor

	\param[in] source Source cWebshotsPicture
	*/
	Picture::Picture(const Picture& source)
	{
		*this = source;
	}
	
	/**
	Constructor
	
	\param[in] File File to load
	*/
	Picture::Picture(string File)
	{
		loadFromFile(File);
	}

	//------------------------------------
	// Operators
	//------------------------------------

	/**
	Operator =
	*/
	Picture& Picture::operator=(const Picture& source)
	{
		if (&source != this)
		{
			// Delete the current picture/thumbnail
			if (JPGFile_Size > 0)
				delete[] JPGFile;
			if (BMPThumbnail_Size > 0)
				delete[] BMPThumbnail;

			JPGFile_Size = 0;
			BMPThumbnail_Size = 0;

			// Copy the source
			metadata = source.metadata;

			JPGFile_Size = source.JPGFile_Size;
			BMPThumbnail_Size = source.BMPThumbnail_Size;

			if (JPGFile_Size > 0)
			{
				JPGFile = new unsigned char[JPGFile_Size];
				memcpy(JPGFile, source.JPGFile, JPGFile_Size);
			}
			if (BMPThumbnail_Size > 0)
			{
				BMPThumbnail = new unsigned char[BMPThumbnail_Size];
				memcpy(BMPThumbnail, source.BMPThumbnail, BMPThumbnail_Size);
			}
		}

		return *this;
	}

	//------------------------------------
	// Public data access
	//------------------------------------

	/**
	Returns a pointer to the picture in memory (jpeg)

	\return a pointer to the picture
	*/
	unsigned char *Picture::getPicturePtr() const
	{
		return JPGFile;
	}

	/**
	Returns the size of the picture

	\return  the size of the picture
	*/
	unsigned long Picture::getPictureSize() const
	{
		return JPGFile_Size;
	}

	/**
	Returns a pointer to the thumbnail (bitmap)

	\return a pointer to the thumbnail
	*/
	unsigned char *Picture::getThumbnailPtr() const
	{
		return BMPThumbnail;
	}

	/**
	Returns the size of the thumbnail

	\return  the size of the thumbnail
	*/
	unsigned long Picture::getThumbnailSize() const
	{
		return BMPThumbnail_Size;
	}

	const MetaData& Picture::metaData() const
	{
		return metadata;
	}

	MetaData& Picture::metaData()
	{
		return metadata;
	}

	void Picture::setMetaData(MetaData& metadata)
	{
		this->metadata = metadata;
	}

	vector<Datum::KeyType> Picture::availableKeys()
	{
		vector<Datum::KeyType> ret;
		ret.push_back(Datum::KeyType("album_id", Datum::type_string));
		ret.push_back(Datum::KeyType("album_title", Datum::type_string));
		ret.push_back(Datum::KeyType("category", Datum::type_string));
		ret.push_back(Datum::KeyType("source", Datum::type_string));
		ret.push_back(Datum::KeyType("credits", Datum::type_string));
		ret.push_back(Datum::KeyType("caption", Datum::type_string));
		ret.push_back(Datum::KeyType("title", Datum::type_string));
		ret.push_back(Datum::KeyType("original_filename", Datum::type_string));
		ret.push_back(Datum::KeyType("id", Datum::type_string));
		ret.push_back(Datum::KeyType("daily_date", Datum::type_date));
		ret.push_back(Datum::KeyType("addition_date", Datum::type_date));
		return ret;
	}

	//------------------------------------
	// Load procedures
	//------------------------------------

	/**
	Loads a picture from a file (WBZ/WB1/JPG)

	\param[in] File Complete path to the file to load

	\exception Exceptions::UnknownFormat Thrown if the file has an unknown format
	\exception Exceptions::FileSystemError Thrown if the file cannot be opened for reading
	*/
	void Picture::loadFromFile(string File)
	{
		ifstream in_file;
		long File_Type;

		// Clear everything
		clear();

		// Open the file for reading
		in_file.open(File.c_str(), ios_base::in | ios_base::binary);

		if (in_file.is_open())
		{
			in_file.read((char *)&File_Type, 4);

			switch (File_Type)
			{
			// JPG File / WB1 File
			case WB1_File_Marker:
			case JFIF_File_Marker: 
			case EXIF_File_Marker:
					// Load the file
					loadWB1(in_file);
					// Set default values for essential metadata
					metadata.setString("original_filename", getFileName(File));
					metadata.setString("title", getFileName(File));
					metadata.setString("id", longToStr(time(0) / (rand() + 1) + 100000) + longToStr(time(0) / (rand() + 1) + 200000) + longToStr(time(0) / (rand() + 1) + 300000));
					// Load the metadata
					loadMetaDataTXT(File);
					break;

			// Unsupported file format
			default:
				Exceptions::UnknownFormat Err("The file \"" + File + "\" cannot be opened as a Webshots picture.");
				throw Err;
			}

			in_file.close();
		}
		else
		{
			Exceptions::FileSystemError Err("Unable to open the file \"" + File + "\" for reading.");
			throw Err;
		}
	};

	/**
	Loads a WB1/JPG file

	\param[in] in_file Reference to the stream of the file to load
	*/
	void Picture::loadWB1(ifstream &in_file)
	{
		unsigned char *File;
		unsigned long Size;

		// Get the size of the file
		in_file.seekg(0, ios_base::end);
		Size = (unsigned long)(in_file.tellg()) + 1;
		in_file.seekg(0);

		// Load the file into a buffer
		File = new unsigned char[Size];

		try
		{
			in_file.read((char *)File, Size);

			// Load the picture
			setPicture(File,Size);
		}
		catch (...)
		{
			delete[] File;
			throw;
		}

		delete[] File;
	}

	/**
	Loads the picture from memory.
	The picture format must be WB1 or JPG.

	\param[in] data Pointer to the picture
	\param[in] size Size of the picture in bytes

	\exception Exceptions::UnknownFormat Thrown if the file has an unknown format
	*/
	void Picture::setPicture(const unsigned char *data, const unsigned long size)
	{
		char Key[8];
		char KeyVal = 0;
		long File_Type;
		int i;

		// Delete the previous picture if necessary
		if (JPGFile_Size > 0)
		{
			delete[] JPGFile;
			JPGFile_Size = 0;
		}

		// Get the type of the file
		memcpy(&File_Type,data,4);

		switch (File_Type)
		{
		// Encrypted header
		case WB1_File_Marker:
			// Get the key
			memcpy(Key,data,8);
			if (Key[4] == '0')
				KeyVal = (char)0xA4;
			else if (Key[4] == '1')
				KeyVal = (char)0xF2;
			
			// Load the file into the buffer
			JPGFile_Size = size - 8;
			JPGFile = new unsigned char[JPGFile_Size];
			memcpy(JPGFile,data + 8,size - 8);

			// Decrypt the file
			for (i = 0; i < 100; i++)
			{
				JPGFile[i] = (JPGFile[i + 100] ^ (~JPGFile[i])) ^ KeyVal;
			}
			break;

		// Header not encrypted
		case JFIF_File_Marker:
		case EXIF_File_Marker:
			JPGFile_Size = size;
			JPGFile = new unsigned char[JPGFile_Size];
			memcpy(JPGFile,data,size);
			break;

		// Unknown format
		default:
			Exceptions::UnknownFormat Err("The file has an unknown format.");
			throw Err;
		}
	};

	/**
	Loads the thumbnail from memory.

	\param[in] data Pointer to the thumbnail
	\param[in] size Size of the thumbnail in bytes
	*/
	void Picture::setThumbnail(const unsigned char *data, const unsigned long size)
	{
		// Delete the previous thumbnail if necessary
		if (BMPThumbnail_Size > 0)
			delete[] BMPThumbnail;

		// Load the new one
		BMPThumbnail_Size = size;
		BMPThumbnail = new unsigned char[BMPThumbnail_Size];
		memcpy(BMPThumbnail,data,size);
	};

	/**
	Loads metadata about a WB1 file from photos.txt

	\param[in] file Complete path to the concerned wb1 file
	*/
	void Picture::loadMetaDataTXT(string file)
	{
		TXT txt;
		int index;
		string test;

		txt.load(test = getDirectory(file));
		index = txt.indexFromFileName(test = getFileName(file));

		if (index != TXT::IndexNotFound)
		{
			metadata.setString("id", txt.picture(index).id);
			metadata.setString("original_filename", txt.picture(index).url_photo);
			metadata.setString("title", txt.picture(index).title);
			metadata.setString("caption", txt.picture(index).caption);
			metadata.setString("credits", txt.picture(index).credit);
			metadata.setString("source", txt.picture(index).source);

			metadata.setString("category", txt.Album.topic_name);
			metadata.setString("album_title", txt.Album.title);
			metadata.setString("album_id", txt.Album.id);
		}
	}

	//------------------------------------
	// Save procedures
	//------------------------------------

	/**
	Saves the JPEG picture to a file

	\param[in] file Complete path to the destination file
	\param[in] format Format in which the file will be saved : JPG or WB1

	\exception Exceptions::FileSystemError Thrown if the file can't be opened for writing.
	*/
	void Picture::saveToFile(string file, Webshots_Formats format) const
	{
		// Open the file
		ofstream out_file(file.c_str(), ios_base::out | ios_base::binary | ios::trunc);

		// If the file is open
		if (out_file.is_open())
		{
			// Write the picture
			if (format == WB1)
			{
				char Header[100];
				int i;
				char KeyVal = 0xA4;

				// Make the encrypted header using the WWBB0000 key
				for (i = 0; i < 100; i++)
					Header[i] = ~(JPGFile[i] ^ KeyVal ^ JPGFile[i + 100]);

				out_file << "WWBB0000";

				out_file.write(Header, 100);
				out_file.write((char *)JPGFile + 100,JPGFile_Size - 100);
			}
			else // format == JPG
				out_file.write((char *)JPGFile,JPGFile_Size);

			out_file.close();
		}
		else
		{
			Exceptions::FileSystemError Err("Unable to open the file \"" + file + "\" for writing.");
			throw Err;
		}
	};

	//------------------------------------
	// Clear procedures
	//------------------------------------

	/**
	Clears the picture
	*/
	void Picture::clear()
	{
		if (JPGFile_Size > 0)
		{
			delete[] JPGFile;
			JPGFile_Size = 0;
		}

		if (BMPThumbnail_Size > 0)
		{
			delete[] BMPThumbnail;
			BMPThumbnail_Size = 0;
		}

		metadata.clear();
	}
}
