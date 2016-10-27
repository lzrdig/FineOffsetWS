#pragma once


#define WORKPATH "C:\\Users\\dguzun\\Desktop\\"
#define LOGPATH WORKPATH"%s.log"


class CUsbWS
{
private:
	static CUsbWS gUsbObj;	//singleton

public:
	CUsbWS();
	~CUsbWS();

	int		LogToScreen = 0;	// log to screen
	int		readflag = 0;	// Read the weather station or use the cache file.
	int		vLevel = 0;	// print more messages (0=only Errors, 3=all)
	char	vDst = 'c';	// print more messages ('c'=ToScreen, 'f'=ToFile, 'b'=ToBoth)

	unsigned short	old_pos = 0;	// last index of previous read
	char		LogPath[255] = "";
	float		pressOffs_hPa = 0;

	// libusb structures and functions
	struct usb_dev_handle *devh;
	struct usb_device *dev;

	//void list_devices();
	struct usb_device *find_device(int vendor, int product);

	int CUSB_Open(int vendor, int product);
	void CUSB_Close();
	static void static_USB_Close();
	short CUSB_read_block(unsigned short ptr, char* buf);


	void MsgPrintf(int Level, const char *fmt, ...);
};

