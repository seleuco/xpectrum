
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

package com.seleuco.xpectrum.input;

import java.lang.reflect.Method;

import android.view.MotionEvent;

import com.seleuco.xpectrum.Xpectroid;

public class InputHandlerFactory {
	
	static public InputHandler createInputHandler(Xpectroid xoid){		
	    try {
		      @SuppressWarnings("unused")
			  Method m = MotionEvent.class.getMethod("getPointerCount");		     
		      return new InputHandlerExt(xoid);//MultiTouch  
		} catch (NoSuchMethodException e) {
			return new InputHandler(xoid);
		}		
	}	
}
