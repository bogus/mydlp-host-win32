This project's aim is to create a host based data loss prevention tool with using Clamav as 
scanning tool.Uses other GPL tools to inspect data in special data formats.

These kind of data analysis added up to now.

- Credit Card (Clamav)
- SSN (Clamav)
- Encrypted Archive
- Custom regex
- File Md5
- Turkish Republic ID Number - (TC Kimlik No)
- IBAN

Over these files types:

- Ms Office 2003 (Doc, ppt)
- Ms Office 2007 (Docx, etc)
- OpenOffice.org
- PDF - text based
- PS - text based
- Almost all archive types
- Ordinary text files

Future releases will contain:

- Source code detection - partial or full
- Enable / disable computer peripherals
- Log computer peripheral actions (USB, CDROM, DVDROM, etc)
- Central management over Network

This project is being developed over MS Visual C++ 2008 Express and MS Visual C# 2008 Express

Components

- mh-clamwin	: Forked from clam-win32 project. Added some features to libclamav
- mydlp-sf	: A dll project which uses libclamav to inspect files for sensitive files. 
		Can conduct system wide or directory based scans. Communicates with fsmf and
		detects sensitive file transfers and blocks them if any sensitive data detected.
- fsmf		: A file system minifilter which listens file transfers to removable devices. 
		Communicates with user mode application in mydlp-sf.

If you want any more features you can simply contact with me. 
