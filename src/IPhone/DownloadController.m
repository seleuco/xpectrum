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
   
   Copyright (c) 2010 Seleuco.
   
*/

#import "DownloadController.h"

#include <stdio.h>

extern int isIpad;
extern int iphone_menu;

@implementation DownloadController

@synthesize bIsDismissed;

- (id)init {

    if (self = [super init]) {

        bIsDismissed = NO;
        //navBar = nil;
        aWebView = nil;
        isDownloading = 0;
    }

    return self;

}

- (void)loadView {

	struct CGRect rect = [[UIScreen mainScreen] bounds];
	rect.origin.x = rect.origin.y = 0.0f;
	UIView *view= [[UIView alloc] initWithFrame:rect];
	self.view = view;
	[view release];
    self.view.backgroundColor = [UIColor whiteColor];

    /*    
    UIToolbar *toolbar = [UIToolbar new];
    [toolbar autorelease];
    toolbar.barStyle = UIBarStyleDefault;
    [toolbar sizeToFit];

    //Set the frame
    CGFloat toolbarHeight = 45;//[toolbar frame].size.height;
    CGRect mainViewBounds = self.view.bounds;//self.parentViewController.view.bounds;
    [toolbar setFrame:CGRectMake(CGRectGetMinX(mainViewBounds), CGRectGetMinY(mainViewBounds) + CGRectGetHeight(mainViewBounds) - toolbarHeight, CGRectGetWidth(mainViewBounds),toolbarHeight)];

    //Create a button
    UIBarButtonItem *b1 = [[UIBarButtonItem alloc]    
        initWithTitle:@"  <  " style:UIBarButtonItemStyleBordered target:self action:@selector(buttonBack)];
    [b1 autorelease];

    UIBarButtonItem *b2 = [[UIBarButtonItem alloc]   
        initWithTitle:@"  >  " style:UIBarButtonItemStyleBordered target:self action:@selector(buttonForward)];
    [b2 autorelease];

    UIBarButtonItem *b3 = [[UIBarButtonItem alloc]   
        initWithTitle:@" Stop " style:UIBarButtonItemStyleBordered target:self action:@selector(buttonForward)];
    [b3 autorelease];
    
    UIBarButtonItem *b4 = [[UIBarButtonItem alloc]   
        initWithTitle:@"Reload" style:UIBarButtonItemStyleBordered target:self action:@selector(buttonForward)];
    [b4 autorelease];
    
    UIBarButtonItem *b5 = [[UIBarButtonItem alloc]   
        initWithTitle:@" Home " style:UIBarButtonItemStyleBordered target:self action:@selector(buttonForward)];
    [b5 autorelease];
    
    [toolbar setItems:[NSArray arrayWithObjects:b1,b2,b3,b4,b5,nil]];

    //Here we go
    [self.view addSubview:toolbar];
    */
    
   CGFloat navBarHeight = isIpad ? 45 : 55; 
    
   UINavigationBar *navBar = [ [ UINavigationBar alloc ] initWithFrame: CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, navBarHeight)];
   [navBar autorelease];
   [navBar setDelegate: self ];
      
   CGFloat width = self.view.frame.size.width;
  
   CGFloat buttonY = isIpad ? 10 : 25; 
  
   UIButton *button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
   [button setFrame:CGRectMake(5,buttonY,50,26)];
   [button setTitle:@"<" forState:UIControlStateNormal];
   button.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin;
   [button addTarget:self action:@selector(buttonBack) forControlEvents:UIControlEventTouchUpInside];
   [navBar addSubview:button];
      
   button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
   [button setFrame:CGRectMake(60,buttonY,50,26)];
   [button setTitle:@">" forState:UIControlStateNormal];
   button.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin;
   [button addTarget:self action:@selector(buttonForward) forControlEvents:UIControlEventTouchUpInside];
   [navBar addSubview:button];
   
   button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
   [button setFrame:CGRectMake(60+55,buttonY,50,26)];
   [button setTitle:@"Stop" forState:UIControlStateNormal];
   button.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin;
   [button addTarget:self action:@selector(buttonStop) forControlEvents:UIControlEventTouchUpInside];
   [navBar addSubview:button];
   
   button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
   [button setFrame:CGRectMake(60+55+55,buttonY,50,26)];
   [button setTitle:@"Home" forState:UIControlStateNormal];
   button.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin;
   [button addTarget:self action:@selector(buttonHome) forControlEvents:UIControlEventTouchUpInside];
   [navBar addSubview:button];
  
   /*
   button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
   [button setFrame:CGRectMake(60+55+55+55,10+15,50,26)];
   [button setTitle:@"Home" forState:UIControlStateNormal];
   button.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin;
   [button addTarget:self action:@selector(buttonForward) forControlEvents:UIControlEventTouchUpInside];
   [navBar addSubview:button];
   */
   
   navLabel = [[UILabel alloc] initWithFrame:CGRectMake(isIpad? 230 : 5,isIpad ? 15 : 5,isIpad ? width - 300: width-50,16)];
   
   navLabel.autoresizingMask = UIViewAutoresizingFlexibleWidth;
   navLabel.text = @"";
   navLabel.backgroundColor = [UIColor clearColor];
   navLabel.font = [UIFont systemFontOfSize:isIpad ? 14 : 12];
   navLabel.textAlignment = UITextAlignmentLeft;
   [navBar addSubview:navLabel];
   

   UINavigationItem *item = [[ UINavigationItem alloc ] initWithTitle:@"" ];
   
   UIBarButtonItem *backButton = [[[UIBarButtonItem alloc] initWithTitle:@"Quit" style:UIBarButtonItemStyleBordered target:/*[self parentViewController]*/self action:  @selector(mydone:) ] autorelease];
      
   item.rightBarButtonItem = backButton;
  
   [ navBar pushNavigationItem: item  animated:YES];
     
   [ self.view addSubview: navBar ];
    
   aWebView = [ [ UIWebView alloc ] initWithFrame: CGRectMake(rect.origin.x, rect.origin.y + navBarHeight, rect.size.width,rect.size.height - (navBarHeight/*+toolbarHeight*/) )];

   aWebView.scalesPageToFit = YES;

   aWebView.autoresizesSubviews = YES;
   aWebView.autoresizingMask=(UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth);

   //set the web view delegates for the web view to be itself
   [aWebView setDelegate:self];
      
   //Set the URL to go to for your UIWebView
   NSString *urlAddress = @"http://www.worldofspectrum.org/infoseek.cgi";

   //Create a URL object.
   NSURL *url = [NSURL URLWithString:urlAddress];

   //URL Requst Object
   NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
   
   //load the URL into the web view.
   [aWebView loadRequest:requestObj];
      
   [ self.view addSubview: aWebView ];

}

