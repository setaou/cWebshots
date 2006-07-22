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
Definition of cWebshotsCollection members.
Headers are defined in cWebshots.h
*/

#include "Collection.h"

#include <cstdio>
#include <sstream>

using namespace std;

namespace cWebshots
{

	
	//------------------------------------
	// Constructors / Destructors
	//------------------------------------

	/** 
	Constructor
	*/
	Collection::Collection()
	{
	}

	/** 
	Destructor
	*/
	Collection::~Collection()
	{
		// Delete every picture
		for (int i = 0; i < Pictures.size(); i++)
			delete Pictures[i];
	}
	
	/** 
	Constructor

	\param[in] source Source cWebshotsCollection
	*/
	Collection::Collection(const cWebshots::Collection &source)
	{
		*this = source;
	}

	/** 
	Constructor

	\param[in] File File to load
	*/
	Collection::Collection(string File)
	{
		loadFromFile(File);
	}
	
	//------------------------------------
	// File open /save
	//------------------------------------
	
	/**
	Clear the collection and load it from a file

	\param[in] File Complete path to the file to load

	\exception Exceptions::FileSystemError Thrown if the file cannot be opened for reading
	\exception Exceptions::UnknownFormat Thrown if the format of the file is unknown
	*/
	void Collection::loadFromFile(string File)
	{
		ifstream in_file;

		// Clear the collection
		clear();

		// Open the file for reading
		in_file.open(File.c_str(), ios_base::in | ios_base::binary);

		if (in_file.is_open())
		{
			// Depending on the type of the file
			switch (identifyFile(in_file))
			{
			case WBZ: // WBZ file
				loadWBZFile(in_file);
				break;
			case WBC: // WBC File
				loadWBCFile(in_file);
				break;
			case WB1: // encrypted WB1 file
			case JPG: // JPG or plain WB1 file
				addPicture().loadFromFile(File);
				break;
			default: // Unknown file type
				{
					Exceptions::UnknownFormat Err("The file \"" + File + "\" has an unknown format.");
					throw Err;
				}
			}

			// Close the file
			in_file.close();
		}
		else
		{
			Exceptions::FileSystemError Err("Unable to open the file \"" + File + "\" for reading.");
			throw Err;
		}
	}

	/**
	Loads a WBC file

	\param[in] in_file Reference to the stream of the file to load

	\exception Exceptions::UnknownFormat Thrown if the file has an unknown format
	*/
	void Collection::loadWBCFile(ifstream &in_file)
	{
		long Count;
		int i;

		sWBC_Header Header;					// File header
		sWBC_Index* Index;					// File Index
		sWBC_Unit_Header Unit_Header;		// Unit header

		unsigned char* Buffer;
		unsigned char* Buffer2;

		Picture* Pic;

		// Read the header of the file
		in_file.seekg(0);
		in_file.read((char *)&Header, sizeof Header);

		// If the header is wrong
		if (Header.File_Marker != WBC_File_Marker)
		{
			Exceptions::UnknownFormat Err("The file has an unknown format.");
			throw Err;
		}

		metadata.setString("title", Header.File_Title);
		//Title = Header.File_Title;

		// Read the index
		in_file.seekg(2196);
		in_file.read((char *)&Count, sizeof Count);

		Index = new sWBC_Index[Count];
		in_file.read((char *)Index, Count * sizeof *Index);

		// for each unit in the file
		for (i = 0; i < Count; i++)
		{
			// read the unit header
			in_file.seekg(Index[i].Unit_Position);
			in_file.read((char *)&Unit_Header, sizeof Unit_Header);
			
			// read the jpg file
			Buffer = new unsigned char[Unit_Header.JPG_File_Size];
			in_file.read((char *)Buffer, Unit_Header.JPG_File_Size);

			// read the thumbnail
			Buffer2 = new unsigned char[Unit_Header.BMP_Thumbnail_Size];
			in_file.read((char *)Buffer2, Unit_Header.BMP_Thumbnail_Size);

			// load the picture and add it to the collection
			try
			{
				Pic = new Picture;

				Pic->setPicture(Buffer, Unit_Header.JPG_File_Size);
				Pic->setThumbnail(Buffer2, Unit_Header.BMP_Thumbnail_Size);

				Pic->metaData().setDate("addition_date", Unit_Header.Addition_Timestamp);
				Pic->metaData().setString("caption", Unit_Header.Description);
				Pic->metaData().setString("category", Unit_Header.Category);
				Pic->metaData().setString("album_title", Unit_Header.Album);
				Pic->metaData().setString("credits", Unit_Header.Credits);
				Pic->metaData().setDate("daily_date", dateYYYYMMDD(Unit_Header.Daily_Date));
				if (strlen(Unit_Header.ID) > 1)
					Pic->metaData().setString("id", &Unit_Header.ID[1]);
				if (strlen(Unit_Header.ID) > 0)
					Pic->metaData().setString("source", string(Unit_Header.ID).substr(0, 1));
				Pic->metaData().setString("original_filename", Unit_Header.Original_Filename);
				Pic->metaData().setString("title", Unit_Header.Title);

				Pictures.push_back(Pic);
			}
			// If the picture has not been loaded correctly, skip it !
			// This little hack allows to open corrupted files.
			catch (Exceptions::UnknownFormat)
			{
				delete Pic;
			}
			
			delete[] Buffer;
			delete[] Buffer2;
		}

		delete[] Index;
	}

