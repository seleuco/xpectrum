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
//#import <UIKit/UIView-Geometry.h>
//#import <GraphicsServices/GraphicsServices.h>
#import <Foundation/Foundation.h>
#import "CoreSurface/CoreSurface.h"
#import <QuartzCore/CALayer.h>

#import <pthread.h>
#import <sched.h>
#import <unistd.h>
#import <sys/time.h>

#import "MagnifierView.h"

struct myGSPathPoint {
	char unk0;
	char unk1;
	char unk2;
	char unk3;
	char unk4;
	char unk5;
	char status;
	char unk7;
	char unk8;
	char unk9;
	char unk10;
	char unk11;
	char unk12;
	char unk13;
	char unk14;
	char unk15;
	float x;
	float y;
};

typedef struct {
	char unk0;
	char unk1;
	char unk2;
	char unk3;
	char unk4;
	char unk5;
	char unk6;
	char unk7;
	
	char unk8;
	char unk9;
	char unk10;
	char unk11;
	char unk12;
	char unk13;
	char unk14;
	char unk15;
	
	char unk16;
	char unk17;
	char unk18;
	char unk19;
	char unk20;
	char unk21;
	char unk22;
	char unk23;
	
	char unk24;
	char unk25;
	char unk26;
	char unk27;
	char unk28;
	char unk29;
	char unk30;
	char unk31;
	
	char unk32;
	char unk33;
	char unk34;
	char unk35;
	char unk36;
	char unk37;
	char unk38;
	char unk39;
	
	char unk40;
	char unk41;
	char unk42;
	char unk43;
	char unk44;
	char unk45;
	char unk46;
	char unk47;
	
	char unk48;
	char unk49;
	char unk50;
	char unk51;
	char unk52;
	char unk53;
	char unk54;
	char unk55;
	
	char type;
	char unk57;
	char unk58;
	char unk59;
	char unk60;
	char unk61;
	char fingerCount;
	char unk62;	
	char unk63;
	char unk64;
	char unk65;
	char unk66;
	char unk67;
	char unk68;
	char unk69;
	char unk70;
	char unk71;
	char unk72;
	char unk73;
	char unk74;
	char unk75;
	char unk76;
	char unk77;
	char unk78;
	char unk79;
	char unk80;
	char unk81;
	char unk82;
	char unk83;
	char unk84;
	char unk85;
	char unk86;
	struct myGSPathPoint points[100];
} myGSEvent;


@interface ScreenView : UIView
{
	CALayer						    *screenLayer;
}

- (id)initWithFrame:(CGRect)frame;
- (void)drawRect:(CGRect)rect;
- (void)updateScreen;


@end

@interface NowPlayingController : UIViewController <UIActionSheetDelegate> 
{

  UIWindow		* window;
  UIView			* screenView;
  UIImageView	    * imageView;
  MagnifierView   *loop;
  NSTimer *touchTimer;

  //joy controller
  CGRect ButtonUp;
  CGRect ButtonLeft;
  CGRect ButtonDown;
  CGRect ButtonRight;
  CGRect ButtonUpLeft;
  CGRect ButtonDownLeft;
  CGRect ButtonUpRight;
  CGRect ButtonDownRight;
  CGRect Up;
  CGRect Left;
  CGRect Down;
  CGRect Right;
  CGRect UpLeft;
  CGRect DownLeft;
  CGRect UpRight;
  CGRect DownRight;
  CGRect Select;
  CGRect Start;
  CGRect LPad;
  CGRect RPad;
  CGRect LPad2;
  CGRect RPad2;
  CGRect Menu;

  CGRect rShowKeyboard;

  //specy keyboard
  CGRect rSPECKEY_1;
  CGRect rSPECKEY_2;
  CGRect rSPECKEY_3;
  CGRect rSPECKEY_4;
  CGRect rSPECKEY_5;
  CGRect rSPECKEY_6;
  CGRect rSPECKEY_7;
  CGRect rSPECKEY_8;
  CGRect rSPECKEY_9;
  CGRect rSPECKEY_0;

  CGRect rSPECKEY_Q;
  CGRect rSPECKEY_W;
  CGRect rSPECKEY_E;
  CGRect rSPECKEY_R;
  CGRect rSPECKEY_T;
  CGRect rSPECKEY_Y;
  CGRect rSPECKEY_U;
  CGRect rSPECKEY_I;
  CGRect rSPECKEY_O;
  CGRect rSPECKEY_P;

  CGRect rSPECKEY_A;
  CGRect rSPECKEY_S;
  CGRect rSPECKEY_D;
  CGRect rSPECKEY_F;
  CGRect rSPECKEY_G;
  CGRect rSPECKEY_H;
  CGRect rSPECKEY_J;
  CGRect rSPECKEY_K;
  CGRect rSPECKEY_L;
  CGRect rSPECKEY_ENTER;

  CGRect rSPECKEY_SHIFT;
  CGRect rSPECKEY_SHIFT2;
  CGRect rSPECKEY_Z;
  CGRect rSPECKEY_X;
  CGRect rSPECKEY_C;
  CGRect rSPECKEY_V;
  CGRect rSPECKEY_B;
  CGRect rSPECKEY_N;
  CGRect rSPECKEY_M;
  CGRect rSPECKEY_SYMB;
  CGRect rSPECKEY_SYMB2;
  CGRect rSPECKEY_SPACE;

  CGRect rShowController;
  CGRect rHideKeyboard;
  CGRect rHideKeyboard2;

  CoreSurfaceBufferRef			_screenSurface;
  CALayer						    *screenLayer;
  //NSTimer						*	timer;
}

- (void)getControllerCoords:(int)orientation;
- (void)getKeyboardCoords:(int)orientation;
//- (void)mouseEvent:(myGSEvent*)event;

- (void)startEmu:(char*)path;
- (void)runMenu;
- (void)runMainMenu;
- (void)buildPortraitImageView;
- (void)buildPortrait;
- (void)buildLandscapeImageView;
- (void)buildLandscape;
- (void)showKey:(CGRect)rect;
- (void)hideKey;
- (void) handleAction:(id)timerObj;
- (void)touchesKeyboard:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesController:(NSSet *)touches withEvent:(UIEvent *)event;

@end
