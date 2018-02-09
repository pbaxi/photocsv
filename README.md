# photocsv
Create csv file (to stdout) from images in current directory.
Column headings are description, comment, date, filename.
 
Description taken from Exif.Image.ImageDescription.
Comment from Exif.Photo.UserComment.
Date from Exif.Photo.DateTimeOriginal.
Filename from path given by ntfw call, "./" removed.
 
Leading and trailing whitespace is removed.
Double quotes in strings replaced with single quotes.
Non-printable characters in strings replaced with space.

This can conveniently be used to create labels for
sticking in photo albums.

Needs exiv2 library and development files.
 
build by:
g++ -o photocsv photocsv.cpp -lexiv2
