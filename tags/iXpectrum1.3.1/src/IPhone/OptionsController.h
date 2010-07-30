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

   @public  int tvFilterPort;
   @public  int tvFilterLand;
   @public  int scanlineFilterPort;
   @public  int scanlineFilterLand;

   @public  int cropBorderPort;
   @public  int cropBorderLand;
}

- (void)loadOptions;
- (void)saveOptions;

@property (readwrite,assign) int keepAspectRatio;
@property (readwrite,assign) int smoothedPort;
@property (readwrite,assign) int smoothedLand;
@property (readwrite,assign) int safeRenderPath;

@property (readwrite,assign) int tvFilterPort;
@property (readwrite,assign) int tvFilterLand;

@property (readwrite,assign) int scanlineFilterPort;
@property (readwrite,assign) int scanlineFilterLand;

@property (readwrite,assign) int cropBorderPort;
@property (readwrite,assign) int cropBorderLand;

@end


@interface OptionsController : UIViewController  <UITableViewDelegate, UITableViewDataSource>
{

   //UINavigationBar    * navBar;
   UISwitch*		  switchKeepAspect;
   UISwitch*		  switchSmoothedPort;
   UISwitch*		  switchSmoothedLand;
   UISwitch*		  switchSafeRender;

   UISwitch*		  switchTvFilterPort;
   //UISlider*		  sliderTvFilterPort;
   UISwitch*		  switchScanlineFilterPort;
   //UISlider*		  sliderScanlineFilterPort;

   UISwitch*		  switchTvFilterLand;
   UISwitch*		  switchScanlineFilterLand;

   UISwitch*		  switchCropBorderLand;
   UISwitch*		  switchCropBorderPort;

}

- (void)optionChanged:(id)sender;
//- (void)setOptions;

@end
