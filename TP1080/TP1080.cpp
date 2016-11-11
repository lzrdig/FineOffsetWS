// TP1080.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
//#include <iostream>

#include "getopt.h"

#include "WSapi.h"

#pragma warning( disable : 4996)




//#define WORKPATH "C:\\Users\\dguzun\\Desktop\\"
//#define LOGPATH WORKPATH"%s.log"



char		LogPath[255] = "";
float		pressOffs_hPa = 0;

int		LogToScreen = 0;	// log to screen
int		readflag = 0;	// Read the weather station or use the cache file.


void print_bytes(char *address, int length) {
	int i = 0;								//used to keep track of line lengths
	char *line = (char*)address;			//used to print char version of data
	unsigned char ch;						// also used to print char version of data
	printf("%08X | ", (int)address);		//Print the address we are pulling from
	while (length-- > 0) {
		printf("%02X ", (unsigned char)*address++); //Print each char
		if (!(++i % 16) || (length == 0 && i % 16)) { //If we come to the end of a line...
													  //If this is the last line, print some fillers.
			if (length == 0) { while (i++ % 16) { printf("__ "); } }
			printf("| ");
			while (line < address) {  // Print the character version
				ch = *line++;
				printf("%c", (ch < 33 || ch == 255) ? 0x2E : ch);
			}
			// If we are not on the last line, prefix the next line with the address.
			if (length > 0) { printf("\n%08X | ", (int)address); }
		}
	}
	puts("");
}



