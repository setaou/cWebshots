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


#ifndef _CWEBSHOTS_DATUM_H
#define _CWEBSHOTS_DATUM_H

#define _CRT_SECURE_NO_DEPRECATE

#include <string>

#include "Exceptions.h"

namespace cWebshots
{
	using namespace std;

	// ----------------------------------------------------------
	//  Class Datum
	//  ~~~~~~~~~~~~~~~~~~~~~~
	//  Members are defined in cWebshotsDatum.cpp
	// ----------------------------------------------------------

	/**
	Contains a piece of information (either a String, an integer, a date or a floating point number)
	*/
	class Datum
	{
	public:
		enum Type {
			type_empty,
			type_integer,
			type_float,
			type_string,
			type_bool,
			type_date
		};

		class KeyType
		{
		public:
			KeyType(string key, Type type)
			{
				this->key = key;
				this->type = type;
			}

			string key;
			Type type;
		};

	private:
		string key;
		Type type;
		void* value;

		void fromPtr(Type type, void* value);
		void clear();

	public:
		Datum(string key);
		Datum(const Datum& source);
		~Datum();

		Datum& operator=(const Datum& source);
		bool operator==(const Datum& dat) const;

		void fromInt(int value);
		void fromFloat(float value);
		void fromString(string value);
		void fromBool(bool value);
		void fromDate(long value);

		string getKey() const;
		Type getType() const;

		string toString() const;
		int toInt() const;
		float toFloat() const;
		bool toBool() const;
		long toDate() const;
	};
}


#endif
