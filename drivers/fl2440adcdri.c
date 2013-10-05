/****************************************************************************
* This file is part of s3c2440adc, a project for S3C2440A ADC.
* Copyright (C) 2013 Tseesing
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*
* Contact: chen-qx@live.cn
*
****************************************************************************/


#ifndef CONFIG_S3C_ADC
#error  "This driver need Samsung ADC common driver"
#else


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/device.h> // class_create, device_create
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/kdev_t.h> /// MKDEV MAJOR xxx
#include <asm/uaccess.h>

#include <linux/platform_device.h>
#include <plat/adc.h> // s3c_adc_xxx

#include "fl2440adc.h"

struct fl2440_adc_dev {
    dev_t   devno;
    struct cdev fcdev[ADCNUM];

    struct platform_device ** pltdev;
    struct platform_driver * pltdri;
    struct s3c_adc_client * client[ADCNUM];
    atomic_t client_ref[ADCNUM];
    struct mutex mutex;
    
    struct class * dev_class;
    struct device * cls_dev[ADCNUM];
};


static struct fl2440_adc_dev adc_dev;

static int fl2440adc_open (struct inode * inodp, struct file * filp)
{
    unsigned int channel;
    struct s3c_adc_client * client;
    
    DPRINTK("File operations open...\n");

    if (filp->f_flags & O_NONBLOCK) // S3C_ADC 驱动中读取时有睡眠的可能, 所以不接受非阻塞的访问
        return -EINVAL;

    channel = MINOR(inodp->i_rdev);


    mutex_lock(&adc_dev.mutex);

    if (adc_dev.client[channel])
    {
        mutex_unlock(&adc_dev.mutex);
        return 0; // 用户空间有可能同一时间段有数个进程都在打开着这个设备
    }
    else
    {
        filp->private_data = (void *)channel;
    }

    client = s3c_adc_register(adc_dev.pltdev[channel], NULL, NULL, 0);
    if (IS_ERR(client))
    {
        DPRINTK("Get S3C ADC clietn failed...\n");
        adc_dev.client[channel] = NULL;
        mutex_unlock(&adc_dev.mutex);
        return PTR_ERR(client);
    }
    adc_dev.client[channel] = client;
    atomic_inc(&adc_dev.client_ref[channel]);
    mutex_unlock(&adc_dev.mutex);

    return 0;
}

static ssize_t fl2440adc_read (struct file *filp, char __user * userbuf, size_t count, loff_t * foffset)
{
    unsigned int channel;
    unsigned int ret_val;

    *foffset = 0;
    channel = (unsigned int)(filp->private_data);
    DPRINTK("Read ADC, Channel = [%d]\n", channel);

    mutex_lock(&adc_dev.mutex);
    ret_val = s3c_adc_read(adc_dev.client[channel], channel);  // 读取转换后的数据
    mutex_unlock(&adc_dev.mutex);

    if (IS_ERR_VALUE(ret_val))
    {
        DPRINTK("Read failed for ADC channel [%d]\n", channel);
        goto RESLOCK_RTN;
    }

    ret_val = copy_to_user(userbuf, &ret_val, count); 
    if (ret_val)
    {
        DPRINTK("Copy ADC:[%d] result to userspace failed...\n", channel);
    }
    ret_val = sizeof(unsigned int) - ret_val;

RESLOCK_RTN:
    return ret_val; // 
}

static int  fl2440adc_release(struct inode *inodp, struct file *filp)
{
    int channel;

    DPRINTK("File operations release...\n");
    channel = MINOR(inodp->i_rdev);
    mutex_lock(&adc_dev.mutex);

    if(!(atomic_add_unless(&adc_dev.client_ref[channel], -1, 0)))
    {
        s3c_adc_release(adc_dev.client[channel]);
        adc_dev.client[channel] = NULL;
    }
    mutex_unlock(&adc_dev.mutex);

    return 0;
}


static struct file_operations fl2440adc_fops = {
    .owner = THIS_MODULE,
    .open = fl2440adc_open,
    .read = fl2440adc_read,
    .release = fl2440adc_release,
};


