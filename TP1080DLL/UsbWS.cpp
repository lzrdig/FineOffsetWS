#include "stdafx.h"

#include "UsbWS.h"



CUsbWS::CUsbWS()
{

}



struct usb_device * CUsbWS::find_device(int vendor, int product)
{
	struct usb_bus *bus;

	for (bus = usb_get_busses(); bus; bus = bus->next) {
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor == vendor
				&& dev->descriptor.idProduct == product)
				return dev;
		}
	}
	return NULL;
}



int CUsbWS::CUSB_Open(int vendor, int product)
{
	int ret;
	char buf[1000];

	usb_init();
	if (vDst != 'f')
		usb_set_debug(vLevel + 1);
	usb_find_busses();
	usb_find_devices();

	dev = find_device(vendor, product);
	if (!dev) {
		MsgPrintf(0, "Weatherstation not found on USB (vendor,product)=(%04X,%04X)\n", vendor, product);
		printf("Press any key to exit...");
		getchar();
		return -1;
	}
	devh = usb_open(dev);
	if (!devh) {
		MsgPrintf(0, "Open USB device failed (vendor,product)=(%04X,%04X)\n", vendor, product);
		return -2;
	}
	signal(SIGTERM, (void(*)(int))static_USB_Close);

#ifdef LIBUSB_HAS_GET_DRIVER_NP
	ret = usb_get_driver_np(devh, 0, buf, sizeof(buf));
	MsgPrintf(3, "usb_get_driver_np returned %d\n", ret);
	if (ret == 0) {
		MsgPrintf(1, "interface 0 already claimed by driver \\'%s\\', attempting to detach it\n", buf);
#ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
		ret = usb_detach_kernel_driver_np(devh, 0);
		MsgPrintf(ret ? 0 : 1, "usb_detach_kernel_driver_np returned %d\n", ret);
#endif
	}
#endif

	ret = usb_claim_interface(devh, 0);
	if (ret<0) {
		MsgPrintf(0, "claim failed with error %d\n", ret);
		return 0;	// error, but device is already open and needs to be closed
	}

	ret = usb_set_altinterface(devh, 0);
	if (ret<0) {
		MsgPrintf(0, "set_altinterface failed with error %d\n", ret);
		return 0;	// error, but device is already open and needs to be closed
	}
	ret = usb_get_descriptor(devh, 1, 0, buf, 0x12);
	ret = usb_get_descriptor(devh, 2, 0, buf, 0x09);
	ret = usb_get_descriptor(devh, 2, 0, buf, 0x22);
	ret = usb_release_interface(devh, 0);
	if (ret) MsgPrintf(0, "failed to release interface before set_configuration: %d\n", ret);
	ret = usb_set_configuration(devh, 1);
	ret = usb_claim_interface(devh, 0);
	if (ret) MsgPrintf(0, "claim after set_configuration failed with error %d\n", ret);
	ret = usb_set_altinterface(devh, 0);
	ret = usb_control_msg(devh, USB_TYPE_CLASS + USB_RECIP_INTERFACE, 0xa, 0, 0, buf, 0, 1000);
	ret = usb_get_descriptor(devh, 0x22, 0, buf, 0x74);

	return 0;
}

void CUsbWS::CUSB_Close()
{
	int ret = usb_release_interface(devh, 0);
	if (ret) MsgPrintf(0, "failed to release interface: %d\n", ret);
	ret = usb_close(devh);
	if (ret) MsgPrintf(0, "failed to close interface: %d\n", ret);
}

void CUsbWS::static_USB_Close()
{
	getInstance().CUSB_Close();
}

short CUsbWS::CUSB_read_block(unsigned short ptr, char* buf)
{
	/*
	Read 32 bytes data command
	After sending the read command, the device will send back 32 bytes data wihtin 100ms.
	If not, then it means the command has not been received correctly.
	*/
	char buf_1 = (char)(ptr / 256);
	char buf_2 = (char)(ptr & 0xFF);
	char tbuf[8];
	tbuf[0] = 0xA1;		// READ COMMAND
	tbuf[1] = buf_1;	// READ ADDRESS HIGH
	tbuf[2] = buf_2;	// READ ADDRESS LOW
	tbuf[3] = 0x20;		// END MARK
	tbuf[4] = 0xA1;		// READ COMMAND
	tbuf[5] = 0;		// READ ADDRESS HIGH
	tbuf[6] = 0;		// READ ADDRESS LOW
	tbuf[7] = 0x20;		// END MARK

	// Prepare read of 32-byte chunk from position ptr
	int ret = usb_control_msg(devh, USB_TYPE_CLASS + USB_RECIP_INTERFACE, 9, 0x200, 0, tbuf, 8, 1000);
	if (ret<0) MsgPrintf(0, "usb_control_msg failed (%d) whithin CUSB_read_block(%04X,...)\n", ret, ptr);
	else {
		// Read 32-byte chunk and place in buffer buf
		ret = usb_interrupt_read(devh, 0x81, buf, 0x20, 1000);
		if (ret<0) MsgPrintf(0, "usb_interrupt_read failed (%d) whithin CUSB_read_block(%04X,...)\n", ret, ptr);
	}
	return ret;
}



void CUsbWS::MsgPrintf(int Level, const char *fmt, ...)
{
	char    Buf[200];
	va_list argptr;
	FILE	*f;

	if (Level>vLevel)
		return;
	va_start(argptr, fmt);
	vsprintf(Buf, fmt, argptr);
	va_end(argptr);
	if (vDst != 'f') {
		printf("%s", Buf);
	}
	if ((vDst == 'b') || (vDst == 'f')) {
		f = fopen(WORKPATH"fowsr.msg", "at");
		if (f) {
			fprintf(f, "%s", Buf);
			fclose(f);
		}
	}
}
