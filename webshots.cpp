/*
    cWebshots CLI Test Program
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
cWebshots command line test program
*/

#include "Collection.h"

#include <iostream>

using namespace std;
using namespace cWebshots;

// Command line options
enum CLArgs
{
	claUndefined,
	claInput,
	claOutputFormat,
	claOutputFileName,
	claOutputDirectory,
	claIdentify,
	claDefaultCategory,
	claDefaultAlbum,
	claDefaultAlbumID,
	claDefaultCredits,
	claDefaultSource
};

int main(int argc, char* argv[])
{
	CLArgs mode = claUndefined;
	int i;
	string commande;

	// Input pictures
	Collection Entree;

	// Input temporary collection
	Collection EntreeColl;

	// Output format
	Webshots_Formats Format = Unknown;

	// Output folder
	string DossierSortie;

	// Output file
	string FichierSortie;

	// Rename files or not
	bool Rename = false;

	// Default values
	string DefaultCategory = "cWebshots";
	string DefaultAlbum = "Converted files";
	string DefaultAlbumID = "0";
	string DefaultCredits = "cWebshots";
	string DefaultSource = "u";

	cout << "UWC - Ultimate Webshots Converter" << endl
		<< "Version 2.0 beta 7 - Console" << endl
		<< "THIS IS A BETA VERSION, USE IT WITH CARE !" << endl
		<< endl
		<< "Use '--help' command for help." << endl
		<< endl;

	//Entree.metaData().setString("title", "cWebshots - Converted Pictures");
	//Entree.metaData().setString("topic", "cWebshots");

	try
	{
		// Read command line arguments
		for (i = 0 ; i < argc; i++)
		{
			commande.assign(argv[i]);

			// Commands
			if ((commande.compare("-i") == 0) || (commande.compare("--input") == 0))
				mode = claInput;
			else if ((commande.compare("-of") == 0) || (commande.compare("--output-format") == 0))
				mode = claOutputFormat;
			else if ((commande.compare("-ofn") == 0) || (commande.compare("--output-file-name") == 0))
				mode = claOutputFileName;
			else if ((commande.compare("-od") == 0) || (commande.compare("--output-directory") == 0))
				mode = claOutputDirectory;
			else if ((commande.compare("-r") == 0) || (commande.compare("--rename") == 0))
				Rename = true;
			else if ((commande.compare("-h") == 0) || (commande.compare("--help") == 0))
			{
				cout << "Usage :" << endl;
				cout << endl
				<< "-i or --input (\"file\" | \"folder\") [(\"file\" | \"folder\") ...]" << endl
				<< "-of or --output-format (WBC | JPG | WBZ | WB1)" << endl
				<< "-ofn or --output-file-name \"file\"" << endl
				<< "-od or --output-directory \"folder\"" << endl
				<< "-r or --rename" << endl
				<< "-id or --identify (file) *must be used alone*" << endl
				<< "-dc or --default-category (category)" << endl
				<< "-da or --default-album (album)" << endl
				<< "-dai or --default-album-id (albumid)" << endl
				<< "-dcr or --default-credits (credits)" << endl
				<< "-ds or --default-source ( w | c | u )" << endl
				<< endl
				<< "If the output format is WBC or WBZ, the input pictures will be merged into one "
				<< "collection, and saved in the file defined by --output-file-name, "
				<< "in the directory defined with --output-directory." << endl
				<< endl
				<< "For the other formats, if there is only one input picture, it will be saved into the file defined "
				<< "by --output-file-name in the directory defined with --output-directory." << endl
				<< endl
				<< "If there is more than one picture or if the --rename option is set, the name of the output file will be "
				<< "the caption of the picture, and if not available, the original filename "
				<< "of the picture will be used instead." << endl;

				return 1;
			}
			else if ((commande.compare("-id") == 0) || (commande.compare("--identify") == 0))
				mode = claIdentify;
			else if ((commande.compare("-dc") == 0) || (commande.compare("--default-category") == 0))
				mode = claDefaultCategory;
			else if ((commande.compare("-da") == 0) || (commande.compare("--default-album") == 0))
				mode = claDefaultAlbum;
			else if ((commande.compare("-dai") == 0) || (commande.compare("--default-album-id") == 0))
				mode = claDefaultAlbumID;
			else if ((commande.compare("-dcr") == 0) || (commande.compare("--default-credits") == 0))
				mode = claDefaultCredits;
			else if ((commande.compare("-ds") == 0) || (commande.compare("--default-source") == 0))
				mode = claDefaultSource;
			// Values
			else
			{
				// Input files
				if (mode == claInput)
				{
					cout << "Loading " << commande << " ... ";
					try
					{
						EntreeColl.loadFromFile(commande);
					}
					catch (Exceptions::FileSystemError)
					{
						cout << "Error !" << endl
							<< "Warning : This cannot be opened as a file, trying to open it as a folder ... ";
						EntreeColl.loadFromFolder(commande);
					}
					cout << "Ok : " << EntreeColl.pictureCount() << " pictures loaded." << endl;

					Entree += EntreeColl;
					EntreeColl.clear();
				}
				// Output format
				else if (mode == claOutputFormat)
				{
					if (commande == "JPG")
						Format = JPG;
					else if (commande == "WB1")
						Format = WB1;
					else if (commande == "WBZ")
						Format = WBZ;
					else if (commande == "WBC")
						Format = WBC;
					else if (commande == "FOLDER")
						Format = Folder;
					else
					{
						Exceptions::Exception error("Unknown output format : " + commande);
						throw error;
					}
				}
				// Output directory
				else if (mode == claOutputDirectory)
					DossierSortie = commande;
				// Output file
				else if (mode == claOutputFileName)
					FichierSortie = commande;
				// Identify
				else if (mode == claIdentify)
				{
					cout << "The file \"" << commande << "\" seems to be a ";

					switch (identifyFile(commande))
					{
					case WB1:
						cout << "WB1 / JPG";
						break;
					case WBZ:
						cout << "WBZ";
						break;
					case WBC:
						cout << "WBC";
						break;
					case Unknown:
						cout << "unknown";
					}

					cout << " file." << endl;

					return 1;
				}
				// Default values
				else if (mode == claDefaultAlbum)
					DefaultAlbum = commande;
				else if (mode == claDefaultAlbumID)
					DefaultAlbumID = commande;
				else if (mode == claDefaultCategory)
					DefaultCategory = commande;
				else if (mode == claDefaultCredits)
					DefaultCredits = commande;
				else if (mode == claDefaultSource)
					DefaultSource = commande;
			}
		}

		// ---- Start conversion ----

		// If there is no picture to convert
		if (Entree.pictureCount() == 0)
		{
			Exceptions::Exception error("No input files, or no pictures in the input files");
			throw error;
		}

		cout << endl;

		// Check if there is the need to use default values
		for (i = 0; i < Entree.pictureCount(); i++)
		{
			if (Entree[i].metaData().get("title").toString().length() == 0)
			{
				if (Entree[i].metaData().get("original_filename").toString().length() > 0)
				{
					Entree[i].metaData().setString("title", Entree[i].metaData().get("original_filename").toString());
					cout << "Warning : Picture " << i << " has no title. Filename will be used instead." << endl;
				}
				else
				{
					Entree[i].metaData().setString("title", "No title");
					cout << "Warning : Picture " << i << " has no title. Using default title." << endl;
				}
			}

			if ((Entree[i].metaData().get("category").toString().length() == 0) && (Entree[i].metaData().get("album_title").toString().length() == 0))
			{
				Entree[i].metaData().setString("category", DefaultCategory);
				Entree[i].metaData().setString("album_title", DefaultAlbum);
				Entree[i].metaData().setString("album_id", DefaultAlbumID);
				cout << "Warning : Picture " << i << " has no album and category. Using defaults." << endl;
			}

			if (Entree[i].metaData().get("credits").toString().length() == 0)
			{
				Entree[i].metaData().setString("credits", DefaultCredits);
				cout << "Warning : Picture " << i << " has no credit. Using default." << endl;
			}

			if (Entree[i].metaData().get("source").toString().length() == 0)
			{
				Entree[i].metaData().setString("source",  DefaultSource);
				cout << "Warning : Picture " << i << " has no source. Using default." << endl;
			}
		}

		// List the pictures in the collection
		cout << endl << "List of the input pictures :" << endl
			<< endl;
		for (i = 0; i < Entree.pictureCount(); i++)
		{
			cout << "[" << i << "] Title: " << Entree[i].metaData().get("title").toString() << endl
				<< "    ID: " << Entree[i].metaData().get("id").toString() << endl
				<< "    Caption: " << Entree[i].metaData().get("caption").toString() << endl
				<< "    Original Filename: " << Entree[i].metaData().get("original_filename").toString() << endl
				<< endl;
		}


		// If the output format is a single picture format
		if ((Format == JPG) || (Format == WB1))
		{
			string tmpFileName;
			string tmpExt;

			switch (Format)
			{
			case JPG:
				tmpExt = ".jpg";
				break;
			case WB1:
				tmpExt = ".wb1";
			}

			// If there is more than one picture to save, then they must obviously be saved each in one file,
			// so the rename option is turned on
			if (Entree.pictureCount() > 1)
			{
				cout << "Warning: There is more than one picture to save, and the format you selected contains only one picture per file. "
					<< "Turning on the --rename option." << endl
					<< endl;

				Rename = true;
			}

			// For each picture in the input collection
			for (i = 0; i < Entree.pictureCount(); i++)
			{
				if (Rename)
				{
					// Use the caption as the output filename
					if (Entree[i].metaData().get("title").toString().length() != 0)
						tmpFileName = Entree[i].metaData().get("title").toString();
					// if unavailable, use the original filename
					else if (Entree[i].metaData().get("original_filename").toString().length() != 0)
						tmpFileName = Entree[i].metaData().get("original_filename").toString();
				}
				else
				{
					tmpFileName = FichierSortie;
					tmpExt = "";
				}

				// Save the file
				cout << "Saving " << DossierSortie << "\\" << tmpFileName << tmpExt << " ... ";
				Entree[i].saveToFile(DossierSortie + "\\" + tmpFileName + tmpExt, Format);
				cout << "Ok." << endl;
			}
		}
		// If the output format is a collection
		else if ((Format == WBC) || (Format == WBZ))
		{
			cout << "Saving "  << DossierSortie << "\\" << FichierSortie << " ... ";

			// Save the collection
			switch (Format)
			{
			case WBC:
				Entree.saveToFile(DossierSortie + "\\" + FichierSortie, WBC);
				break;

			case WBZ:
				Entree.saveToFile(DossierSortie + "\\" + FichierSortie, WBZ);
			}

			cout << "Ok." << endl;
		}
		else if (Format == Folder)
		{
			cout << "Saving the collection in "  << DossierSortie << endl;

			// Save the collection
			Entree.saveToFolder(DossierSortie);
		}


	}
	catch (Exceptions::Exception &Err)
	{
		cout << endl << "Error : " << Err.Description() << endl;
	}

	return 1;
}
