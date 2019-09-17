#ifndef __USB_H__
#define __USB_H__
	
struct usb_handle
{
    char fname[64];
    int desc;
    unsigned char ep_in;
    unsigned char ep_out;
};

typedef struct usb_handle usb_handle;

typedef struct usb_ifc_info usb_ifc_info;

struct usb_ifc_info
{
        /* from device descriptor */
    unsigned short dev_vendor;
    unsigned short dev_product;

    unsigned char dev_class;
    unsigned char dev_subclass;
    unsigned char dev_protocol;

    unsigned char ifc_class;
    unsigned char ifc_subclass;
    unsigned char ifc_protocol;

    unsigned char has_bulk_in;
    unsigned char has_bulk_out;

    unsigned char writable;

    char serial_number[256];
    char device_path[256];
};

typedef int (*ifc_match_func)(usb_ifc_info *ifc);

usb_handle *usb_open(ifc_match_func callback);
int usb_close(usb_handle *h);
int usb_read(usb_handle *h, void *_data, int len);
int usb_write(usb_handle *h, const void *_data, int len);

#endif // __USB_H__