-(void)viewDidLoad{	

       
}

-(void)mydone:(id)sender{

   [aWebView stopLoading]; 
   [aWebView setDelegate:nil];
   [self dismissModalViewControllerAnimated:YES];
   iphone_menu = 0;
}
 
- (void)buttonBack {

   if(aWebView!=nil)
   {
       [aWebView goBack];
   } 
}

- (void)buttonForward {

   if(aWebView!=nil)
      [aWebView goForward];
}

- (void)buttonStop {

   if(aWebView!=nil)
   {
       [aWebView stopLoading];   
       if(navLabel!=nil)
	     navLabel.text = @"Stopped!";    
   } 
}

- (void)buttonHome {

   if(aWebView!=nil)
   {
      NSString *urlAddress = @"http://www.worldofspectrum.org/infoseek.cgi";

     //Create a URL object.
     NSURL *url = [NSURL URLWithString:urlAddress];

     //URL Requst Object
     NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
   
     //load the URL into the web view.
     
     [aWebView loadRequest:requestObj];
   } 
}

- (void)webViewDidStartLoad:(UIWebView *)webView{
    //[navBar topItem].title = webView.request.URL.absoluteString;
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error{

    //[navBar topItem].title = @"Error";
    //if(navLabel!=nil) navLabel.text = @"Error!";
    
    if(error!=nil && error.code != -999)
    {
	    UIAlertView *connectionAlert = [[UIAlertView alloc] initWithTitle:@"Connection Failed!" 
															message:[NSString stringWithFormat:@"Error:%@",[error localizedDescription]] 
															delegate:self 
													        cancelButtonTitle:@"OK" 
													        otherButtonTitles: nil];	
	    [connectionAlert show];
	    [connectionAlert release];
	}
	
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType{
     
    //[navBar topItem].title = @"Wait... Loading!:";
    
      
    NSString* fileName = [[[request URL] path] lastPathComponent];
        	                              
    if([fileName hasSuffix:@".zip"] || 
       [fileName hasSuffix:@".tzx"] || 
       [fileName hasSuffix:@".tap"] || 
       [fileName hasSuffix:@".sna"] ||
       [fileName hasSuffix:@".z80"] ||
       [fileName hasSuffix:@".sp"] ||       
       [fileName hasSuffix:@".dsk"]
       )
    {
       /*
       NSString* alertMessage = [NSString stringWithFormat:@"%@%@", @"Download?:\n", [[[request URL] path] lastPathComponent]];
	   UIAlertView* downloadAlertView=[[UIAlertView alloc] initWithTitle:nil
                    message:alertMessage
					delegate:self cancelButtonTitle:nil
                    otherButtonTitles:@"DENY",@"CONFIRM",nil];
	  [downloadAlertView show];
	  [downloadAlertView release];
	  */
	  
	  if(isDownloading)
	     return NO;
       	
	  dRequest = [request copy];//If you are using managed memory (not garbage collection), this method retains the new object before returning it. The invoker of the method, however, is responsible for releasing the returned object.
	  NSURLConnection *connection = [[NSURLConnection alloc] initWithRequest:request delegate:self];//al ser asincrona hay que hacer el release mas tarde
	 	  
	  if (connection)
	  {
	    isDownloading = 1;
	    
	    payload = [[NSMutableData data] retain];
	    
	    progressAlert = [[UIAlertView alloc] initWithTitle: @"Downloading!" message: @"Please wait..." delegate: self cancelButtonTitle: nil otherButtonTitles: nil];
        progressView = [[UIProgressView alloc] initWithFrame:CGRectMake(30.0f, 80.0f, 225.0f, 90.0f)];
        [progressAlert addSubview:progressView];
        [progressView setProgressViewStyle: UIProgressViewStyleBar];
        [progressView release];    
        [progressAlert show];
      
	  }
	  else
  	  {
  	  
  	      [dRequest release];
  	      
  	      UIAlertView *errorAlert = [[UIAlertView alloc] initWithTitle:@"Error" 
														   message:@"Unable to start download."
														   delegate:self 
													       cancelButtonTitle:@"OK" 
													       otherButtonTitles: nil];
	      [errorAlert show];
	      [errorAlert release];
	
	   }
	   return NO;
	}
	else
	{	
	   if(navLabel!=nil)
	     navLabel.text = @"Wait... Loading!";
	}
	
   return YES;
}

- (void)webViewDidFinishLoad:(UIWebView *)webView{
   //[navBar topItem].title = webView.request.URL.absoluteString;
   if(navLabel!=nil && webView!=nil)
   {
       if(webView.request!=nil)
          navLabel.text = webView.request.URL.absoluteString;
   }    
}

- (void)connection:(NSURLConnection *)conn didReceiveResponse:(NSURLResponse *)response
{
 
   [payload setLength:0];
   expected = [response expectedContentLength];
   
}

- (void)connection:(NSURLConnection *)conn didReceiveData:(NSData *)data
{
   
   [payload appendData:data];
   progressView.progress = (float)([payload length] / expected);

}
- (void)connectionDidFinishLoading:(NSURLConnection *)conn
{
   [payload writeToFile: [NSString stringWithFormat:@"/var/mobile/Media/ROMs/iXpectrum/downloads/%@", [[[dRequest URL] path] lastPathComponent]] atomically:NO];
    
   [dRequest release];
   
   [conn release];
   
   [payload release];
   
   isDownloading = 0;
   
   [progressAlert dismissWithClickedButtonIndex:0 animated:YES];  
   [progressAlert release];

}
- (void)connection:(NSURLConnection *)conn didFailWithError:(NSError *)error
{
        
    [dRequest release];

    [payload release];
    
    [conn release];

    isDownloading = 0;
    
    [progressAlert dismissWithClickedButtonIndex:0 animated:YES]; 
    [progressAlert release];
    
   	UIAlertView *errorAlert = [[UIAlertView alloc] initWithTitle:@"Error" 
												   message:[NSString stringWithFormat:@"%@\n%@",[error localizedDescription],[[error userInfo] objectForKey:NSErrorFailingURLStringKey] ] 
												   delegate:self 
												   cancelButtonTitle:@"OK" 
												   otherButtonTitles: nil];
	
	[errorAlert show];
	[errorAlert release];
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    //http://developer.apple.com/iphone/library/qa/qa2010/qa1688.html	   
     return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {

   if(aWebView!=nil)
   {
       [aWebView setDelegate:nil];
       [aWebView stopLoading]; 
       [aWebView removeFromSuperview];
       [aWebView release];
       aWebView = nil;
   }
		  
	//[super didReceiveMemoryWarning];
}


- (void)dealloc {
   	
    if(navLabel != nil)
    {
       [navLabel release];
       navLabel = nil;
    }
  
  
    if(aWebView!=nil)
    {
       [aWebView setDelegate:nil];
       //[aWebView stopLoading];       
       [aWebView release];
												   
     }  
     
	[super dealloc];
}

@end