static int fl2440adc_probe (struct platform_device *platform_dev)
{
    int ret_val;
    int minor;

    minor = platform_dev->id;

    /* ----------------------- */
    cdev_init(&adc_dev.fcdev[minor], &fl2440adc_fops);  
    ret_val = cdev_add(&adc_dev.fcdev[minor], MKDEV(MAJOR(adc_dev.devno), minor), 1);
    if (IS_ERR_VALUE(ret_val))
    {
        DPRINTK("Add chrdev [%d] failed...\n", minor);
        return ret_val;
    }
    adc_dev.cls_dev[minor] = device_create(adc_dev.dev_class, adc_dev.pltdev[minor]->dev.parent, 
            MKDEV(MAJOR(adc_dev.devno), minor), NULL, ADCDEVNAME "%d", minor);

    if (IS_ERR(adc_dev.cls_dev[minor]))
    {
        DPRINTK("Create class device [%d] failed...\n", minor);
        ret_val = PTR_ERR(adc_dev.cls_dev[minor]);
        cdev_del(&adc_dev.fcdev[minor]);
        adc_dev.cls_dev[minor] = NULL;
        return ret_val;
    }

    DPRINTK("Pltdev [%d] Probe complete\n", minor);

    return 0;
}

static int fl2440adc_remove(struct platform_device *platform_dev)
{
    int minor;
    
    minor = platform_dev->id;
    cdev_del(&adc_dev.fcdev[minor]);
    device_destroy(adc_dev.dev_class, MKDEV(MAJOR(adc_dev.devno), minor)); 

    return 0;
}

struct platform_driver plt_dri = {
    .probe = fl2440adc_probe,
    .remove = fl2440adc_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = ADCDRINAME,
    },
};

static int __init fl2440adc_init(void)
{
    int ret_val;
    int loopidx;

    memset(&adc_dev, 0, sizeof(adc_dev));

    ret_val = alloc_chrdev_region(&adc_dev.devno, 0, ADCNUM, ADCDEVNAME);
    if (IS_ERR_VALUE(ret_val))
    {
        DPRINTK("Request Major failed...\n");
        return ret_val;
    }

    adc_dev.dev_class = class_create(THIS_MODULE, ADCDEVNAME);
    if (IS_ERR(adc_dev.dev_class))
    {
        DPRINTK("Create device class failed...\n");
        ret_val = PTR_ERR(adc_dev.dev_class);
        goto UNREG_DEVNO;
    }

    adc_dev.pltdev = fl2440adc_register_device();
    if (unlikely(!adc_dev.pltdev))
    {
        DPRINTK("Create platform ADC devices failed...\n");
        ret_val = -ENOMEM;
        goto RM_DEV_CLASS;
    }
    DPRINTK("Create platform ADC devices completed...\n");

    adc_dev.pltdri = &plt_dri;
    ret_val = platform_driver_register(adc_dev.pltdri);
    if (IS_ERR_VALUE(ret_val))
    {
        DPRINTK("Create platform ADC driver failed...\n");
        goto UNREG_PLTDEV;
    }

    DPRINTK("Create platform ADC driver completed...\n");
    
    mutex_init(&adc_dev.mutex);
    for (loopidx = 0; loopidx < ADCNUM; loopidx++)
    {
        atomic_set(&adc_dev.client_ref[loopidx], 0);
    }

    return 0;

UNREG_PLTDEV:
    fl2440adc_unregister_device(adc_dev.pltdev);

RM_DEV_CLASS:
    class_destroy(adc_dev.dev_class);

UNREG_DEVNO:
    unregister_chrdev_region(adc_dev.devno, ADCNUM);


    return ret_val;
}

static void __exit fl2440adc_exit(void)
{
    fl2440adc_unregister_device(adc_dev.pltdev);
    class_destroy(adc_dev.dev_class);
    platform_driver_unregister(adc_dev.pltdri);
    unregister_chrdev_region(adc_dev.devno, ADCNUM);
    mutex_destroy(adc_dev.mutex);
}

module_init(fl2440adc_init);
module_exit(fl2440adc_exit);
MODULE_AUTHOR("chen-qx@live.cn");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("FL2440 ADC drivers based s3c-adc");


#endif // CONFIG_S3C_ADC
