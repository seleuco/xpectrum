
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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.StringTokenizer;
import java.util.Timer;

import android.content.res.Configuration;
import android.graphics.Rect;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.View.OnTouchListener;

import com.seleuco.xpectrum.Emulator;
import com.seleuco.xpectrum.Xpectroid;

public class InputHandler implements OnTouchListener, OnKeyListener, IController, ISpecKeys{
		
	protected float dx;
	protected float dy;
				
	protected ArrayList<InputValue> buttons = new ArrayList<InputValue>();
	
	protected long pad_data = 0;
			
	//////////////////
	
	protected ArrayList<InputValue> keys = new ArrayList<InputValue>();
	
	protected ArrayList<InputValue> pressedKeys = new ArrayList<InputValue>();
	
	protected int [] pressedKeysValues = new int[10];

	protected int numPressedKeys = 0;
	   
    /////////////////
	
	final public static int STATE_SHOWING_CONTROLLER = 1;
	final public static int STATE_SHOWING_KEYBOARD = 2;
	final public static int STATE_SHOWING_NONE = 3;
	
	protected int state = STATE_SHOWING_CONTROLLER;
	
	final public static int TYPE_ACTION  = 1;
	final public static int TYPE_SWITCH  = 2;
	final public static int TYPE_OPACITY = 3;
	
	
	protected Xpectroid xoid = null;
	
	protected Timer timer = new Timer();
	
	protected Handler handler4Key = new Handler();
		
	protected Runnable runnable2Clear = new Runnable(){
		//@Override
	    public void run() {
			pressedKeys.clear();
			xoid.getInputView().invalidate();					
		}};
		
	public InputHandler(Xpectroid value){
		
		xoid = value;
				
		if(xoid.getMainHelper().getscrOrientation() == Configuration.ORIENTATION_LANDSCAPE)
		{
			state = xoid.getPrefsHelper().isLandscapeTouchController() ? STATE_SHOWING_CONTROLLER : STATE_SHOWING_NONE;
		}
		else
		{
			state = xoid.getPrefsHelper().isPortraitTouchController() ? STATE_SHOWING_CONTROLLER : STATE_SHOWING_NONE;
		}
	}
	
	public int setInputHandlerState(int value){
		return state = value;
	}
	
	public int getInputHandlerState(){
		return state;
	}
	
	public void changeState(){
		
		
		boolean isTouchKeyboard = xoid.getMainHelper().getscrOrientation() == Configuration.ORIENTATION_PORTRAIT ?
				                  xoid.getPrefsHelper().isPortraitTouchKeyboard():
				                  xoid.getPrefsHelper().isLandscapeTouchKeyboard();
				                  
		boolean isTouchController = xoid.getMainHelper().getscrOrientation() == Configuration.ORIENTATION_PORTRAIT ?
				                    xoid.getPrefsHelper().isPortraitTouchController():
				                    xoid.getPrefsHelper().isLandscapeTouchController();
		
	    if(state == STATE_SHOWING_CONTROLLER)
	    {				    	
	    	pad_data = 0;
	    	Emulator.setPadData(pad_data);
	    	
	    	if(isTouchKeyboard)
	    	   state = STATE_SHOWING_KEYBOARD;
	    	else
	    	   state = STATE_SHOWING_NONE;		    				    	
	    }
	    else if(state == STATE_SHOWING_KEYBOARD)
	    {
	    	pressedKeys.clear();
	    	numPressedKeys = 0;
	    	Emulator.setKeys(pressedKeysValues, numPressedKeys);
	    	
	    	if(isTouchController)
	    	    state = STATE_SHOWING_CONTROLLER;
	    	else
	    		state = STATE_SHOWING_NONE;			    	
	    }
	    else
	    {
	    	if(isTouchController)
	    	    state = STATE_SHOWING_CONTROLLER;
	    	else if(isTouchKeyboard)
	    		state = STATE_SHOWING_KEYBOARD;
	    }	    	    	
	}
	
	public void setFixFactor(float dx, float dy){
		this.dx = dx;
		this.dy = dy;
		fixControllerCoords(buttons);
		fixControllerCoords(keys);
	}
	
	protected boolean setPadData(KeyEvent event, int data){
		int action = event.getAction();
		if(action == KeyEvent.ACTION_DOWN)
			pad_data |= data;
		else if(action == KeyEvent.ACTION_UP)
			pad_data &= ~ data;
		return true;
	}
	