	/**
	Loads a WBZ file

	\param[in] in_file Reference to the stream of the file to load

	\exception Exceptions::UnknownFormat Thrown if the file has an unknown format
	*/
	void Collection::loadWBZFile(ifstream &in_file)
	{
		sWBZ_Header File_Header;
		sWBZ_Element_Header Element_Header;
		unsigned int i;
		unsigned char *ElementContent;
		string Metadata;

		// Read the header
		in_file.seekg(0);
		in_file.read((char *)&File_Header,sizeof File_Header);

		// Bad file format
		if (File_Header.File_Marker != WBZ_File_Marker)
		{
			Exceptions::UnknownFormat Err("The file has an unknown format.");
			throw Err;
		}

		// For each element
		for (i = 0; i < File_Header.Element_Count; i++)
		{
			// Read the header
			in_file.read((char *)&Element_Header,sizeof Element_Header);

			// Corrupted header
			if (Element_Header.Element_Marker != WBZ_Element_Marker)
			{
				stringstream Description;
				Description << "Corrupted data : The header of the element n°" << (i + 1) << " is incorrect.";
				Exceptions::CorruptedData Err(Description.str());
				throw Err;
			}

			// Read the element
			ElementContent = new unsigned char[Element_Header.File_Size];

			try
			{
				in_file.read((char *)ElementContent, Element_Header.File_Size);

				// If the element is a picture
				if (strstr(Element_Header.File_Name,"wbx") == NULL)
				{
					addPicture().setPicture(ElementContent,Element_Header.File_Size);
				}
				// If the element is the metadata file
				else
				{
					Metadata.assign((char *)ElementContent);
				}
			}
			// If an exception is thrown, delete the element content and re-throw the exception
			catch (...)
			{
				delete[] ElementContent;
				throw;
			}

			delete[] ElementContent;
		}

		// Once all elements are loaded, get the metadata for each picture in the collection
		for (i = 0; i < pictureCount(); i++)
		{
			try
			{
				Pictures[i]->metaData().setString("category", getINI(Metadata, longToStr(i), "catTitle"));
				Pictures[i]->metaData().setString("credits", getINI(Metadata, longToStr(i), "credit"));
				Pictures[i]->metaData().setDate("daily_date", dateYYYYMMDD(strToLong(getINI(Metadata, longToStr(i), "dailyDateYYYYMMDD"))));
				Pictures[i]->metaData().setString("id", getINI(Metadata, longToStr(i), "id"));
				Pictures[i]->metaData().setString("source", getINI(Metadata, longToStr(i), "source"));
				Pictures[i]->metaData().setString("original_filename", getINI(Metadata, longToStr(i), "filename"));
				Pictures[i]->metaData().setString("title", getINI(Metadata, longToStr(i), "title"));
				Pictures[i]->metaData().setString("album_title", getINI(Metadata, longToStr(i), "albumTitle"));
				Pictures[i]->metaData().setString("album_id", getINI(Metadata, longToStr(i), "albumID"));
			}
			// If there are missing metadata
			catch (Exceptions::CorruptedData)
			{
				// Do nothing
			}
		}
	}

