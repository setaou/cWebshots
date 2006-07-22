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


#ifndef _CWEBSHOTS_METADATA_H
#define _CWEBSHOTS_METADATA_H

#define _CRT_SECURE_NO_DEPRECATE

#include <vector>
#include <string>

#include "Exceptions.h"
#include "Datum.h"

namespace cWebshots
{
	using namespace std;

	// ----------------------------------------------------------
	//  Class MetaData
	//  ~~~~~~~~~~~~~~~~~~~~~~
	//  Members are defined in cWebshotsMetaData.cpp
	// ----------------------------------------------------------

	/**
	Contains informations
	*/
	class MetaData
	{
	private:
		vector<Datum> data;		///< Array that contains data
		void set(Datum& datum);

	public:
		const Datum get(string key) const;

		void setDate(string key, long value);
		void setInt(string key, int value);
		void setFloat(string key, float value);
		void setString(string key, string value);
		void setBool(string key, bool value);

		void clear(string key);
		void clear();
		bool exists(string key);

		bool operator==(const MetaData& meta) const;

		vector<string> listKeys();
	};
}


#endif
