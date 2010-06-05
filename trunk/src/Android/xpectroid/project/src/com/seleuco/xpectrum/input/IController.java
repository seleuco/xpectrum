
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

package com.seleuco.xpectrum.input;

public interface IController {
	
	final public static int UP = 0x1;
	final public static int LEFT=0x4;
	final public static int DOWN=0x10;
	final public static int RIGHT=0x40;
	final public static int START=1<<8;   
	final public static int SELECT=1<<9;
	final public static int L1=1<<10;
	final public static int R1=1<<11;   
	final public static int A=1<<12;
	final public static int B=1<<13;
	final public static int X=1<<14;
	final public static int Y=1<<15;
	
}
