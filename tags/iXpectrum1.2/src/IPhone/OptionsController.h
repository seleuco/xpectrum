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

#import <UIKit/UIKit.h>

@interface Options : NSObject
{
   NSMutableArray*	  optionsArray;
   @public  int keepAspectRatio;
   @public  int smoothedPort;
   @public  int smoothedLand;
   @public  int safeRenderPath;
}

- (void)loadOptions;
- (void)saveOptions;

@property (readwrite,assign) int keepAspectRatio;
@property (readwrite,assign) int smoothedPort;
@property (readwrite,assign) int smoothedLand;
@property (readwrite,assign) int safeRenderPath;

@end


@interface OptionsController : UIViewController
{

   UINavigationBar    * navBar;
   UISwitch*		  switchKeepAspect;
   UISwitch*		  switchSmoothedPort;
   UISwitch*		  switchSmoothedLand;
   UISwitch*		  switchSafeRender;
}

- (void)optionChanged:(id)sender;
- (void)setOptions;

@end
