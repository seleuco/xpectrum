/* 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
   
   Copyright (c) 2010 Seleuco. Based in ZodTTD code.
   
*/

#import "Bootstrapper.h"
#import "helpers.h"
#include <stdio.h>

int isIpad = 0;

@implementation Bootstrapper


-(void)applicationDidFinishLaunching:(NSNotification *)aNotification {

	struct CGRect rect = [[UIScreen mainScreen] bounds];
	rect.origin.x = rect.origin.y = 0.0f;
	
	//printf("Machine: '%s'\n",[[Helper machine] UTF8String]) ;
	
	mkdir("/var/mobile/Media/ROMs/iXpectrum/downloads", 0755);
	mkdir("/var/mobile/Media/ROMs/iXpectrum/saves", 0755);

    [[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
    
	[[UIApplication sharedApplication] setIdleTimerDisabled:YES];//TODO ???
	/*
	BOOL iPad = NO;
	UIDevice* dev = [UIDevice currentDevice];
    if ([dev respondsToSelector:@selector(isWildcat)])
    {
       iPad = [dev isWildcat];
    }
	
	isIpad = iPad != NO;
	*/
	
	isIpad = [[Helper machine] rangeOfString:@"iPad"].location != NSNotFound;
	//isIpad = 1;

	hrViewController = [[NowPlayingController alloc] init];
	
	window = [[UIWindow alloc] initWithFrame:rect];
	window.backgroundColor = [UIColor redColor];
	[window addSubview: hrViewController.view ];
	
	[window makeKeyAndVisible];
		
	[hrViewController startEmu:"/kk"];

}

-(void)dealloc {
    [hrViewController release];
	[window dealloc];
	[super dealloc];
}

@end