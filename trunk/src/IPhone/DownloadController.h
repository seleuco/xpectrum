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
#import <UIKit/UIWebView.h>
#import <UIKit/UIProgressView.h>
#import <UIKit/UIToolbar.h>

@interface DownloadController : UIViewController  <UIWebViewDelegate>
{
   UIWebView *aWebView;
   //UINavigationBar    *navBar;

   //NSURLConnection *connection;
   NSURLRequest *dRequest;
   NSMutableData *payload;

   double  expected;
   int isDownloading;

   UIAlertView *progressAlert;
   UIProgressView *progressView;
   UILabel *navLabel;

   @public BOOL bIsDismissed ;
}

  - (void)buttonBack;
  - (void)buttonForward;
  - (void)buttonStop;
  - (void)buttonHome;

  -(void)mydone:(id)sender;

 - (void)connection:(NSURLConnection *)conn didReceiveResponse:(NSURLResponse *)response;
 - (void)connection:(NSURLConnection *)conn didReceiveData:(NSData *)data;
 - (void)connectionDidFinishLoading:(NSURLConnection *)conn;
 - (void)connection:(NSURLConnection *)conn didFailWithError:(NSError *)error;

 @property (readwrite,assign)  BOOL bIsDismissed;
@end
