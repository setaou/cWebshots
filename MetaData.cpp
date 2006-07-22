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

#include "MetaData.h"
#include <algorithm>

using namespace std;

namespace cWebshots
{
	const Datum MetaData::get(string key) const
	{
		for(int i = 0; i < data.size(); i++)
		{
			if (data.at(i).getKey() == key)
				return data.at(i);
		}

		return Datum(key);
		//throw Exceptions::Exception("Metadata not found");
	}
	
	void MetaData::set(Datum& datum)
	{
		clear(datum.getKey());
		data.push_back(datum);
	}

	void MetaData::setInt(string key, int value)
	{
		Datum d(key);
		d.fromInt(value);
		set(d);
	}
	
	void MetaData::setFloat(string key, float value)
	{
		Datum d(key);
		d.fromFloat(value);
		set(d);
	}
	
	void MetaData::setString(string key, string value)
	{
		Datum d(key);
		d.fromString(value);
		set(d);
	}

	void MetaData::setBool(string key, bool value)
	{
		Datum d(key);
		d.fromBool(value);
		set(d);
	}

	void MetaData::setDate(string key, long value)
	{
		Datum d(key);
		d.fromDate(value);
		set(d);
	}

	void MetaData::clear(string key)
	{
		for(int i = 0; i < data.size(); i++)
		{
			if (data.at(i).getKey() == key)
				data.erase(data.begin() + i );
		}
	}
	
	void MetaData::clear()
	{
		data.clear();
	}

	bool MetaData::exists(string key)
	{
		for(int i = 0; i < data.size(); i++)
		{
			if (data.at(i).getKey() == key)
				return true;
		}

		return false;
	}

	vector<string> MetaData::listKeys()
	{
		vector<string> ret;

		for(int i = 0; i < data.size(); i++)
		{
			ret.push_back(data.at(i).getKey());
		}

		sort(ret.begin(), ret.end());
		return ret;
	}

	bool MetaData::operator==(const MetaData& meta) const
	{
		if (data.size() != meta.data.size())
			return false;

		for(int i = 0; i < data.size(); i++)
		{
			if (!(data.at(i) == meta.get(data.at(i).getKey())))
				return false;
		}

		return true;
	}

}
