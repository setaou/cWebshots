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


#ifndef _CWEBSHOTS_TXT_H
#define _CWEBSHOTS_TXT_H

#define _CRT_SECURE_NO_DEPRECATE

#include <string>

#include "Exceptions.h"

namespace cWebshots
{
	using namespace std;

	// ----------------------------------------------------------
	//  Class TXT
	//  ~~~~~~~~~~~~~~~~~~~~~~
	//  Members are defined in cWebshotsTXT.cpp
	// ----------------------------------------------------------

	/// Contains informations about a picture
	struct sPhotosTXT
	{
		string id;					///< Picture ID
		string position;			///< Picture position (seems to be the position in the file, but it is still very mysterious !)
		string title;				///< Picture title
		string caption;				///< Picture caption
		string credit;				///< Picture Credit
		string url_photo;			///< FileName of the picture
		string url_thumb;			///< FileName of the thumbnail
		string source;				///< Picture source (W = webshots)
	};

	/// Contains informations about a collection
	struct sAlbumTXT
	{
		string id;					///< Collection ID
		string title;				///< Collection title
		int photo_count;			///< Number of pictures in the collection
		bool in_use;				///< True if the collection is in use, False otherwise
		string description;			///< Description of the collection
		string topic_name;			///< Topic of the collection
		long create_date;			///< Creation date of the collection.
		long edit_date;				///< Last date when the collection was edited.
	};

	/**
	Contains informations about a collection and its pictures, read from photos.txt / album.txt files
	*/
	class TXT
	{
	private:
		sPhotosTXT *Pictures;		///< Pointer to an array of sPhotosTXT sturctures which contains informations about pictures
		unsigned int Count;			///< Number of picture information records

	public:
		TXT(void);
		TXT(string directory);
		~TXT(void);

		void load(string directory);

		sPhotosTXT& picture(int index);
		unsigned int pictureCount();

		int indexFromFileName(string filename, bool case_sensitive = false);

		sAlbumTXT Album;						///< sAlbumTXT structure which contains informations about the collection

		static const int IndexNotFound = -1;	///< Returned by indexFromFileName when the index is not found
	};

}

#endif