	/**
	Loads a collection from a folder which contains photos.txt, album.txt, and WB1 files

	\param[in] Path Path to the collection to load

	\todo Handle thumbnails
	*/
	void Collection::loadFromFolder(const string Path)
	{
		TXT album;
		Picture pic;
		unsigned int i;

		// Clear the collection
		clear();

		// Load the photos.txt/album.txt files
		album.load(Path);

		metadata.setDate("create_date", album.Album.create_date);
		metadata.setDate("edit_date", album.Album.edit_date);
		metadata.setString("description", album.Album.description);
		metadata.setString("id", album.Album.id);
		metadata.setBool("in_use", album.Album.in_use);
		metadata.setString("title", album.Album.title);
		metadata.setString("topic", album.Album.topic_name);

		// For each picture in the album
		for (i = 0; i < album.pictureCount(); i++)
		{
			try
			{
				pic.clear();

				// Load the picture
				pic.loadFromFile(Path + album.picture(i).url_photo);

				// Load the metadata
				pic.metaData().setString("credits", album.picture(i).credit);
				pic.metaData().setString("id", album.picture(i).id);
				pic.metaData().setString("source", album.picture(i).source);
				pic.metaData().setString("original_filename", album.picture(i).url_photo);
				pic.metaData().setString("title", album.picture(i).title);
				pic.metaData().setString("caption", album.picture(i).caption);

				// Add the picture to the collection
				addPicture(pic);
			}
			// If a picture does not load correctly 
			catch (Exceptions::Exception)
			{
				// Do nothing
			}
		}
	}


	/**
	Saves the collection into a file.

	\param[in] File Complete path to the file
	\param[in] Format Format in which to save the file (WBC / WBZ)

	\exception Exceptions::FileSystemError Thrown if the file cannot be opened for writing
	*/
	void Collection::saveToFile(string File, Webshots_Formats Format) const
	{
		// Open the file for writing
		ofstream out_file(File.c_str(), ios_base::out | ios_base::binary | ios::trunc);

		// If the file is open
		if (out_file.is_open())
		{
			// Save the collection in the right format
			if (Format == WBC)
				saveWBCFile(out_file);
			else if (Format == WBZ)
				saveWBZFile(out_file);

			out_file.close();
		}
		else
		{
			Exceptions::FileSystemError Err("Unable to open the file \"" + File + "\" for writing.");
			throw Err;
		}
	}

