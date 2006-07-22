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
Headers are defined in cWebshots.h
*/

#include "Datum.h"
#include <sstream>

using namespace std;

namespace cWebshots
{
	Datum::~Datum()
	{
		clear();
	}

	Datum::Datum(string key)
	{
		type = type_empty;
		value = NULL;
		this->key = key;
	}

	void Datum::clear()
	{
		if (value != NULL)
		{
			switch (type)
			{
			case type_bool:
				delete (bool *)value;
				break;
			case type_float:
				delete (float *)value;
				break;
			case type_date:
				delete (long *)value;
				break;
			case type_string:
				delete (string *)value;
				break;
			case type_integer:
				delete (int *)value;
				break;
			}
		}
	}

	void Datum::fromPtr(Type type, void* value)
	{
		clear();

		this->type = type;
		this->value = value;
		this->key = key;
	}

	void Datum::fromInt(int value)
	{
		fromPtr(Datum::type_integer, (void*)(new int(value)));
	}
	
	void Datum::fromFloat(float value)
	{
		fromPtr(Datum::type_float, (void*)(new float(value)));
	}
	
	void Datum::fromString(string value)
	{
		fromPtr(Datum::type_string, (void*)(new string(value)));
	}

	void Datum::fromBool(bool value)
	{
		fromPtr(Datum::type_bool, (void*)(new bool(value)));
	}

	void Datum::fromDate(long value)
	{
		fromPtr(Datum::type_date, (void*)(new long(value)));
	}

	Datum::Datum(const Datum& source)
	{
		type = type_empty;
		value = NULL;
		*this = source;
	}

	Datum& Datum::operator=(const Datum& source)
	{
		if (&source != this)
		{
			clear();

			type = source.type;
			key = source.key;

			switch (type)
			{
			case type_bool:
				value = new bool(*((bool *)source.value));
				break;
			case type_float:
				value = new float(*((float *)source.value));
				break;
			case type_date:
				value = new long(*((long *)source.value));
				break;
			case type_string:
				value = new string(*((string *)source.value));
				break;
			case type_integer:
				value = new int(*((int *)source.value));
				break;
			}
		}

		return *this;
	}
		
	string Datum::getKey() const
	{
		return key;
	}

	Datum::Type Datum::getType() const
	{
		return type;
	}

	string Datum::toString() const
	{
		switch (type)
		{
		case type_string:
			return *((string *)value);
		case type_float:
			{
			stringstream s;
			s << *((float *)value);
			return s.str();
			}
		case type_integer:
			{
			stringstream s;
			s << *((int *)value);
			return s.str();
			}
		case type_bool:
			{
			stringstream s;
			s << *((bool *)value);
			return s.str();
			}
		case type_date:
			{
			stringstream s;
			s << *((long *)value);
			return s.str();
			}
		case type_empty:
			return "";
		default:
			throw Exceptions::Exception("Cannot convert to string !");
		}
	}

	float Datum::toFloat() const
	{
		switch (type)
		{
		case type_float:
			return *((float *)value);
		case type_empty:
			return 0;
		default:
			throw Exceptions::Exception("Cannot convert to float !");
		}
	}

	int Datum::toInt() const
	{
		switch (type)
		{
		case type_integer:
			return *((int *)value);
		case type_empty:
			return 0;
		default:
			throw Exceptions::Exception("Cannot convert to integer !");
		}
	}

	bool Datum::toBool() const
	{
		switch (type)
		{
		case type_bool:
			return *((bool *)value);
		case type_empty:
			return false;
		default:
			throw Exceptions::Exception("Cannot convert to bool !");
		}
	}

	long Datum::toDate() const
	{
		switch (type)
		{
		case type_date:
			return *((long *)value);
		case type_empty:
			return 0;
		default:
			throw Exceptions::Exception("Cannot convert to date !");
		}
	}

	bool Datum::operator==(const Datum& dat) const
	{
		if (type == dat.type)
		{
			switch (type)
			{
			case type_string:
				return (*((string *)value) == *((string *)dat.value));
			case type_float:
				return (*((float *)value) == *((float *)dat.value));
			case type_integer:
				return (*((int *)value) == *((int *)dat.value));
			case type_bool:
				return (*((bool *)value) == *((bool *)dat.value));
			case type_date:
				return (*((long *)value) == *((long *)dat.value));
			case type_empty:
				return true;
			}
		}
		else
			return false;
	}
}
