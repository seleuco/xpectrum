
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
 
   Created by Sean Christmann on 12/22/08. Adapted by Seleuco.
*/

#import "MagnifierView.h"

#import <QuartzCore/QuartzCore.h>  

extern CGRect rLoopImageMask;

@implementation MagnifierView
@synthesize viewref, touchPoint;

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
		self.backgroundColor = [UIColor clearColor];
		self.multipleTouchEnabled = NO;
	    self.userInteractionEnabled = NO;
	
	   
    }
    return self;
}


- (void)drawRect:(CGRect)rect {
	//printf("draw magnifying glass\n");
	if(cachedImage == nil){
		UIGraphicsBeginImageContext(self.bounds.size);
		[self.viewref.layer renderInContext:UIGraphicsGetCurrentContext()];
		cachedImage = [UIGraphicsGetImageFromCurrentImageContext() retain];
		UIGraphicsEndImageContext();
	}
	CGImageRef imageRef = [cachedImage CGImage];
	CGImageRef maskRef = [[UIImage imageNamed:@"loopmask.png"] CGImage];
	CGImageRef overlay = [[UIImage imageNamed:@"loop.png"] CGImage];
	CGImageRef mask = CGImageMaskCreate(CGImageGetWidth(maskRef), 
										CGImageGetHeight(maskRef),
										CGImageGetBitsPerComponent(maskRef), 
										CGImageGetBitsPerPixel(maskRef),
                                        CGImageGetBytesPerRow(maskRef), 
										CGImageGetDataProvider(maskRef), 
										NULL, 
										true);
										
										
										
	//Create Mask
	//CGImageRef subImage = CGImageCreateWithImageInRect(imageRef, CGRectMake(touchPoint.x-15, touchPoint.y-15, 30, 30));
	CGImageRef subImage = CGImageCreateWithImageInRect(imageRef, CGRectMake(touchPoint.x-rLoopImageMask.origin.x, 
	  touchPoint.y-rLoopImageMask.origin.y, rLoopImageMask.size.width, rLoopImageMask.size.height));
	
	
	//CGImageRef subImage = CGImageCreateWithImageInRect(imageRef, CGRectMake(touchPoint.x-18, touchPoint.y-18, 36, 36));
	//CGImageRef subImage = CGImageCreateWithImageInRect(imageRef, CGRectMake(touchPoint.x-25, touchPoint.y-25, 50, 50));
	CGImageRef xMaskedImage = CGImageCreateWithMask(subImage, mask);
	
	// Draw the image
	// Retrieve the graphics context
	CGContextRef context = UIGraphicsGetCurrentContext();
	CGAffineTransform xform = CGAffineTransformMake(
													1.0,  0.0,
													0.0, -1.0,
													0.0,  0.0);
													
	CGContextConcatCTM(context, xform);
	CGRect area = CGRectMake(touchPoint.x-42, -touchPoint.y, 85, 85);
	CGRect area2 = CGRectMake(touchPoint.x-40, -touchPoint.y+2, 80, 80);
		
	CGContextDrawImage(context, area2, xMaskedImage);
	CGContextDrawImage(context, area, overlay);
}


- (void)dealloc {
	[cachedImage release];
	[viewref release];
    [super dealloc];
}


@end