	/**
	Saves the collection into a WBC file.

	\param[in] out_file Stream of the file to write to
	*/
	void Collection::saveWBCFile(ofstream &out_file) const
	{
		// File header
		sWBC_Header file_header;

		file_header.File_Marker = WBC_File_Marker;
		strcpy(file_header.File_Title, metadata.get("title").toString().c_str());
		file_header.Header_Size = 8608;
		file_header.unknown = 0;

		out_file.seekp(0);
		out_file.write((char *)&file_header, sizeof file_header);

		unsigned int i;

		// Units
		out_file.seekp(8608);

		sWBC_Unit_Header unit_header;

		for (i = 0; i < pictureCount(); i++)
		{
			unit_header.Unit_Marker = WBC_Unit_Marker;
			unit_header.Header_Size = sizeof unit_header;
			unit_header.Unit_Size = unit_header.Header_Size + Pictures[i]->getPictureSize() + Pictures[i]->getThumbnailSize();

			strcpy(unit_header.Original_Filename, Pictures[i]->metaData().get("original_filename").toString().c_str());
			strcpy(unit_header.Title, Pictures[i]->metaData().get("title").toString().c_str());
			strcpy(unit_header.Description, Pictures[i]->metaData().get("caption").toString().c_str());
			strcpy(unit_header.Credits, Pictures[i]->metaData().get("credits").toString().c_str());
			unit_header.Extension[0] = 0; // Empty string
			unit_header.JPG_File_Size = Pictures[i]->getPictureSize();
			unit_header.BMP_Thumbnail_Size = Pictures[i]->getThumbnailSize();
			unit_header.Unknown[0] = 0; // Empry string
			unit_header.Daily_Date = Pictures[i]->metaData().get("daily_date").toDate();
			unit_header.Addition_Timestamp = Pictures[i]->metaData().get("addition_date").toDate();
			unit_header.Fit_To_Screen = 0; // Default

			stringstream picID;
			picID << Pictures[i]->metaData().get("source").toString();
			picID << Pictures[i]->metaData().get("id").toString();
			strcpy(unit_header.ID, picID.str().c_str());

			strcpy(unit_header.Album, Pictures[i]->metaData().get("album_title").toString().c_str());
			strcpy(unit_header.Category, Pictures[i]->metaData().get("category").toString().c_str());
			unit_header.Unknown2[0] = 0;

			out_file.write((char *)&unit_header, sizeof unit_header);

			if (Pictures[i]->getPictureSize() > 0)
				out_file.write((char *)Pictures[i]->getPicturePtr(), Pictures[i]->getPictureSize());
			if (Pictures[i]->getThumbnailSize() > 0)
				out_file.write((char *)Pictures[i]->getThumbnailPtr(), Pictures[i]->getThumbnailSize());
		}

		// Index
		out_file.seekp(2196);

		sWBC_Index index;
		long offset;
		long count;

		count = pictureCount();

		out_file.write((char *)&count, sizeof count);

		offset = 8608;

		for (i = 0; i < pictureCount(); i++)
		{
			index.Addition_TimeStamp = Pictures[i]->metaData().get("addition_date").toDate();
			index.Unit_Position = offset;
			index.Unit_Size = sizeof unit_header + Pictures[i]->getPictureSize() + Pictures[i]->getThumbnailSize();
			index.unknown = 0;
			index.unknown2[0] = 0;

			out_file.write((char *)&index, sizeof index);

			offset += index.Unit_Size;
		}
	}


	/**
	Saves the collection into a WBZ file.

	\param[in] out_file Stream of the file to write to
	*/
	void Collection::saveWBZFile(ofstream &out_file) const
	{
		// File header
		sWBZ_Header header;

		header.Element_Count = pictureCount() + 1;
		header.File_Marker = WBZ_File_Marker;
		header.Header_Size = sizeof header;
		header.unknown1 = 0;
		header.Header_Version = 1;
		header.unknown3 = 0;
		header.unknown4[0] = 0; 

		out_file.write((char *)&header,sizeof header);

		// Pictures
		unsigned int i;

		for (i = 0; i < pictureCount(); i++)
		{
			sWBZ_Element_Header pic_header;

			pic_header.Element_Marker = WBZ_Element_Marker;
			strcpy(pic_header.File_Name, picture(i).metaData().get("original_filename").toString().c_str());
			pic_header.File_Size = picture(i).getPictureSize();
			pic_header.File_Size2 = picture(i).getPictureSize();
			pic_header.Header_Size = sizeof pic_header;
			pic_header.Header_Version = 1;
			pic_header.unknown2 = 0;
			pic_header.unknown4[0] = 0;

			out_file.write((char *)&pic_header,sizeof pic_header);
			out_file.write((char *)picture(i).getPicturePtr(),picture(i).getPictureSize());
		}

		// Last element : metadata
		stringstream meta_stream;
		string meta;

		meta_stream << "[Header]" << "\r\n";
		meta_stream << "Count=" << pictureCount() << "\r\n";

		// The following two lines are for compatibility with the old WBX format
		for (i = 0; i < pictureCount(); i++)
			meta_stream << i << "=100" << "\r\n";

		for (i = 0; i < pictureCount(); i++)
		{
			meta_stream << "[" << i << "]" << "\r\n";
			meta_stream << "BOO_MakeWallpaper=1\r\n";
			meta_stream << "FIL_ImageFileName=" << picture(i).metaData().get("original_filename").toString() << "\r\n";
			meta_stream << "STR_ImageTitle=" << picture(i).metaData().get("title").toString() << "\r\n";
			meta_stream << "STR_ImageCredit=" << picture(i).metaData().get("credits").toString() << "\r\n";
			meta_stream << "STR_ImagePID=" << picture(i).metaData().get("source").toString() << picture(i).metaData().get("id").toString() << "\r\n";
			meta_stream << "STR_CollectionTitle=" << picture(i).metaData().get("category").toString() << " - " << picture(i).metaData().get("album_title").toString() << "\r\n";
			meta_stream << "source=" << picture(i).metaData().get("source").toString() << "\r\n";
			meta_stream << "filename=" << picture(i).metaData().get("original_filename").toString() << "\r\n";
			meta_stream << "id=" << picture(i).metaData().get("id").toString() << "\r\n";
			meta_stream << "title=" << picture(i).metaData().get("title").toString() << "\r\n";
			meta_stream << "credit=" << picture(i).metaData().get("credits").toString() << "\r\n";
			meta_stream << "position=\r\n";
			meta_stream << "catTitle=" << picture(i).metaData().get("category").toString() << "\r\n";
			meta_stream << "albumID=" << picture(i).metaData().get("album_id").toString() << "\r\n";
			meta_stream << "albumTitle=" << picture(i).metaData().get("album_title").toString() << "\r\n";
			meta_stream << "dailyDateYYYYMMDD=" << picture(i).metaData().get("daily_date").toDate() << "\r\n";
		}

		meta = meta_stream.str();

		sWBZ_Element_Header meta_header;

		meta_header.Element_Marker = WBZ_Element_Marker;
		strcpy(meta_header.File_Name,"info.wbx");
		meta_header.File_Size = (long)meta.length();
		meta_header.File_Size2 = (long)meta.length();
		meta_header.Header_Size = sizeof meta_header;
		meta_header.Header_Version = 1;
		meta_header.unknown2 = 0;
		meta_header.unknown4[0] = 0;

		out_file.write((char *)&meta_header,sizeof meta_header);
		out_file.write(meta.c_str(),meta.length());
	}


