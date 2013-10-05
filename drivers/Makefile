
obj-m	:= fl2440adc.o
fl2440adc-objs	:= fl2440adcdev.o fl2440adcdri.o

KERNELPATH=/opt/embedded/build/linux-2.6.34.14
CURRPATH=`pwd`


all:
	make -C $(KERNELPATH) M=$(CURRPATH) modules


.phony: clean

clean:
	make -C $(KERNELPATH) M=$(CURRPATH) clean
