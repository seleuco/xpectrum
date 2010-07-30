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

/*

iPhone Simulator == i386
iPhone == iPhone1,1
3G iPhone == iPhone1,2
3GS iPhone == iPhone2,1
1st Gen iPod == iPod1,1
2nd Gen iPod == iPod2,1

1st Gen iPad == iPad1,1
iPhone 4 == iPhone3,1

I imagine the iPod Touch 4 will be == iPod3,1
and the 2011 next generation iPad will be == iPad2,1

 */

#import <UIKit/UIKit.h>

@interface Helper : NSObject
+ (NSString *)machine;
@end

extern const char* get_resource_path(char* file);
extern const char* get_documents_path(char* file);