	protected boolean handleDPAD(int keyCode, KeyEvent event){
		boolean b = false;
		
		switch(keyCode)
		{		
			//digital PAD
		    case KeyEvent.KEYCODE_DPAD_UP:		
				b=setPadData(event,UP);break;
			case KeyEvent.KEYCODE_DPAD_DOWN:
				b=setPadData(event,DOWN);break;
			case KeyEvent.KEYCODE_DPAD_LEFT:
				b=setPadData(event,LEFT);break;
			case KeyEvent.KEYCODE_DPAD_RIGHT:
				b=setPadData(event,RIGHT);break;
			case KeyEvent.KEYCODE_DPAD_CENTER:
				b=setPadData(event,B);break;
			case KeyEvent.KEYCODE_BACK:
				b=setPadData(event,X);break;	
			case KeyEvent.KEYCODE_SEARCH:
				b=setPadData(event,A);break;	
		}
						
		if(b)
		{
		   Emulator.setPadData(pad_data);
		}   
		
		return b;		
	}
	
	protected int findKey(int res)
	{
 	   int pos = -1;
		for(int i=0;i<numPressedKeys;i++)
	   {	   
		   if(pressedKeysValues[i]==res)
		   {
			   pos = i;break;
		   }
	   }
		return pos;
	}
	
	protected boolean handleKey( int keyCode, KeyEvent event){
		   
		boolean b = false;

		int res = this.getSpeccyKeyCode(event.getKeyCode());

		if (res != -1) {
			if (event.getAction() == KeyEvent.ACTION_DOWN) {
				int pos = findKey(res);
				if (pos == -1) {
					pressedKeysValues[numPressedKeys] = res;
					numPressedKeys++;
					Emulator.setKeys(pressedKeysValues, numPressedKeys);
				}
				b = true;
			} else if (event.getAction() == KeyEvent.ACTION_UP) {
				int pos = findKey(res);
				if (pos != -1) {

					for (int i = pos + 1; i < numPressedKeys; i++)
						pressedKeysValues[i - 1] = pressedKeysValues[i];
					numPressedKeys--;
					Emulator.setKeys(pressedKeysValues, numPressedKeys);

				}
				b = true;
			}
		}

		return b;	
	}
		
	public boolean onKey(View v, int keyCode, KeyEvent event) {
		//Log.d("JAVA", "onKeyDown=" + keyCode + " " + keyCode + " " + event.getDisplayLabel() + " " + event.getUnicodeChar() + " " + event.getNumber());

		
         if(handleDPAD(keyCode, event))return true;
         if(handleKey(keyCode, event))return true;
         return false;
	}
	
	public void handleVirtualKey(int action){
		
		pad_data |= action;
		Emulator.setPadData(pad_data);
		
		try {
			Thread.sleep(150);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		pad_data &= ~ action;
		Emulator.setPadData(pad_data);
		
	}
	
	//debug method
	public ArrayList<InputValue> getAllInputData(){
		if(state == STATE_SHOWING_CONTROLLER)
			return buttons;
		else if(state == STATE_SHOWING_KEYBOARD)
			return keys;
		else
			return null;			    
	}
	
	public ArrayList<InputValue> getPressedKeys(){
		
		return	pressedKeys;//TODO best, tipo, varias teclas... etc
	}
	
	protected boolean handleTouchKeyboard(MotionEvent event){

		if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_MOVE)
		{
		   
		   int x = (int)event.getX();
		   int y = (int)event.getY();
		   
		   for(int i =0; i< keys.size();i++)
		   {
			   InputValue iv = keys.get(i);
			   
			   //TODO: solo proceso los tipo 1
			   if(iv.getRect().contains(x, y))
			   {	  
				  if(iv.getType()==TYPE_ACTION)
				  {	  
				     pressedKeysValues[0]=iv.getValue();
				     numPressedKeys = 1;
				     Emulator.setKeys(pressedKeysValues, numPressedKeys);
				     pressedKeys.clear();//solo una de momento
				     pressedKeys.add(iv);
				     xoid.getInputView().invalidate();
				     return true;//NO MULTITOUCH (SIC)
				  }
				  else if(iv.getType()==TYPE_SWITCH)
				  {					  
					  changeState();
					  xoid.getMainHelper().updateViews();
					  return true;
				  }				  
			   }			   
		   }
		}   
		else
		{
			numPressedKeys = 0;
			Emulator.setKeys(pressedKeysValues, numPressedKeys);
			handler4Key.removeCallbacks(runnable2Clear);
			handler4Key.postDelayed(runnable2Clear, 300);
		}
		return false;
	}

