// TP1080.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <getopt.h>


#define WORKPATH "C:\\Users\\dguzun\\Desktop\\"
#define LOGPATH WORKPATH"%s.log"

char		LogPath[255] = "";
float		pressOffs_hPa = 0;

int		LogToScreen = 0;	// log to screen
int		readflag = 0;	// Read the weather station or use the cache file.

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
			LogToScreen = 1;
			break;
		case 'n': {
					  readflag = 1;
					  strftime(LogPath, sizeof(LogPath), optarg, localtime(&tAkt));
					  MsgPrintf(3, "option -n with value '%s'\n", LogPath);
					  break;
		}
		case 'r':	// Dump all weather station records
			rflag = 1;
			break;
		case 'f':
			readflag = 1;
			switch (optarg[0]) {
			case 'f':	fflag = 1; break;
			case 'p':	pflag = 1; break;
			case 's':	sflag = 1; break;
			case 'w':	wflag = 1; break;
			case 'x':	xflag = 1; break;
			default:
				MsgPrintf(0, "wrong option -f%s\n", optarg);
				abort();
				break;
			}
			break;
		case 'v':
			if (optarg[1])
				switch (optarg[1]) {
				case 'b':	vDst = 'b'; break;
				case 'f':	vDst = 'f'; break;
				default:
					MsgPrintf(0, "Wrong option -v%s. Used -v%cc instead.\n", optarg, optarg[0]);
				case 'c':
					vDst = 'c'; break;
			}
			else {
				MsgPrintf(0, "Wrong option -v%s. Used -v0c instead.\n", optarg);
				vDst = 'c';
			}
			vLevel = atoi(optarg);
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
	if (vLevel >= 3) {
		int i;
		strcpy(Buf2, " Cmd:");
		for (i = 0; i<argc; ++i) {
			sprintf(Buf2 + strlen(Buf2), " %s", argv[i]);
		}
	}
	MsgPrintf(1, "%s FOWSR ""VERSION"" started%s\n", Buf, Buf2);

	if (0 == CWS_Open()) {	// Read the cache file and open the weather station
		if (readflag)
		if (CWS_Read())		// Read the weather station
			NewDataFlg = 1;

		//calc press. offset (representing station height)
		pressOffs_hPa = 0.1 * (
			CWS_unsigned_short(&m_buf[WS_CURR_REL_PRESSURE])
			- CWS_unsigned_short(&m_buf[WS_CURR_ABS_PRESSURE])
			);
		MsgPrintf(2, "pressure offset = %.1fhPa (about %.0fm a.s.l.)\n",
			pressOffs_hPa, pressOffs_hPa * 8);
		// Write the log files
		if (LogToScreen)
			CWF_Write('c', "", "");
		if (fflag)
			CWF_Write('f', LogPath, "WS");
		if (pflag)
			CWF_Write('p', LogPath, "pywws");
		if (sflag)
			CWF_Write('s', LogPath, "pwsweather");
		if (wflag)
			CWF_Write('w', LogPath, "wunderground");
		if (xflag)
			CWF_Write('x', LogPath, "xml");

		if (bflag)	// Display fixed block
			print_bytes((char*)m_buf, WS_FIXED_BLOCK_SIZE);
		if (dflag)	// Dump decoded fixed block data
			CWS_print_decoded_data();
		if (rflag)	// Dump all weather station records
			print_bytes((char*)&m_buf[WS_BUFFER_START], WS_BUFFER_SIZE - WS_BUFFER_START);

		CWS_Close(NewDataFlg);	// Write the cache file and close the weather station

		printf("Press any key...");
		getchar();
	}

	return 0;
}

