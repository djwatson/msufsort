#include "stdio.h"
#include "conio.h"
#include "time.h"

#include <string>
#include <iostream>
using namespace std;

#include "MSufSort/MSufSort.h"

static int numErrors = 0;
static int numSuccess = 0;


bool LoadFile(string path, unsigned char * & buffer, unsigned int & nBytes)
{
	// Load file into memory.
	// return false if failure or true if success.

	FILE * fp = fopen(path.c_str(), "rb");
	if (fp == 0)
	{
		buffer = 0;
		nBytes = 0;
		printf("Error: Could not open source file %s\n", path.c_str());
		return false;
	}

	fseek(fp, 0, SEEK_END);
	nBytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// allocate space for string and suffix sort/suffix array (6n total)
	buffer = new unsigned char[(nBytes + 4) * 6];
	fread(buffer, 1, nBytes, fp);
	fclose(fp);

	return true;
}





void DisplayAbout()
{
	// Display app information
	cout << "MSufSort Demo\n";
	cout << "Version: 3.1 *BETA*\n";
	cout << "Authors: Michael A. Maniscalco\n";
	cout << "         Simon J. Puglisi\n";
	cout << "Date:    February 3, 2007\n";
	cout << "\n";
	cout << "This is a BETA version of the algorithm.\n";
	cout << "Do not use except for test purposes.\n";
	cout << "\n";
}





void DisplayUsage()
{
	// displays the options available with this demo
	cout << "Options:\n";
	cout << "b|B\t\tCompute the BWT on the input file\n";
	cout << "u|U\t\tReverse the BWT on the input file\n";
	cout << "s|S\t\tCompute the suffix array of the input file\n";
	cout << "h|H|?\t\tHelp: Usage, version info, authors etc.\n";
	cout << "\n";
	cout << "Example usage:\n";
	cout << "MSufSort.exe b myFile.txt myFile.bwt\n";
	cout << "MSufSort.exe u myFile.bwt myFile.txt\n";
	cout << "MSufSort.exe s myFile.txt myFile.sa\n";
	cout << "MSufSort.exe h\n";
}





void BWT(string path, string destPath)
{
	// perform BWT on file using MSufSort 3.1
	unsigned char * buffer;
	unsigned int nBytes;
	MSufSort * msufsort = new MSufSort;

	if (LoadFile(path, buffer, nBytes))
	{		
		if (nBytes >= 2)	
		{
			unsigned int index;
			int start = clock();
			msufsort->BWT(buffer, nBytes, index);
			int finish = clock();
			cout << "Elapsed time: " << (double(finish - start) / CLOCKS_PER_SEC) << " seconds\n";

			FILE * fp = fopen(destPath.c_str(), "wb");
			fwrite((void *)&index, 1, 4, fp);
			fwrite(buffer, 1, nBytes, fp);
			fclose(fp);
		}
		else
			cout << "Source file is less than 2 bytes.  No BWT attempted.\n";
		delete [] buffer;
		cout << "\n\n";
	}
	delete msufsort;
}






void UnBWT(string path, string destPath)
{
	// perform reverse BWT on file using MSufSort 3.1
	unsigned char * buffer;
	unsigned int nBytes;
	MSufSort * msufsort = new MSufSort;

	if (LoadFile(path, buffer, nBytes))
	{
		bool invalid = false;
		if (nBytes < 4)
			invalid = true;
		nBytes -= 4;
		unsigned int n = *(unsigned int *)buffer;
		if (nBytes <= n)
			invalid = true;
		if (invalid)
		{
			cout << "Invalid input file\n";
			delete [] buffer;
			delete msufsort;
			return;
		}

		int start = clock();
		msufsort->UnBWT(buffer + 4, nBytes, n);
		int finish = clock();
		cout << "Elapsed time: " << (double(finish - start) / CLOCKS_PER_SEC) << " seconds\n";

		FILE * fp = fopen(destPath.c_str(), "wb");
		fwrite(buffer + 4, 1, nBytes, fp);
		fclose(fp);
		delete [] buffer;
		cout << "\n\n";
	}
	delete msufsort;
}








void SuffixArray(string path, string destPath)
{
	// perform suffix sort on file using MSufSort 3.1
	unsigned char * buffer;
	unsigned int nBytes;
	MSufSort * msufsort = new MSufSort;

	if (LoadFile(path, buffer, nBytes))
	{		
		if (nBytes >= 2)	
		{
			int start = clock();
			msufsort->Sort(buffer, nBytes);
			int finish = clock();
			cout << "Elapsed time: " << (double(finish - start) / CLOCKS_PER_SEC) << " seconds\n";

			FILE * fp = fopen(destPath.c_str(), "wb");
			fwrite(buffer + ((nBytes + 3) & ~3), 1, (nBytes + 1) << 2, fp);
			fclose(fp);
		}
		else
			cout << "Source file is less than 2 bytes.  No sort attempted.\n";
		delete [] buffer;
		cout << "\n\n";
	}
	delete msufsort;
}







int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		DisplayAbout();
		DisplayUsage();
		return 1;
	}

	if (strlen(argv[1]) != 1)
	{
		// bad option
		cout << "Invalid option: " << argv[1] << "\n";
		DisplayUsage();
		return 0;
	}

	switch (argv[1][0])
	{
		case 'b':
		case 'B':
		{
			if (argc != 4)
			{
				DisplayUsage();
				return 0;
			}
			BWT(argv[2], argv[3]);
			break;
		}

		case 's':
		case 'S':
		{
			if (argc != 4)
			{
				DisplayUsage();
				return 0;
			}
			SuffixArray(argv[2], argv[3]);
			break;
		}

		case 'u':
		case 'U':
		{
			if (argc != 4)
			{
				DisplayUsage();
				return 0;
			}
			UnBWT(argv[2], argv[3]);
			break;
		}

		case 'h':
		case 'H':
		case '?':
		{
			DisplayAbout();
			DisplayUsage();
			break;
		}

		default:
		{
			cout << "Invalid option: " << argv[1] << "\n";
			DisplayUsage();
			return 0;
		}
	}

	return 0;
}
