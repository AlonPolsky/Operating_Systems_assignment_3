#ifndef MSGSLOT_H
#define MSG_SLOT_H

#include <linux/ioctl.h>
#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
//#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/string.h>   /* for memset. NOTE - not string.h!*/
//#include <linux/slab.h>
#include <errno.h>

#define MAJOR_NUM 235

#define DEVICE_NAME "msg_slot"
#define BUF_LEN 128
#define MAX_MSGSLOTS 256
// It may be needed to have more than one File name.
#define DEVICE_FILE_RANGE "msg_slot_"
#define MAX_CHANNELS 1 < 20
#define SUCCESS 0
#define FAIL -1
#define FREE_CHANNEL 0 // We use that number to indicate free channel and illigal channel numbers because it's an illigal channel number.
#define ILLEGAL_INDX -1
#define MSG_SLOT_CHANNEL _IO(MAJOR_NUM, 0, unsigned long)
#define MIN_WRITE 1
#define CHANNEL_INDX ((file_data*)file->private_data)->channel_indx
#define MINOR ((file_data*)file->private_data)->minor
#define ERROR_CHECK(condition, extra_code, errno_value){\
    if(condition){\
        extra_code;\
        return errno_value;\
    }\
}

// A data structure that every device file will contain, channel_array will store the channel numbers that are cureently used.
// If the channel_array[i] != 0, then channel_array[i] is the number of the channel that has its message stored at msgs[sizeof(int) i].
// We add sizeof(int) chars in the beginning of every message so we'll be a able to infer the message length.
typedef struct Channels{
    unsigned int channels_array[MAX_CHANNELS];
    char msgs[MAX_CHANNELS][BUF_LEN + sizeof(size_t)];
}Channels;

// This data structure stores the minor number of a struct file and the channel indx of the struct file in channels[minor]->channels_array.
typedef struct file_data{
    byte minor;
    int channel_indx;
}file_data;

#endif