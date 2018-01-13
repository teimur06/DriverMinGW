OBJECTS = DriverMinGW.o
TARGET = DriverMinGW.sys
CFLAGS = -I D:\mingw64\x86_64-w64-mingw32\include\ddk -Wall -m64 -shared 
CC = gcc

all: $(TARGET)

clean:
	cmd //C del $(OBJECTS) $(TARGET) *.map
	
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 
	
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -Wl,--subsystem,native \
	-Wl,--image-base,0x10000 \
	-Wl,--file-alignment,0x1000 \
	-Wl,--section-alignment,0x1000 \
	-Wl,--entry,DriverEntry@8 \
	-Wl,--stack,0x40000 \
	-Wl,--dynamicbase \
	-Wl,--nxcompat \
	-Xlinker -Map=$(basename $<).map 	\
	-nostartfiles -nostdlib -o $(TARGET) \
	$(OBJECTS) -lntoskrnl -lhal