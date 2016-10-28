#pragma once

#pragma warning( disable : 4305)
#pragma warning( disable : 4244)
#pragma warning( disable : 4838)

#include "TP1080Defines.h"
#include "UsbWS.h"

//#define WORKPATH "C:\\Users\\dguzun\\Desktop\\"
//#define LOGPATH WORKPATH"%s.log"

class CWSapi
{
private:
	CUsbWS usbObj;
public:
	CWSapi();
	CWSapi(CUsbWS* ptrUsbObj);

	int		vLevel = 0;	// print more messages (0=only Errors, 3=all)
	char	vDst = 'c';	// print more messages ('c'=ToScreen, 'f'=ToFile, 'b'=ToBoth)
	int		readflag = 0;	// Read the weather station or use the cache file.
	int		LogToScreen = 0;	// log to screen

	unsigned short	old_pos = 0;	// last index of previous read
	char		LogPath[255] = "";
	float		pressOffs_hPa = 0;

	void CWS_Cache(char isStoring);
	void CWS_print_decoded_data();
	int CWS_Open();
	int CWS_Close(int NewDataFlg);

	unsigned short CWS_dec_ptr(unsigned short ptr);
	unsigned short CWS_inc_ptr(unsigned short ptr);
	short CWS_DataHasChanged(unsigned char OldBuf[], unsigned char NewBuf[], size_t size);
	short CWS_read_fixed_block();
	int CWS_calculate_rain_period(unsigned short pos, unsigned short begin, unsigned short end);
	int CWS_calculate_rain(unsigned short current_pos, unsigned short data_count);
	float CWS_dew_point(char* raw, float scale, float offset);
	unsigned char CWS_bcd_decode(unsigned char byte);
	unsigned short CWS_unsigned_short(unsigned char* raw);
	signed short CWS_signed_short(unsigned char* raw);
	int CWS_decode(unsigned char* raw, enum ws_types ws_type, float scale, float offset, char* result);

	int CWS_Read();
	int CWF_Write(char arg, const char* fname, const char* ftype);

	void MsgPrintf(int Level, const char *fmt, ...);

	// Weather Station properties
	unsigned char m_buf[WS_BUFFER_SIZE] = { 0 };	// Raw WS data
	time_t m_previous_timestamp = 0;		// Previous readout
	time_t m_timestamp = 0;				// Current readout


	~CWSapi();
};

