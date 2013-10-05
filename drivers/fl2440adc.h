
/* 
 * This file is part of s3c2440adc.
 */


#ifndef FL2440_ADC_H__
#define FL2440_ADC_H__

#include <linux/platform_device.h>

#ifndef ADCDEVNAME 
#define ADCDEVNAME "fl2440adc"
#endif 

#ifndef ADCDRINAME 
#define ADCDRINAME ADCDEVNAME
#endif 

#define ADCNUM  4    /* 实际支持 8 路, 但有四路预留给 TS 了 */
#define ADCCH0  0
#define ADCCH1  1
#define ADCCH2  2
#define ADCCH3  3
#define ADCCH4  4
#define ADCCH5  5
#define ADCCH6  6
#define ADCCH7  7

#define DPRINTK(fmt, args...) printk(KERN_ERR fmt, ##args)


extern struct platform_device s3c_device_adc;

struct platform_device ** fl2440adc_register_device(void);
void fl2440adc_unregister_device(struct platform_device **);



#endif //  FL2440_ADC_H__
