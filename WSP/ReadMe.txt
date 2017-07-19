========================================================================
    CONSOLE APPLICATION : WSP Project Overview

This project is a compilation of the code written by Joakim Söderberg and the libusb library.
For the argument passing in a console app the getopt library was added.

The app is to establish connection to a FineOffset type weather station (WH-1080).
The device registers itself as a HID type of device on most OS.

========================================================================

AppWizard has created this WSP application for you.

This file contains a summary of what you will find in each of the files that
make up your WSP application.


WSP.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

WSP.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

WSP.cpp
    This is the main application source file provided by Joakim Söderberg.
	Small modifications were done to adapt to getopt library.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp - these files have been excluded
    These files are used to build a precompiled header (PCH) file
    named WSP.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

The main point to understand in the communication with FineOffset is that its entire memory
consists of two sections. The total size of the memory is (256 + 4080)*32 bytes (spans from 0x000 to 0xFFF).

The 0x000 - 0x100 section stoes the station settings data. The rest is allocated for storing the weather data history.

/////////////////////////////////////////////////////////////////////////////
