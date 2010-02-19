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

#import "HelpController.h"
#include <stdio.h>


@implementation HelpController

@synthesize bIsDismissed;

- (id)init {

    if (self = [super init]) {

        bIsDismissed = NO;
    }

    return self;

}

- (void)loadView {

	struct CGRect rect = [[UIScreen mainScreen] bounds];
	rect.origin.x = rect.origin.y = 0.0f;
	self.view = [[UIView alloc] initWithFrame:rect];
    self.view.backgroundColor = [UIColor whiteColor];
    
   navBar = [ [ UINavigationBar alloc ] initWithFrame: CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, 45.0f)];
   [ navBar setDelegate: self ];

   UINavigationItem *item = [[ UINavigationItem alloc ] initWithTitle:@"Credits & Help" ];
   UIBarButtonItem *backButton = [[[UIBarButtonItem alloc] initWithTitle:@"OK" style:UIBarButtonItemStyleBordered target:[self parentViewController]  action:  @selector(done:) ] autorelease];
   item.rightBarButtonItem = backButton;
   [ navBar pushNavigationItem: item  animated:YES];
     
   [ self.view addSubview: navBar ];
    
    
    FILE *file;
    char buffer[262144], buf[1024];

    textView = [ [ UITextView alloc ] initWithFrame: CGRectMake(rect.origin.x, rect.origin.y + 45.0f, rect.size.width,rect.size.height - 45.0f )];
//        [ textView setTextSize: 12 ];

    textView.font = [UIFont fontWithName:@"Courier New" size:14.0];


    textView.editable = NO;

    file = fopen("/Applications/iXpectrum.app/readme.txt", "r");

    if (!file) 
    {        
            textView.textColor =  [UIColor redColor];            
            [ textView setText: @"ERROR: File not found" ];
            
    } else 
    {
            buffer[0] = 0;
            while((fgets(buf, sizeof(buf), file))!=NULL) {
                strlcat(buffer, buf, sizeof(buffer));
            }
            fclose(file);

            [ textView setText: [ [ NSString alloc ] initWithCString: buffer ]];
    }

    [ self.view addSubview: textView ];

}

-(void)viewDidLoad{	

}



-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
       return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning];
}


- (void)dealloc {
   
	[super dealloc];
}

@end