	/**
	Saves the collection into a folder (each picture in a separate WB1 file + photos.txt + album.txt)

	\param[in] Path Complete path to the folder

	\exception Exceptions::FileSystemError Thrown if a file cannot be opened for writing

	\todo Handle thumbnails
	*/
	void Collection::saveToFolder(const string Path) const
	{
		ofstream out_file;
		string filename;
		stringstream filename_stream;
		unsigned int i;

		// Create album.txt
		filename = Path + "album.txt";
		out_file.open(filename.c_str(),  ios_base::out | ios::trunc);

		if (out_file.is_open())
		{
			out_file << "count||1" << endl
				<< "ID_0||" << metadata.get("id").toString() << endl
				<< "TITLE_0||" << metadata.get("title").toString() << endl
				<< "PHOTO_COUNT_0||" << pictureCount() << endl
				<< "IN_USE_0||" << (metadata.get("in_use").toBool() ? 1 : 0) << endl
				<< "DESCRIPTION_0||" << metadata.get("description").toString() << endl
				<< "TOPIC_NAME_0||" << metadata.get("topic").toString() << endl
				<< "CREATE_DATE_0||" << metadata.get("create_date").toDate() << endl
				<< "EDIT_DATE_0||" << metadata.get("edit_date").toDate() << endl;

			out_file.close();
		}
		else
		{
			Exceptions::FileSystemError Err("Unable to open the file \"" + filename + "\" for writing.");
			throw Err;
		}

		// Create photos.txt & save each picture
		filename = Path + "photos.txt";
		out_file.open(filename.c_str(),  ios_base::out | ios::trunc);

		if (out_file.is_open())
		{
			out_file << "count||" << pictureCount() << endl;

			for (i = 0; i < pictureCount(); i++)
			{
				// Save the picture
				filename_stream << Path << "ph" <<  picture(i).metaData().get("id").toString() << ".wb1"; 
				picture(i).saveToFile(filename_stream.str());
				filename_stream.str("");

				out_file << "ID_" << i << "||" << picture(i).metaData().get("id").toString() << endl
					<< "POSITION_" << i << "||" << picture(i).metaData().get("id").toString() << endl
					<< "TITLE_" << i << "||" << picture(i).metaData().get("title").toString() << endl
					<< "CAPTION_" << i << "||" << picture(i).metaData().get("caption").toString() << endl
					<< "CREDIT_" << i << "||" << picture(i).metaData().get("credits").toString() << endl
					<< "URL_PHOTO_" << i << "||" << "ph" << picture(i).metaData().get("id").toString() << ".wb1" << endl
					<< "URL_THUMB_" << i << "||" << "ph" << picture(i).metaData().get("id").toString() << ".wb1" << endl // TODO : implement a real thumbnail system  :)
					<< "SOURCE_" << i << "||" << (picture(i).metaData().get("source").toString().length() > 0 ? picture(i).metaData().get("source").toString() : "u") << endl;
			}

			out_file.close();
		}
		else
		{
			Exceptions::FileSystemError Err("Unable to open the file \"" + filename + "\" for writing.");
			throw Err;
		}
	}

