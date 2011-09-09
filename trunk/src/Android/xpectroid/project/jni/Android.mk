# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_LDLIBS    := -ldl -llog
LOCAL_MODULE    := xpectroid-jni
LOCAL_SRC_FILES := xpectroid-jni.c

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
#LOCAL_PATH := $(call my-dir)
LOCAL_PATH := /home/david/Projects/iphone/toolchain/apps/iXpectrum

MY_SOURCE_PATH := /home/david/Projects/iphone/toolchain/apps/iXpectrum
LOCAL_MODULE    := xpectrum
LOCAL_CFLAGS    := -DANDROID -DGP2X -DSOUND_X128 -O3 -ffast-math -DUSE_ZLIB
LOCAL_LDLIBS    := -ldl -lc -llog -lgcc -lm -lz 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cpu
LOCAL_C_INCLUDES += $(LOCAL_PATH)/includes 
LOCAL_SRC_FILES :=  main.c                  \
            font.c                          \
            Android/microlib.c              \
            cpu/z80.c                       \
            graphics.c                      \
            zx.c                            \
            ay8910.c                        \
            fdc.c                           \
            snaps.c                         \
            player.c                        \
            minizip/unzip.c                 \
            minizip/ioapi.c                 \
	    bzip/blocksort.c                \
	    bzip/huffman.c                  \
	    bzip/crctable.c                 \
	    bzip/randtable.c                \
	    bzip/compress.c                 \
	    bzip/decompress.c               \
	    bzip/bzlib.c                    \
 	    mylibspectrum/tzx_read.c        \
 	    mylibspectrum/tape.c            \
 	    mylibspectrum/tape_block.c      \
 	    mylibspectrum/myglib.c          \
	    mylibspectrum/tap.c             \
	    mylibspectrum/tape_set.c        \
	    mylibspectrum/symbol_table.c    \
	    mylibspectrum/libspectrum.c     \
	    mylibspectrum/zlib.c            \
	    mylibspectrum/tape_accessors.c  \
	    zxtape.c


include $(BUILD_SHARED_LIBRARY)
