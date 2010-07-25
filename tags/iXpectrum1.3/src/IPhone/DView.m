
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
 
*/

#import "DView.h"

#import <QuartzCore/QuartzCore.h>  

extern controller;

extern int isIpad;

@implementation DView


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
	//printf("draw dview\n");
	
	//TEST CODE
    
   // printf("Drawing Rect");
    
    	//Get the CGContext from this view
	CGContextRef context = UIGraphicsGetCurrentContext();
	 
   	//Set the stroke (pen) color
	CGContextSetStrokeColorWithColor(context, [UIColor redColor].CGColor);
	//Set the width of the pen mark
	CGContextSetLineWidth(context, 2.0);
	
    int i=0;
    
    if(controller)
    {
	    for(i=0; i<ndrects;i++)
		   CGContextStrokeRect(context, drects[i]);
	}
	else
	{
	    for(i=0; i<ndrects2;i++)
		   CGContextStrokeRect(context, drects2[i]);
	}  

	//CGContextAddRect(context, drects[1]);
	//Draw it
	CGContextFillPath(context);
	       
    CGContextSelectFont(context, "Helvetica", 16, kCGEncodingMacRoman); 
    CGContextSetTextDrawingMode (context, kCGTextFillStroke);
    CGContextSetRGBFillColor (context, 0, 5, 0, .5);
    CGRect viewBounds = self.bounds;
    CGContextTranslateCTM(context, 0, viewBounds.size.height);
    CGContextScaleCTM(context, 1, -1);
    CGContextSetRGBStrokeColor (context, 0, 1, 1, 1);
 
   if(!isIpad)
   {
      CGContextShowTextAtPoint(context, 10, 10, "Es un iPhone",12 );
   }
   else
   {
      CGContextShowTextAtPoint(context, 10, 10, "Es un iPad",10 );
   }
		
}


- (void)dealloc {

    [super dealloc];
}


@end
