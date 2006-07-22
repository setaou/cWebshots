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


#ifndef _CWEBSHOTS_COLLECTION_H
#define _CWEBSHOTS_COLLECTION_H

#define _CRT_SECURE_NO_DEPRECATE

#include <vector>
#include <string>
#include <fstream>

#include "Picture.h"
#include "Exceptions.h"
#include "MetaData.h"
#include "TXT.h"
#include "Misc.h"

namespace cWebshots
{
	using namespace std;

	// ----------------------------------------------------------
	//  Class Collection
	//  ~~~~~~~~~~~~~~~~~~~~~~
	//  Members are defined in cWebshotsCollection.cpp
	// ----------------------------------------------------------

	/**
	Contains a collection of cWebshotsPicture and some informations about the collection
	*/
	class Collection
	{
	private:
		vector<Picture*> Pictures;		///< Array that contains pointers to the pictures
		MetaData metadata;

		void loadWBCFile(ifstream &in_file);
		void loadWBZFile(ifstream &in_file);

		void saveWBCFile(ofstream &out_file) const;
		void saveWBZFile(ofstream &out_file) const;

	public:
		~Collection();
		Collection();
		Collection(const Collection& source);
		Collection(string File);

		Collection& operator=(const Collection& source);
		Collection& operator+=(const Collection& source);
		Collection& operator=(const Picture& source);
		Collection& operator+=(const Picture& source);

		friend Collection operator+(const Collection& col1, const Collection& col2);
		friend Collection operator+(const Picture& pic, const Collection& col);
		friend Collection operator+(const Collection& col, const Picture& pic);
		friend Collection operator+(const Picture& pic1, const Picture& pic2);

		MetaData& metaData();
		const MetaData& metaData() const;
		void setMetaData(MetaData& metadata);
		static vector<Datum::KeyType> availableKeys();

		void loadFromFile(const string File);
		void loadFromFolder(const string Path);

		void saveToFile(const string File, Webshots_Formats Format) const;
		void saveToFolder(const string Path) const;

		Picture& operator[](const unsigned int Index);
		const Picture& operator[](const unsigned int Index) const;
		Picture& picture(unsigned int Index);
		const Picture& picture(unsigned int Index) const;

		Picture& addPicture();
		Picture& addPicture(const Picture& Picture);

		void removePicture(const unsigned int Index);
		void clear();

		unsigned int pictureCount() const;
	};

	//----------------------------------------------------------
	// Structures
	//----------------------------------------------------------

	/// WBC header structure
	struct sWBC_Header
	{
		unsigned long File_Marker;		///< WBC File marker, must be AB16FA95
		unsigned long Header_Size;		///< Header size
		long unknown;					///< Unknown usage
		char File_Title[256];			///< File title (max 256 char)
	};

	/// WBC index structure
	struct sWBC_Index
	{
		unsigned long Unit_Position;	///< Position of the unit in the file
		unsigned long Unit_Size;		///< Unit size
		long unknown;					///< Unknown usage
		long Addition_TimeStamp;		///< Date when the unit was added to the collection. Unix timestamp format.
		char unknown2[24];				///< Unknown usage
	};

	/// WBC unit header structure
	struct sWBC_Unit_Header
	{
		unsigned long Unit_Marker;		///< Unit marker, must be E2CD71F0
		unsigned long Header_Size;		///< Unit Header size
		unsigned long Unit_Size;		///< Unit Size (including the header)
		char Original_Filename[256];	///< Original filename of the picture
		char Title[128];				///< Picture title
		char Description[256];			///< Picture description
		char Credits[256];				///< Picture credits
		char Extension[8];				///< Picture extension (.wb1, .wbd, .jpg ...)
		unsigned long JPG_File_Size;		///< JPG file size
		unsigned long BMP_Thumbnail_Size;	///< BMP thumbnail size
		char Unknown[140];				///< Unknown usage
		unsigned long Daily_Date;		///< Date when the picture was published as a Daily picture on the Webshots website. In the format yyyymmdd
		long Addition_Timestamp;		///< Date when the picture was added to the collection. Unix timestamp format.
		unsigned long Fit_To_Screen;	///< Fit to screen behaviour (0=default, 1=never, 2=always)
		char ID[128];					///< Picture ID
		char Album[96];					///< Album
		char Category[256];				///< Category
		char Unknown2[532];				///< Unknown usage
	};

	/// WBZ file header structure
	struct sWBZ_Header
	{
		unsigned long File_Marker;		///< File marker, must be 43AB9167
		unsigned long Header_Size;		///< Header size
		long unknown1;					///< Unknown usage
		unsigned long Header_Version;	///< Header Version (1, on March 2005)
		long unknown3;					///< Unknown usage
		unsigned long Element_Count;	///< Number of elements in the WBZ file
		char unknown4[128];				///< Unknown usage
	};

	/// WBZ element header structure
	struct sWBZ_Element_Header
	{
		unsigned long Element_Marker;	///< Element marker, must be E1CD8210
		unsigned long Header_Size;		///< Header size
		unsigned long Header_Version;	///< Header Version (1, on March 2005)
		char File_Name[256];			///< File name (max 256 char)
		unsigned long File_Size;		///< File size
		long unknown2;					///< Unknown usage
		unsigned long File_Size2;		///< File size
		char unknown4[256];				///< Unknown usage
	};
}

#endif