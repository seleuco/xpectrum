
TARGET = gp2xpectrum.gpe
PREFIX := arm-linux-
CC    := $(PREFIX)gcc
LD    := $(PREFIX)gcc
AS    := $(PREFIX)as
AR    := $(PREFIX)ar


OBJECTS = main.o cpuctrl.o font.o flush_uppermem_cache.o usbjoy.o usbkeyb.o microlib.o  \
	 cpu/z80.o graphics.o zx.o ay8910.o fdc.o snaps.o player.o \
	 bzip/blocksort.o bzip/huffman.o bzip/crctable.o bzip/randtable.o bzip/compress.o bzip/decompress.o bzip/bzlib.o \
 	 mylibspectrum/tzx_read.o  mylibspectrum/tape.o  mylibspectrum/tape_block.o mylibspectrum/myglib.o \
	 mylibspectrum/tap.o mylibspectrum/tape_set.o mylibspectrum/symbol_table.o \
	 mylibspectrum/libspectrum.o mylibspectrum/zlib.o mylibspectrum/tape_accessors.o zxtape.o
	 
CFLAGS = -DGP2X -D__ARM__ -DSOUND_X128 -I. -Icpu/ -Iincludes/  -II:/dev/gp2x/devkitGP2X/lib/

CFLAGS += -O7
LDFLAGS =     -lm -lc -lrt I:/dev/gp2x/devkitGP2X/lib/libzip.a  I:/dev/gp2x/devkitGP2X/lib/libz.a 
#LDFLAGS =   -lm -lpthread -static  I:\dev\gp2x\devkitGP2X/lib/libzip.a I:\dev\gp2x\devkitGP2X/lib/libz.a

all: $(TARGET)

$(TARGET): $(OBJECTS)

	$(CC) -o $@ $(OBJECTS) $(LDFLAGS) 
	#gpecomp $(TARGET) gp2xpectrum.gpe

%.o:/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.s
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	

release: clean all


