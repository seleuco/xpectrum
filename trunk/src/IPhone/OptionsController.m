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

#import "OptionsController.h"
#include <stdio.h>
@implementation Options

@synthesize keepAspectRatio;
@synthesize smoothed;

- (id)init {

    if (self = [super init]) {
        [self loadOptions];
    }

    return self;
}


- (void)loadOptions
{
	NSString *path= @"/var/mobile/Media/ROMs/iXpectrum/options.bin";
	
	NSData *plistData;
	id plist;
	NSString *error;
	
	NSPropertyListFormat format;
	
	plistData = [NSData dataWithContentsOfFile:path];
		
	plist = [NSPropertyListSerialization propertyListFromData:plistData			 
											 mutabilityOption:NSPropertyListImmutable			 
													   format:&format
											 errorDescription:&error];
	
	if(!plist)
	{
		
		NSLog(error);
		
		[error release];
		
		optionsArray = [[NSMutableArray alloc] init];
		
		keepAspectRatio=1;
		smoothed=0;
		
		[self saveOptions];
	}
	else
	{
		optionsArray = [[NSMutableArray alloc] initWithArray:plist];
		
		keepAspectRatio = [[[optionsArray objectAtIndex:0] objectForKey:@"KeepAspect"] intValue];
		smoothed = [[[optionsArray objectAtIndex:0] objectForKey:@"Smoothed"] intValue];		
	}
		
}

- (void)saveOptions
{
	

	[optionsArray removeAllObjects];
	[optionsArray addObject:[NSDictionary dictionaryWithObjectsAndKeys:
							 [NSString stringWithFormat:@"%d", keepAspectRatio], @"KeepAspect",
							 [NSString stringWithFormat:@"%d", smoothed], @"Smoothed",
							 nil]];	

	
    NSString *path= @"/var/mobile/Media/ROMs/iXpectrum/options.bin";
	NSData *plistData;
	
	NSString *error;
		
	plistData = [NSPropertyListSerialization dataFromPropertyList:optionsArray				 
										     format:NSPropertyListBinaryFormat_v1_0				 
										     errorDescription:&error];	
	if(plistData)		
	{
		[plistData writeToFile:path atomically:NO];		
	}
	else
	{
	
		NSLog(error);		
		[error release];		
	}	
}

- (void)dealloc {
    
    [optionsArray dealloc];

	[super dealloc];
}

@end

@implementation OptionsController


- (id)init {

    if (self = [super init]) {

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

   UINavigationItem *item = [[ UINavigationItem alloc ] initWithTitle:@"Options" ];
   UIBarButtonItem *backButton = [[[UIBarButtonItem alloc] initWithTitle:@"OK" style:UIBarButtonItemStyleBordered target:[self parentViewController]  action:  @selector(done:) ] autorelease];
   item.rightBarButtonItem = backButton;
   [ navBar pushNavigationItem: item  animated:YES];
     
   [ self.view addSubview: navBar ];
   
   	UILabel *myLabel1 = [[UILabel alloc] initWithFrame:CGRectMake(10, 100, 200, 25)];
	myLabel1.text = @"Keep Aspect Ratio";
    [self.view addSubview:myLabel1];
   
    switchKeepAspect = [[UISwitch alloc] initWithFrame:CGRectMake(225 ,100, 0, 0)];  
    [ self.view addSubview: switchKeepAspect ];
    [switchKeepAspect addTarget:self action:@selector(optionChanged:) forControlEvents:UIControlEventValueChanged];
   
    UILabel *myLabel2 = [[UILabel alloc] initWithFrame:CGRectMake(10, 100+50, 200, 25)];
	myLabel2.text = @"Smoothed Screeen";
    [self.view addSubview:myLabel2];
   
    switchSmoothed = [[UISwitch alloc] initWithFrame:CGRectMake(225 ,100+50, 0, 0)];
    [ self.view addSubview: switchSmoothed ];
    [switchSmoothed addTarget:self action:@selector(optionChanged:) forControlEvents:UIControlEventValueChanged];
   
    [self setOptions];
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

- (void)optionChanged:(id)sender
{
    Options *op = [[Options alloc] init];
	    		
	op.keepAspectRatio = [switchKeepAspect isOn];
	op.smoothed =  [switchSmoothed isOn];
	
    [switchKeepAspect setOn: op.keepAspectRatio animated:NO];
	[switchSmoothed setOn: op.smoothed animated:NO];
	
	[op saveOptions];
		
	[op release];
}

- (void)setOptions
{
     Options *op = [[Options alloc] init];
	    	
     [switchKeepAspect setOn:[op keepAspectRatio] animated:NO];
	 [switchSmoothed setOn:[op smoothed] animated:NO];
		
	 [op release];
}

@end