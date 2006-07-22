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


#ifndef _CWEBSHOTS_PICTURE_H
#define _CWEBSHOTS_PICTURE_H

#define _CRT_SECURE_NO_DEPRECATE

#include <vector>
#include <string>
#include <fstream>

#include "Exceptions.h"
#include "MetaData.h"
#include "TXT.h"
#include "Misc.h"

namespace cWebshots
{
	using namespace std;

	// ----------------------------------------------------------
	//  Class Picture          
	//  ~~~~~~~~~~~~~~~~~~~~~~
	//  Members are defined in cWebshotsPicture.cpp
	// ----------------------------------------------------------

	/**
	Contains a webshots picture and its meta informations
	*/
	class Picture
	{
	private:
		unsigned char *JPGFile;				///< Pointer to an array containing the picture (if any)
		unsigned long JPGFile_Size;			///< Size of the picture
		unsigned char *BMPThumbnail;		///< Pointer to an array containing the thumbnail (if any)
		unsigned long BMPThumbnail_Size;	///< Size of the thumbnail
		MetaData metadata;

		void loadWB1(ifstream &in_file);

		void loadMetaDataTXT(string file);

	public:
		~Picture();
		Picture();
		Picture(const Picture& source);
		Picture(string File);

		Picture& operator=(const Picture& source);

		void clear();

		void loadFromFile(string File);
		void saveToFile(string file, Webshots_Formats format = JPG) const;

		unsigned char *getPicturePtr() const;
		unsigned long getPictureSize() const;
		void setPicture(const unsigned char *data, const unsigned long size);

		unsigned char *getThumbnailPtr() const;
		unsigned long getThumbnailSize() const;
		void setThumbnail(const unsigned char *data, const unsigned long size);

		MetaData& metaData();
		const MetaData& metaData() const;
		void setMetaData(MetaData& metadata);
		static vector<Datum::KeyType> availableKeys();
	};
}

#endif