	protected boolean handleTouchController(MotionEvent event){
		boolean b = false;
		
		if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_MOVE)
		{
		   //int x = (int)event.getRawX();
		   //int y = (int)event.getRawY();
		   
		   int x = (int)event.getX();
		   int y = (int)event.getY();
		   
		   for(int i =0; i< buttons.size();i++)
		   {
			   InputValue iv = buttons.get(i);
			   
			   if(iv.getRect().contains(x, y))
			   {	
				   if(iv.getType()==TYPE_ACTION)
				   {	 
				       pad_data |= getButtonValue(iv.getValue());				  
				       b =  true;//NO MULTITOUCH (SIC)
				  }
				  else if(iv.getType()==TYPE_SWITCH)
				  {					  
					  changeState();
					  xoid.getMainHelper().updateViews();
					  return true;
				  }		
			   }				   
		   }	   		   
		}   
		else
		{
			pad_data = 0; b = true;
		}
		
		if(b)
	       Emulator.setPadData(pad_data);
		
        return b;
		
	}
	
	public boolean onTouch(View v, MotionEvent event) {
		
		//System.out.println(event.getRawX()+" "+event.getX()+" "+event.getRawY()+" "+event.getY());
		
		if(v==xoid.getEmuView() && xoid.getMainHelper().getscrOrientation() == Configuration.ORIENTATION_PORTRAIT)
		{		
		    if(event.getAction() == MotionEvent.ACTION_DOWN )
		    {
		    	changeState();
		    	xoid.getMainHelper().updateViews();	
		    	
			    return true;
		    }
		    return false;
		} else if(xoid.getMainHelper().getscrOrientation() == Configuration.ORIENTATION_LANDSCAPE && state == STATE_SHOWING_NONE)
		{		
		    if(event.getAction() == MotionEvent.ACTION_DOWN )
		    {
		    	changeState();
		    	xoid.getMainHelper().updateViews();			    	
			    return true;
		    }
		    return false;
		}
		else if(v == xoid.getInputView())
		{
		    if(state == STATE_SHOWING_CONTROLLER)
		    {	
		    	 return handleTouchController(event);
		    }
		    else if(state == STATE_SHOWING_KEYBOARD)
		    {
		    	 return handleTouchKeyboard(event);
		    }
		    return false;
		}    
		return false;
	}
	
	protected void fixControllerCoords(ArrayList<InputValue> values) {

		if (values != null) {
			for (int i = 0; i < values.size(); i++) {

				Rect r = values.get(i).newFixedRect();
				if (r != null) {
					if(xoid.getMainHelper().getscrOrientation() == Configuration.ORIENTATION_PORTRAIT)
					{
						r.top -= 240;
					    r.bottom -= 240;
					}
					r.right = (int) (r.right * dx);
					r.left = (int) (r.left * dx);
					r.top = (int) (r.top * dy);
					r.bottom = (int) (r.bottom * dy);
					//values.get(i).setRect(r);
				}
			}
		}
	}
	
	public int getOpacity(){
		
		ArrayList<InputValue> values = null;
		if(state==STATE_SHOWING_CONTROLLER)
			values = buttons;
		else if(state==STATE_SHOWING_KEYBOARD)
			values = keys;
		else
			return -1;
		
		for(InputValue v : values)
		{
		   if(v.getType() == TYPE_OPACITY)
			   return v.getValue();
		}
		return -1;
	}
	
	public void readControllerValues(int v){
		readInputValues(v,buttons);
		fixControllerCoords(buttons);
	}

	public void readKeyboardValues(int v){
		readInputValues(v,keys);
		fixControllerCoords(keys);
	}
	
	protected void readInputValues(int id, ArrayList<InputValue> values)
	{
	     InputStream is = xoid.getResources().openRawResource(id);
	     
	     InputStreamReader isr = new InputStreamReader(is);
	     BufferedReader br = new BufferedReader(isr);
	     
	     InputValue iv = null;
	     values.clear();
	     
	     int i=0;
	     try{
		     String s = br.readLine();
		     while(s!=null)
		     {
		    	 int [] data = new int[10]; 
		    	 StringTokenizer st = new StringTokenizer(s,",");
		    	    int j = 0;
		    		while(st.hasMoreTokens()){
		                String token = st.nextToken();
		                int k = token.indexOf("/");
		                if(k!=-1)
		                {
		                   token = token.substring(0, k);	
		                }
		                token = token.trim();
		                data[j] = Integer.parseInt(token);
		                j++;
		            }    	 
		    	 		    
                    //values.
		    		
		    	    s = br.readLine();i++;		    	    
		    	    if(j!=0)
		    	    {	
		    	       iv = new InputValue(data);				    	    
		    	       values.add(iv);
		    	    }     
		     }	 
	     }catch(IOException e)
	     {
	    	 e.printStackTrace();
	     }
	}
	
	int getButtonValue(int i){
		switch(i){
		   case 1: return   UP;
		   case 2: return   DOWN;
		   case 3: return   LEFT;
		   case 4: return   RIGHT;
		   case 5: return   UP | LEFT;
		   case 6: return   UP | RIGHT;
		   case 7: return   DOWN | LEFT;
		   case 8: return   DOWN | RIGHT;
		   case 9: return   START;   
		   case 10: return  SELECT;
		   case 11: return  L1;
		   case 12: return  R1;   
		   case 13: return  A;
		   case 14: return  B;
		   case 15: return  X;
		   case 16: return  Y;
		}
		return -1;
	}
	
	int getSpeccyKeyCode(int value){
	    switch(value){
	       case KeyEvent.KEYCODE_1: return SPECKEY_1;
	       case KeyEvent.KEYCODE_2: return SPECKEY_2;
	       case KeyEvent.KEYCODE_3: return SPECKEY_3;
	       case KeyEvent.KEYCODE_4: return SPECKEY_4;
	       case KeyEvent.KEYCODE_5: return SPECKEY_5;
	       case KeyEvent.KEYCODE_6: return SPECKEY_6;
	       case KeyEvent.KEYCODE_7: return SPECKEY_7;
	       case KeyEvent.KEYCODE_8: return SPECKEY_8;
	       case KeyEvent.KEYCODE_9: return SPECKEY_9;
	       case KeyEvent.KEYCODE_0: return SPECKEY_0;
	       			
	       case KeyEvent.KEYCODE_Q: return SPECKEY_Q;
	       case KeyEvent.KEYCODE_W: return SPECKEY_W;
	       case KeyEvent.KEYCODE_E: return SPECKEY_E;
	       case KeyEvent.KEYCODE_R: return SPECKEY_R;
	       case KeyEvent.KEYCODE_T: return SPECKEY_T;
	       case KeyEvent.KEYCODE_Y: return SPECKEY_Y;
	       case KeyEvent.KEYCODE_U: return SPECKEY_U;
	       case KeyEvent.KEYCODE_I: return SPECKEY_I;
	       case KeyEvent.KEYCODE_O: return SPECKEY_O;
	       case KeyEvent.KEYCODE_P: return SPECKEY_P;
						
	       case KeyEvent.KEYCODE_A: return SPECKEY_A;
	       case KeyEvent.KEYCODE_S: return SPECKEY_S;
	       case KeyEvent.KEYCODE_D: return SPECKEY_D;
	       case KeyEvent.KEYCODE_F: return SPECKEY_F;
	       case KeyEvent.KEYCODE_G: return SPECKEY_G;
	       case KeyEvent.KEYCODE_H: return SPECKEY_H;
	       case KeyEvent.KEYCODE_J: return SPECKEY_J;
	       case KeyEvent.KEYCODE_K: return SPECKEY_K;
	       case KeyEvent.KEYCODE_L: return SPECKEY_L;
	       case KeyEvent.KEYCODE_ENTER: return SPECKEY_ENTER;
	       
	       case KeyEvent.KEYCODE_SHIFT_LEFT: return SPECKEY_SHIFT;
	       case KeyEvent.KEYCODE_SHIFT_RIGHT: return SPECKEY_SHIFT;
	       
	       case KeyEvent.KEYCODE_Z: return SPECKEY_Z;
	       case KeyEvent.KEYCODE_X: return SPECKEY_X;
	       case KeyEvent.KEYCODE_C: return SPECKEY_C;
	       case KeyEvent.KEYCODE_V: return SPECKEY_V;
	       case KeyEvent.KEYCODE_B: return SPECKEY_B;
	       case KeyEvent.KEYCODE_N: return SPECKEY_N;
	       case KeyEvent.KEYCODE_M: return SPECKEY_M;
	       case KeyEvent.KEYCODE_ALT_LEFT: return SPECKEY_SYMB;
	       case KeyEvent.KEYCODE_ALT_RIGHT: return SPECKEY_SYMB;
	       
	       case KeyEvent.KEYCODE_SPACE: return SPECKEY_SPACE;	
	    }
	    return -1;
	}		
}