int main(int argc, char* argv[])
{
	int bflag = 0;	// Display fixed block
	int dflag = 0;	// Dump decoded fixed block data
	int rflag = 0;	// Dump all weather station records

	int fflag = 0;	// Create fhemws.log
	int pflag = 0;	// Create pywws.log
	int sflag = 0;	// Create pwsweather.log
	int wflag = 0;	// Create wunderground.log
	int xflag = 0;	// Create fowsr.xml

	int NewDataFlg = 0;	// write to cache file or not
	int 	c;
	time_t	tAkt = time(NULL);
	char	Buf[40], Buf2[200];

	strcpy(LogPath, LOGPATH);

	// Load the DLL
	//HINSTANCE dll_handle = ::LoadLibrary(TEXT("TP1080DLL.dll"));
	//if (!dll_handle) {
	//	//std::cerr << "Unable to load DLL!\n";
	//	printf("Unable to load DLL!\n");
	//	return 1;
	//}

	// Get the function from the DLL
	//iklass_factory factory_func = reinterpret_cast<iklass_factory>(
	//	::GetProcAddress(dll_handle, "GetSingleton"));
	//if (!factory_func) {
	//	cerr << "Unable to load create_klass from DLL!\n";
	//	printf("Unable to load create_klass from DLL!\n");
	//	::FreeLibrary(dll_handle);
	//	return 1;
	//}

	//// Ask the factory for a new object implementing the IKlass
	//// interface
	//ISingleton* instance = factory_func();


	ISingleton& myUsbApi = GetSingleton();


	while ((c = getopt(argc, argv, "bcdf:n:rpswxv:")) != -1)
	{
		switch (c)
		{
		case 'b':	// Display fixed block
			bflag = 1;
			break;
		case 'd':	// Dump decoded fixed block data
			dflag = 1;
			break;
		case 'c':
			readflag = 1;
			myUsbApi.CWS_SetReadFlag(readflag);
			LogToScreen = 1;
			break;
		case 'n': {
			readflag = 1;
			strftime(LogPath, sizeof(LogPath), optarg, localtime(&tAkt));
			myUsbApi.MsgPrintf(3, "option -n with value '%s'\n", LogPath);
			break;
		}
		case 'r':	// Dump all weather station records
			rflag = 1;
			break;
		case 'f':
			readflag = 1;
			myUsbApi.CWS_SetReadFlag(readflag);
			switch (optarg[0]) {
			case 'f':	fflag = 1; break;
			case 'p':	pflag = 1; break;
			case 's':	sflag = 1; break;
			case 'w':	wflag = 1; break;
			case 'x':	xflag = 1; break;
			default:
				myUsbApi.MsgPrintf(0, "wrong option -f%s\n", optarg);
				abort();
				break;
			}
			break;
		case 'v':
			if (optarg[1])
				switch (optarg[1]) {
				case 'b':	myUsbApi.vDst = 'b'; break;
				case 'f':	myUsbApi.vDst = 'f'; break;
				default:
					myUsbApi.MsgPrintf(0, "Wrong option -v%s. Used -v%cc instead.\n", optarg, optarg[0]);
				case 'c':
					myUsbApi.vDst = 'c'; break;
				}
			else {
				myUsbApi.MsgPrintf(0, "Wrong option -v%s. Used -v0c instead.\n", optarg);
				myUsbApi.vDst = 'c';
			}
			myUsbApi.vLevel = atoi(optarg);
			//				MsgPrintf (3, "option v with value '%s' / Level=%d Dst=%c\n", optarg, vLevel, vDst);
			break;
		case '?':
			printf("\n");
			printf("Fine Offset Weather Station Reader ""VERSION""\n\n");
			printf("(c) 2013 Joerg Schulz (Josch at abwesend dot de)\n");
			printf("(c) 2010 Arne-Jørgen Auberg (arne.jorgen.auberg@gmail.com)\n");
			printf("Credits to Michael Pendec, Jim Easterbrook, Timo Juhani Lindfors\n\n");
			printf("See http://fowsr.googlecode.com for more information\n\n");
			printf("options\n");
			printf(" -f[p|s|w|x|f]	set Logformat for weather data\n");
			printf(" 	-fp	Logfile in pywws format\n");
			printf("	-fs	Logfile in PWS Weather format\n");
			printf("	-fw	Logfile in Wunderground format\n");
			printf("	-fx	Logfile in XML format\n");
			printf("	-ff	Logfile in FHEM log format\n");
			printf(" -c	Log to screen (in FHEM-WS3600 format)\n");
			printf(" -n<filename>	set full path and name for weather data, may contain\n");
			printf("		%%-wildcards of the POSIX strftime function and %%%%s\n");
			printf("		for a type specific name part\n");
			printf("		default for pywws is: ""WORKPATH""pywws.log\n");
			printf(" -b	Display fixed block\n");
			printf(" -d	Display decoded fixed block data\n");
			printf(" -r	Dump all weather station records\n");
			printf(" -v<Level><Destination>	output debug messages\n");
			printf(" 	Level: 0-3	0-only errors, 3-all\n");
			printf(" 	Destination:	(c)onsole, (f)ile (same place as weather data), (b)oth\n\n");
			//exit(0);
			break;
		default:
			abort();
		}
	}

	strftime(Buf, sizeof(Buf), "%Y-%m-%d %H:%M:%S", localtime(&tAkt));
	Buf2[0] = '\0';
	if (myUsbApi.vLevel >= 3) {
		int i;
		strcpy(Buf2, " Cmd:");
		for (i = 0; i < argc; ++i) {
			sprintf(Buf2 + strlen(Buf2), " %s", argv[i]);
		}
	}
	myUsbApi.MsgPrintf(1, "%s FOWSR ""VERSION"" started%s\n", Buf, Buf2);

	if (0 == myUsbApi.CWS_Open()) {	// Read the cache file and open the weather station
		if (readflag)
			if (myUsbApi.CWS_Read())		// Read the weather station
				NewDataFlg = 1;

		//calc press. offset (representing station height)
		pressOffs_hPa = 0.1 * (
			myUsbApi.CWS_unsigned_short(myUsbApi.m_buf)
			- myUsbApi.CWS_unsigned_short(myUsbApi.m_buf)
			);
		myUsbApi.MsgPrintf(2, "pressure offset = %.1fhPa (about %.0fm a.s.l.)\n",
			pressOffs_hPa, pressOffs_hPa * 8);
		// Write the log files
		if (LogToScreen)
			myUsbApi.CWF_Write('c', "", "");
		if (fflag)
			myUsbApi.CWF_Write('f', LogPath, "WS");
		if (pflag)
			myUsbApi.CWF_Write('p', LogPath, "pywws");
		if (sflag)
			myUsbApi.CWF_Write('s', LogPath, "pwsweather");
		if (wflag)
			myUsbApi.CWF_Write('w', LogPath, "wunderground");
		if (xflag)
			myUsbApi.CWF_Write('x', LogPath, "xml");

		if (bflag)	// Display fixed block
			print_bytes((char*)myUsbApi.m_buf, WS_FIXED_BLOCK_SIZE);
		if (dflag)	// Dump decoded fixed block data
			myUsbApi.CWS_print_decoded_data();
		if (rflag)	// Dump all weather station records
			print_bytes((char*)myUsbApi.m_buf, WS_BUFFER_SIZE - WS_BUFFER_START);

		myUsbApi.CWS_Close(NewDataFlg);	// Write the cache file and close the weather station

		printf("Press ENTER key to close window...");
		getchar();
	}

	return 0;
}

