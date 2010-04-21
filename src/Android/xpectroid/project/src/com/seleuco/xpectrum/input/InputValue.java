
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

import android.graphics.Rect;


public class InputValue {
	
	protected Rect origRect = null;    
	protected Rect fixedRect = null;
	
    int [] data;
    
    public InputValue(int d[]){
       data = d;	
    }
  
    public Rect getRect(){
    	if(fixedRect!=null)
    		return fixedRect;    	
    	return getOrigRect();
    }
    
    public Rect newFixedRect(){
    	Rect r = getOrigRect();
    	if(r!=null)
    	{	
    	   fixedRect = new Rect();
    	   fixedRect.set(r);
    	}   
    	return fixedRect;
    }
    
    protected Rect getOrigRect(){
    	if(origRect==null)
    	{
    		origRect = new Rect( data[2], data[3], data[2] + data[4] , data[3] + data[5]); 
    	}
    	return origRect;
    }
        
    public int getType(){
    	return data[0];
    }
    
    public int  getValue(){
    	return data[1];
    }
        
}