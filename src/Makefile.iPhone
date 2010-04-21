
PREFIX=/home/david/Projects/iphone/toolchain/toolchain/pre/bin/arm-apple-darwin9-
#BASE_DEV=/home/david/Projects/iphone/toolchain/sdks/iPhoneOS3.1.2.sdk
BASE_DEV=/home/david/Projects/iphone/toolchain/sdks/iPhoneOS2.0.sdk

BUILD_APP = iXpectrum

CC    := $(PREFIX)gcc
LD    := $(PREFIX)gcc
AS    := $(PREFIX)as
AR    := $(PREFIX)ar

OBJECTS =   IPhone/iph_main.o   \
            IPhone/Bootstrapper.o   \
            IPhone/NowPlayingController.o   \
            IPhone/HelpController.o \
            IPhone/OptionsController.o \
            IPhone/helpers.o   \
            IPhone/MagnifierView.o   \
            main.o                          \
            font.o                          \
            IPhone/microlib.o               \
            cpu/z80.o                       \
            graphics.o                      \
            zx.o                            \
            ay8910.o                        \
            fdc.o                           \
            snaps.o                         \
            player.o                        \
	        bzip/blocksort.o                \
	        bzip/huffman.o                  \
	        bzip/crctable.o                 \
	        bzip/randtable.o                \
	        bzip/compress.o                 \
	        bzip/decompress.o               \
	        bzip/bzlib.o                    \
 	        mylibspectrum/tzx_read.o        \
 	        mylibspectrum/tape.o            \
 	        mylibspectrum/tape_block.o      \
 	        mylibspectrum/myglib.o          \
	        mylibspectrum/tap.o             \
	        mylibspectrum/tape_set.o        \
	        mylibspectrum/symbol_table.o    \
	        mylibspectrum/libspectrum.o     \
	        mylibspectrum/zlib.o            \
	        mylibspectrum/tape_accessors.o  \
	        zxtape.o
 
 #           usbjoy.o                        \
 #           usbkeyb.o                       \

LDFLAGS = -lobjc -lpthread -bind_at_load 
LDFLAGS += -framework Foundation -framework CoreFoundation -framework CoreSurface -framework UIKit  -framework QuartzCore -framework CoreGraphics -framework AudioToolbox    
LDFLAGS += -lm -lc  -lz 
#-lzip      
LDFLAGS += -F$(BASE_DEV)/System/Library/Frameworks
LDFLAGS += -F$(BASE_DEV)/System/Library/PrivateFrameworks
lDFLAGS += -L$(BASE_DEV)/usr/lib 

CFLAGS = -march=armv6 -DARM_ARCH -DIPHONE
CFLAGS += -I. -Icpu -Iincludes
CFLAGS += -I$(BASE_DEV)/usr/lib/gcc/arm-apple-darwin9/4.0.1/include 
#CFLAGS += -isysroot $(BASE_DEV)
#CFLAGS += -O2
CFLAGS += -O3 -ffast-math 
#CFLAGS += -DDEBUG_MSG 
CFLAGS += -DGP2X -DSOUND_X128

all: $(BUILD_APP)

$(BUILD_APP): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

%.o:/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.m
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.s
	$(CC) -c $(CFLAGS) $< -o $@

	
clean:
	rm -f $(OBJECTS) $(BUILD_APP)

release: clean all
