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

#ifndef _CWEBSHOTS_EXCEPTIONS_H
#define _CWEBSHOTS_EXCEPTIONS_H

#define _CRT_SECURE_NO_DEPRECATE

#include <string>

namespace cWebshots
{
	using namespace std;

	//----------------------------------------------------------
	// Exceptions
	//----------------------------------------------------------
	
	/// Contains all exceptions thrown by cWebshots
	namespace Exceptions
	{
		/// Serves as the base for all exceptions thrown by cWebshots
		class Exception
		{
		private:
			string strDescription;

		public:
			Exception()
			{
				strDescription = "Unknown error !";
			};

			Exception(string _Description)
			{
				strDescription = _Description;
			};

			string Description()
			{
				return strDescription;
			};
		};

		/// Exception thrown when an error with a file occurs.
		class FileSystemError : public Exception
		{
		public:
			FileSystemError(string _Description) : Exception(_Description) {};
		};

		/// Exception thrown when there is a problem with a data in a Webshots file.
		class DataError : public Exception
		{
		public:
			DataError(string _Description) : Exception(_Description) {};
		};

		/// Exception thrown when a file format is unknown by cWebshots.
		class UnknownFormat : public DataError
		{
		public:
			UnknownFormat(string _Description) : DataError(_Description) {};
		};

		/// Exception thrown when a corrupted data is found in a Webshots file.
		class CorruptedData : public DataError
		{
		public:
			CorruptedData(string _Description) : DataError(_Description) {};
		};

		/// Exception thrown when a value is out of range.
		class OutOfRange : public Exception
		{
		public:
			OutOfRange(string _Description) : Exception(_Description) {};
		};
	}
}


#endif
