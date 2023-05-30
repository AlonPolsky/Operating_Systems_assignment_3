#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE

#include "message_slot.h"


// MODULE_LICENSE("GPL");

// The msg_slot with minor number i has its data stored in msg_slots[i].
static Channels msg_slots[MAX_MSGSLOTS];


//================== DEVICE FUNCTIONS ===========================
static int device_open(struct inode* inode,
                        struct file*  file )
{
    // Here we fill file->private_data correctly.
    ERROR_CHECK((file->private_data = (void*) kmalloc(sizeof(file_data), GFP_KERNEL)) == NULL,,EMVSDYNALC)

    MINOR = iminor(inode);
    CHANNEL_INDX = ILLEGAL_INDX;

    return SUCCESS;
}

//---------------------------------------------------------------
static int device_release( struct inode* inode,
                           struct file*  file)
{
    kfree(file->private_data);
    return SUCCESS;
}

//---------------------------------------------------------------
static ssize_t device_read( struct file* file,
                            char __user* buffer,
                            size_t       length,
                            loff_t*      offset )
{
    // First we check arguments, then we transfer the message into the buffer, while continuing the verification.

    int i;
    char* checker;

    ERROR_CHECK(CHANNEL_INDX == ILLEGAL_INDX, , EINVAL)
    ERROR_CHECK(((size_t*)(msg_slots[MINOR]->msgs[CHANNEL_INDX]))* == 0, , EWOULDBLOCK)
    ERROR_CHECK(length <((size_t*)(msg_slots[MINOR]->msgs[CHANNEL_INDX]))*, , ENOSPC)

    for(i = 0; i < length; i++)
    {
        ERROR_CHECK(get_user(checker, buffer + i),,EINVAL)
    }

    for(i = 0; i < ((size_t*)msg_slots[MINOR]->msgs[CHANNEL_INDX])*; i++)
        ERROR_CHECK(put_user(msg_slots[MINOR]->msgs[CHANNEL_INDX][sizeof(size_t) + i], buffer + i),, EINVAL)
    
    return i;
}

//---------------------------------------------------------------

static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
                             size_t             length,
                             loff_t*            offset)
{
    // First we check arguments, then we transfer the message from the buffer, while continuing the verification.

    size_t i;
    char checker;

    ERROR_CHECK(CHANNEL_INDX == ILLEGAL_INDX, , EINVAL)
    ERROR_CHECK(length > BUF_LEN || length < MIN_WRITE, , EMSGSIZE)

    ERROR_CHECK(access_ok(VERIFY_READ, buffer, length) != 0,, EINVAL)

    for(i = 0; i < length; i++)
    {
        ERROR_CHECK(get_user(checker, buffer + i),,EINVAL)
    }

    ((size_t*)msg_slots[MINOR]->msgs[CHANNEL_INDX])* = length;

    for(i = 0; i < length; i++)
    {
        ERROR_CHECK(get_user(msg_slots[MINOR]->msgs[CHANNEL_INDX][sizeof(size_t) + i], buffer + i),,EINVAL)
    }

    return i;
}

//----------------------------------------------------------------
static ssize_t device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned int ioctl_param )
{
    // Here we find the right channel id in msg_slots[((file_data*)file->private_data) -> minor]->channels_array, then we update
    // the data structures accordingly.
    ssize_t i;
    int min = ILLEGAL_INDX;

    ERROR_CHECK(ioctl_command_id != MSG_SLOT_CHANNEL || !ioctl_param, ,EINVAL)


    for (i = 0; i < MAX_CHANNELS; i++)
    {
        if (msg_slots[MINOR]->channels_array[i] == ioctl_param)
        {
            break;
        }
        if ((min == ILLEGAL_INDX) && (msg_slots[MINOR]->channels_array[i] == FREE_CHANNEL))
        {
            min = i;
        }
    }
    

    if(i == MAX_CHANNELS)
    {
        CHANNEL_INDX = min;
        msg_slots[MINOR]->channels_array[min] = ioctl_param;
    }
    else
        CHANNEL_INDX = i;
    
    return SUCCESS;
}

//==================== DEVICE SETUP =============================

// This structure will hold the functions to be called
// when a process does something to the device we created
struct file_operations Fops = {
  .owner	  = THIS_MODULE, 
  .read           = device_read,
  .write          = device_write,
  .open           = device_open,
  .unlocked_ioctl = device_ioctl,
  .release        = device_release,
};

//---------------------------------------------------------------
// Initialize the module - Register the character device
static int __init simple_init(void)
{
    int rc;

    ERROR_CHECK((rc = r_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops)) < 0, printk(KERN_ERR "%s registraion failed for  %d\n", DEVICE_FILE_RANGE, MAJOR_NUM), rc)
    
    return SUCCESS;
}

//---------------------------------------------------------------
static void __exit simple_cleanup(void)
{
  unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

//---------------------------------------------------------------
module_init(simple_init);
module_exit(simple_cleanup);

//========================= END OF FILE =========================
