cWebshots
=========

cWebshots is a set of two C++ classes created to handle easily Webshots files.

 * The class cWebshotsPicture handles individual pictures
 * The class cWebshotsCollection handles collections

The usage is quite simple, you can see it by reading the project documentation (generate it from the source code using Doxygen) or the source code of the CLI test program (webshots.cpp). 

Usage
-----

There are two main classes : cWebshotsPicture and cWebshotsCollection.

 * The first, cWebshotsPicture handles individual pictures. Supported formats are JPG and WB1.
 * The last, cWebshotsCollection handles collections of pictures. It is a sort of vector of cWebshotsPicture. Supported formats are WBZ, WBC, WB1, JPG and folders (newest Webshots albums are stored in folders).

In practice, you can handle about everything with just cWebshotsCollection.

You can load a file by instancing a class and giving the file name as the constructor parameter.

    mypicture = new cWebshotsPicture('mypic.wb1');
    mycollection = new cWebshotsCollection('mycollection.wbc');
    
Affectation and addition operator (=, +, +=) are defined for and between the two classes, so you can easily add a new picture to a collection by writing for example:

    mycollection += mypicture;
    
You can access pictures in a collection by using the array operator.

    mypicture = mycollection[i];
    
Finally you can save a picture or a collection using the function saveToFile, giving the output format as the second parameter.

    mycollection.saveToFile('outfile.wbz', WBZ);
    mypicture.saveToFile('outfile.jpg', JPG);

This is just an overlook of the different functions, to see all the function, read the doc and the example CLI program.
