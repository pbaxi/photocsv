/*
 * Create csv file (to stdout) from images in current directory.
 * Column headings are description, comment, date, filename.
 * 
 * Description taken from Exif.Image.ImageDescription.
 * Comment from Exif.Photo.UserComment.
 * Date from Exif.Photo.DateTimeOriginal.
 * Filename from path given by ntfw call, "./" removed.
 * 
 * Leading and trailing whitespace is removed.
 * Double quotes in strings replaced with single quotes.
 * Non-printable characters in strings replaced with space.
 * 
 * Needs exiv2 library and development files.
 * 
 * g++ -o photocsv photocsv.cpp -lexiv2
*/

#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <exiv2/exiv2.hpp>
#include <stdint.h>

#include <iostream>
#include <string>

using namespace std;

// remove undesirable characters from strings copied from metadata
void cleanit(string& str)
{
	int i;
	
	// replace n non-printable chars with space
	for(i=0; i<str.length(); i++)
    {
		if(!isprint(str[i])) str[i] = ' ';
	}
	
	// replace double quotes with single quotes)
	for(i=0; i<str.length(); i++)
	{
		if(str[i] == '\"') str[i] = '\'';
	}
		
	//remove spurious whitespace at start
	for(i=0; i<str.length(); i++) if(!isspace(str[i])) break;
	if(i != 0) str.erase(0,i);
	
	// remove whitespace at end
	for(i=str.length()-1; i>0; i--) if(!isspace(str[i])) break;
	if(i < str.length() -1) str.erase(i+1,str.length());
	
}

// get the metadata from the image
int getInfo(const char *fpath, const struct stat *sb,
				int tflag, struct FTW *ftwbuf)
{
	int ftype = Exiv2::ImageFactory::getType(fpath);
	
	// return if not acceptable file type	
	if(ftype == Exiv2::ImageType::none) return 0;
	
	Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(fpath);
	
	if(image.get() == 0) return 0;
	
	
	// read the meta data
	image->readMetadata();
    Exiv2::ExifData exif = image->exifData();
    
    // get description
    string desc("");
    Exiv2::ExifKey key("Exif.Image.ImageDescription");
    Exiv2::ExifData::iterator pos = exif.findKey(key);
    if (! (pos == exif.end()) ) desc = pos->getValue()->toString();
 
    // get date
    string dateTime;
    key = Exiv2::ExifKey("Exif.Photo.DateTimeOriginal");
    pos = exif.findKey(key);
    if (! (pos == exif.end()) ) dateTime = pos->getValue()->toString();
    
	 
    // get comment
    string comment;
    string chset("charset=\"Ascii\""); // usually precedes string
    
    key = Exiv2::ExifKey("Exif.Photo.UserComment");
    pos = exif.findKey(key);
    if (! (pos == exif.end()) ) comment = pos->getValue()->toString();
  	
  	// if metadata starts with charset ascii, remove it
  	size_t found = comment.find(chset);
  	if (found!=std::string::npos) comment.replace(found,chset.length(),"");
  	
  	// get path to file, remove leading "./"
  	string path(fpath);
  	path.erase(0,2);
  	
  	// remove anything spurious from strings
	cleanit(dateTime);
	cleanit(desc);
	cleanit(comment);
	cleanit(path);
	
  	// print them out
  	cout << "\"" << desc << "\",\"" << comment << "\",\"" << dateTime << "\",\"" << path << "\"";
  	cout << "\n";
  	
 	return 0;
 }
    



int main(int argc, char *argv[])
{
	const char *path = ".";
	
	// print column headers
	cout << "\"description\",\"comment\",\"date\",\"filename\"\n";
	
	if( nftw(path, getInfo, 20, 0) == -1)
	{
		cout << "nftw failed\n";
		return 1;
	}
	else return 0;
}
	

