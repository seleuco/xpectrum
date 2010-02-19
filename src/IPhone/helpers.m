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

const char* get_resource_path(char* file)
{
  static char resource_path[1024];
#ifdef APPSTORE_BUILD
  const char* path = [[[NSBundle mainBundle] resourcePath] UTF8String];
  sprintf(resource_path, "%s/%s", path, file);
#else
  sprintf(resource_path, "/Applications/iXpectrum.app/%s", file);
#endif
  return resource_path;
}

const char* get_documents_path(char* file)
{
  static char documents_path[1024];
#ifdef APPSTORE_BUILD
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex: 0];
  const char* path = [documentsDirectory UTF8String];
  sprintf(documents_path, "%s/%s", path, file);
#else
  sprintf(documents_path, "/var/mobile/Media/ROMs/MAME/%s", file);
#endif
  return documents_path;
}
