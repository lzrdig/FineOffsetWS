#pragma once

#pragma warning( disable : 4305)
#pragma warning( disable : 4244)
#pragma warning( disable : 4838)
#pragma warning( disable : 4309)
#pragma warning( disable : 4018)


#include "UsbWS.h"


#include "TP1080Defines.h"


//--------  the interaction point that exposes the WSapi singleton class

//#define TP1080DLL_EXPORTS 


#if defined(TP1080DLL_EXPORTS) // inside DLL
#   define TP1080DLL   __declspec(dllexport)
#else // outside DLL
#   define TP1080DLL   __declspec(dllimport)
#endif  // TP1080DLL_EXPORT




class ISingleton
{
public:
	int		vLevel = 0;			// print more messages (0=only Errors, 3=all)
	char	vDst = 'c';			// print more messages ('c'=ToScreen, 'f'=ToFile, 'b'=ToBoth)

	// Weather Station properties
	unsigned char m_buf[WS_BUFFER_SIZE] = { 0 };	// Raw WS data
	time_t m_previous_timestamp = 0;				// Previous readout
	time_t m_timestamp = 0;							// Current readout

	virtual int CWS_Open() = 0;
	virtual int CWS_Close(int NewDataFlg) = 0;
	virtual void MsgPrintf(int Level, const char *fmt, ...) = 0;
	virtual unsigned short CWS_unsigned_short(unsigned char* raw) = 0;

	virtual int CWS_Read() = 0;
	virtual int CWF_Write(char arg, const char* fname, const char* ftype) = 0;

	virtual void CWS_SetReadFlag(int readFlag) = 0;
	virtual int CWS_GetReadFlag() = 0;

	virtual void CWS_print_decoded_data() = 0;
};



extern "C" TP1080DLL ISingleton & APIENTRY GetSingleton();

//--------  end of implementation of the interaction point




// A factory of IKlass-implementing objects looks thus
//typedef ISingleton* (__cdecl *iklass_factory)();


class CWSapi : public ISingleton
{
private:
	//CUsbWS &_UsbObj;
public:
	CWSapi();
	//CWSapi(CUsbWS* ptrUsbObj);

	
	
	int		readflag = 0;		// Read the weather station or use the cache file.
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

	void CWS_SetReadFlag(int readFlag);

	int CWS_GetReadFlag();

	void MsgPrintf(int Level, const char *fmt, ...);




	~CWSapi();
};