	//------------------------------------
	// Access / add / remove / count pictures
	//------------------------------------

	/**
	Returns a reference to the picture number <i>Index</i> in the collection.

	\return A reference to the picture number <i>Index</i> in the collection.

	\exception Exceptions::OutOfRange Thrown if Index is greater than the number of pictures in the collection
	*/
	Picture& Collection::picture(unsigned int Index)
	{
		if (Index < (unsigned int)Pictures.size())
			return *Pictures[Index];
		else
		{
			Exceptions::OutOfRange Err("Index out of range.");
			throw Err;
		}
	}

	/**
	Returns a reference to the picture number <i>Index</i> in the collection. Used if the collection is constant.

	\return A reference to the constant picture number <i>Index</i> in the collection.

	\exception Exceptions::OutOfRange Thrown if Index is greater than the number of pictures in the collection
	*/
	const Picture& Collection::picture(unsigned int Index) const
	{
		if (Index < (unsigned int)Pictures.size())
			return *Pictures[Index];
		else
		{
			Exceptions::OutOfRange Err("Index out of range.");
			throw Err;
		}
	}

	/**
	Removes the picture number <i>Index</i> from the collection.

	\param[in] Index Number of the picture to remove.

	\exception Exceptions::OutOfRange Thrown if Index is greater than the number of pictures in the collection
	*/
	void Collection::removePicture(unsigned int Index)
	{
		if (Index < (unsigned int)Pictures.size())
		{
			delete Pictures[Index];
			Pictures.erase(Pictures.begin() + Index);
		}
		else
		{
			Exceptions::OutOfRange Err("Index out of range.");
			throw Err;
		}
	}

	/**
	Adds a blank picture to the collection.

	\return A reference to the picture in the collection
	*/
	Picture& Collection::addPicture()
	{
		Picture* Pic = new Picture;
		Pictures.push_back(Pic);

		return *Pic;
	}

	/**
	Adds a picture to the collection.

	\param[in] Picture Reference to the picture to add to the collection

	\return A reference to the picture in the collection
	*/
	Picture& Collection::addPicture(const Picture& Picture)
	{
		return addPicture() = Picture;
	}

	/**
	Returns the number of pictures in the collection.

	\return The number of pictures in the collection
	*/
	unsigned int Collection::pictureCount() const
	{
		return (int)Pictures.size();
	}

	/**
	Removes all the pictures in the collection and clear the properties
	*/
	void Collection::clear()
	{
		unsigned long i;

		// Delete every picture
		for (i = 0; i < Pictures.size(); i++)
			delete Pictures[i];

		// Clear the array
		Pictures.clear();

		// Clear the properties
		metadata.clear();
	}

	// -----------------------------------------
	// Operators
	// -----------------------------------------

	/**
	Operator = (cWebshotsCollection).
	Clears the collection and adds the pictures contained in <i>source</i> to it.
	*/
	Collection& Collection::operator=(const Collection& source)
	{
		if (&source != this)
		{
			unsigned int i;

			// Clear the collection
			clear();

			// Add the pictures of "source" to it
			for (i = 0; i < source.pictureCount(); i++)
				addPicture(source.picture(i));

			// Copy the properties from "source"
			metadata= source.metadata;

			//ID = source.ID;
			//Title = source.Title;
			//Description = source.Description;
			//Topic =source.Topic;
			//In_Use = source.In_Use;
			//CreateDate = source.CreateDate;
			//EditDate = source.EditDate;
		}

		return *this;
	}

