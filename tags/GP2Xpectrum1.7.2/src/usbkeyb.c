/* Title: USB Keyboard library
   Version 0.0.5 WIP
   Written by Metalbrain (metalbrain_coder@gmx.net), (c) 2008.
   
   If you use this library or a part of it, please, let it know.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdio.h>		/* For the definition of NULL */
#include <sys/types.h>	        // For Device open
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>		// For Device read

#include <dirent.h>
#include <string.h>
#include <limits.h>		/* For the definition of PATH_MAX */
#include <linux/input.h> // For ease of use

#include "usbkeyb.h"

int keyboard_fd = -1;
int keyboardmap[128];

int CheckKeyboard(char * filePath)
	{
	FILE * device_file;
	int device_file_n;
	device_file = fopen(filePath, "r");
	if (device_file==NULL)
		{
		return (-1);	// Can't read device
		}
	device_file_n=fileno(device_file);

	unsigned int keyboardevents = (1<<EV_KEY)|(1<<EV_LED)|(1<<EV_REP);		/*(1<<EV_SYN)|*/
	unsigned int keyboardleds = (1<<LED_CAPSL);	/*(1<<LED_NUML)|*/ /*|(1<<LED_SCROLLL)*/

	unsigned int evtype_bitmask=0;
	if (ioctl(device_file_n, EVIOCGBIT(0, EV_MAX), &evtype_bitmask) < 0) 
		{
		return(-1); // Can't do ioctl to check bitmask
		}

	unsigned int led_bitmask = 0;
	if (ioctl(device_file_n, EVIOCGBIT(EV_LED, sizeof(led_bitmask)), &led_bitmask) <0)
		{
		return(-1); // Can't do ioctl to check led bitmask
		}

	if (((evtype_bitmask&keyboardevents)==keyboardevents) && ((led_bitmask&keyboardleds)==keyboardleds))
		{
		return(0);  // Keyboard Found!
		}
	return(-1);	// Dispositivo no teclado
	}


int findUSBkeyboard()
	{
	int found=0;
	DIR * dir_;
	dir_ = opendir("/dev/input");
	int i;
	for (i=0;i<128;i++)
		{
		keyboardmap[i]=0;
		}
	if(dir_==NULL)
		{
		return (-1);	// Error can't open /dev/input directory to find devices
		}
	rewinddir(dir_);
	struct dirent * entry = readdir(dir_);
	for (;entry!=0; entry=readdir(dir_))
		{
		if (strncmp(entry->d_name,"event",5)==0)	// Find all /dev/input/event* devices
			{
			char devstr[25];			// Should be enough
			sprintf(devstr,"/dev/input/%s",entry->d_name);
			int fd = open(devstr, O_RDONLY);
			if (fd !=-1)
				{
				struct input_id device_info;
				memset(&device_info, 0, sizeof(struct input_id));
				if(!ioctl(fd,EVIOCGID, &device_info))
					{
					int value;
					int ioctloutput;
					value=CheckKeyboard(devstr);	// Check if device is a keyboard
					if(value==0)
						{
						found=1;
						keyboard_fd=fd;		// Keyboard detected and open, keep file descriptor
						}
					}
				if(keyboard_fd!=fd)
					close(fd);	// Close anything that's not a keyboard
				}
			}
		}
	closedir(dir_);
	return found;
	}

int readUSBkeyboard()
	{
    	int yalv;           /* loop counter */
    	size_t read_bytes;  /* how many bytes were read */
    	struct input_event ev[64]; /* the events (up to 64 at once) */
	fd_set set;
	struct timeval timeout;

	read_bytes=0;
	FD_ZERO (&set);
	FD_SET (keyboard_fd, &set);
  	timeout.tv_sec = 0;
  	timeout.tv_usec = 0;		// no timeout

	int ready = select(FD_SETSIZE,&set, NULL, NULL,&timeout);
    	if(ready>0)
    		{
		read_bytes = read(keyboard_fd,ev, sizeof(struct input_event)*64);
	    	}
    	if (read_bytes<0)
    		{
		return 0;	// Error reading, so keyboard is probably disconnected.
    		}
	for (yalv = 0; yalv < (int) (read_bytes / sizeof(struct input_event)); yalv++)
	    	{
	    	if (ev[yalv].type==1)
	    		{
	    		if (ev[yalv].value==0)
	    			{
	    			keyboardmap[ev[yalv].code]=0;
	    			}
			else if (ev[yalv].value==1)
				{
	    			keyboardmap[ev[yalv].code]=1;
				}
	    		}
	    	}
	return 1;
	}