	/**
	Operator += (cWebshotsCollection).
	Adds the pictures contained in <i>source</i> to the collection.
	*/
	Collection& Collection::operator+=(const Collection& source)
	{
		unsigned int i;

		for (i = 0; i < source.pictureCount(); i++)
			addPicture(source.picture(i));

		return *this;
	}

	/**
	Operator = (cWebshotsPicture).
	Clears the collection and adds the picture <i>source</i> to it.
	*/
	Collection& Collection::operator=(const Picture& source)
	{
		clear();
		addPicture(source);

		return *this;
	}

	/**
	Operator += (cWebshotsPicture).
	Adds the picture <i>source</i> to the collection.
	*/
	Collection& Collection::operator+=(const Picture& source)
	{
		addPicture(source);

		return *this;
	}

	/**
	Operator []
	Allow to access a picture contained in the collection with <b>myCollection[<i>Index</i>]</b> instead of <b>myCollection.picture(<i>Index</i>)</b>

	\return A reference to the picture number <i>Index</i>

	\exception Exceptions::OutOfRange Thrown if Index is greater than the number of pictures in the collection or lower than 0
	*/
	Picture& Collection::operator[](const unsigned int Index)
	{
		if (Index < (unsigned int)Pictures.size())
			return *Pictures[Index];
		else
		{
			Exceptions::OutOfRange Err("Index out of range.");
			throw Err;
		}
	}

	/**
	Operator []. Used if the collection is constant.
	Allow to access a picture contained in the collection with <b>myCollection[<i>Index</i>]</b> instead of <b>myCollection.picture(<i>Index</i>)</b>

	\return A reference to the constant picture number <i>Index</i>

	\exception Exceptions::OutOfRange Thrown if Index is greater than the number of pictures in the collection or lower than 0
	*/
	const Picture& Collection::operator[](const unsigned int Index) const
	{
		if (Index < (unsigned int)Pictures.size())
			return *Pictures[Index];
		else
		{
			Exceptions::OutOfRange Err("Index out of range.");
			throw Err;
		}
	}


	/**
	Operator +, to merge two collections together

	\return A collection which contains the pictures of both source collections and the properties of the first source collection.
	\return For example : A + B returns a collections withs pictures from A and B, and properties from A.
	*/
	Collection operator+(const Collection& col1, const Collection& col2)
	{
		Collection Collection;

		Collection = col1;
		Collection += col2;

		return Collection;
	}

	/**
	Operator +, to merge a picture (left) and a collection (right)

	\return A collection which contains the picture from the source picture plus the pictures and the properties of the source collection.
	*/
	Collection operator+(const Picture& pic, const Collection& col)
	{
		Collection Collection;

		Collection = pic;
		Collection += col;

		return Collection;
	}

	/**
	Operator +, to merge a collection (left) and a picture (right)

	\return A collection which contains the pictures and the properties of the source collection plus the picture from the source picture.
	*/
	Collection operator+(const Collection& col, const Picture& pic)
	{
		Collection Collection;

		Collection = col;
		Collection += pic;

		return Collection;
	}

	/**
	Operator +, to merge two pictures together

	\return A collection which contains the two pictures
	*/
	Collection operator+(const Picture& pic1, const Picture& pic2)
	{
		Collection Collection;

		Collection = pic1;
		Collection += pic2;

		return Collection;
	}


	const MetaData& Collection::metaData() const
	{
		return metadata;
	}

	MetaData& Collection::metaData()
	{
		return metadata;
	}

	void Collection::setMetaData(MetaData& metadata)
	{
		this->metadata = metadata;
	}

	vector<Datum::KeyType> Collection::availableKeys()
	{
		vector<Datum::KeyType> ret;
		ret.push_back(Datum::KeyType("description", Datum::type_string));
		ret.push_back(Datum::KeyType("id", Datum::type_string));
		ret.push_back(Datum::KeyType("title", Datum::type_string));
		ret.push_back(Datum::KeyType("topic", Datum::type_string));
		ret.push_back(Datum::KeyType("in_use", Datum::type_bool));
		ret.push_back(Datum::KeyType("create_date", Datum::type_date));
		ret.push_back(Datum::KeyType("edit_date", Datum::type_date));
		return ret;
	}
}
