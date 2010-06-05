
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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

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
	
	final public static int SPECKEY_DEL_POS =  52;
	final public static int SPECKEY_SHIFT_POS = 50;//51;
	final public static int SPECKEY_0_POS = 12;//12;

	protected static final int[] emulatorInputValues = {
		UP,DOWN,LEFT,RIGHT, B, X, A, Y, L1, R1, SELECT, START,
		SPECKEY_0, SPECKEY_1, SPECKEY_2, SPECKEY_3, 
		SPECKEY_4, SPECKEY_5, SPECKEY_6, SPECKEY_7,
		SPECKEY_8, SPECKEY_9, SPECKEY_A, SPECKEY_B, 
		SPECKEY_C, SPECKEY_D, SPECKEY_E, SPECKEY_F, 
		SPECKEY_G, SPECKEY_H, SPECKEY_I, SPECKEY_J,
		SPECKEY_K, SPECKEY_L, SPECKEY_M, SPECKEY_N,
		SPECKEY_O, SPECKEY_P, SPECKEY_Q, SPECKEY_R, 
		SPECKEY_S, SPECKEY_T, SPECKEY_U, SPECKEY_V, 
		SPECKEY_W, SPECKEY_X, SPECKEY_Y, SPECKEY_Z, 
		SPECKEY_SPACE, SPECKEY_ENTER, SPECKEY_SHIFT, SPECKEY_SYMB, 
		0
	};
		
	public static int[] defaultKeyMapping = {
		KeyEvent.KEYCODE_DPAD_UP,KeyEvent.KEYCODE_DPAD_DOWN,KeyEvent.KEYCODE_DPAD_LEFT,KeyEvent.KEYCODE_DPAD_RIGHT,
		KeyEvent.KEYCODE_DPAD_CENTER,KeyEvent.KEYCODE_SEARCH,-1,-1,-1,-1,-1,KeyEvent.KEYCODE_BACK,
		KeyEvent.KEYCODE_0,KeyEvent.KEYCODE_1,KeyEvent.KEYCODE_2,KeyEvent.KEYCODE_3,
		KeyEvent.KEYCODE_4,KeyEvent.KEYCODE_5,KeyEvent.KEYCODE_6,KeyEvent.KEYCODE_7,
		KeyEvent.KEYCODE_8,KeyEvent.KEYCODE_9,KeyEvent.KEYCODE_A,KeyEvent.KEYCODE_B,
		KeyEvent.KEYCODE_C,KeyEvent.KEYCODE_D,KeyEvent.KEYCODE_E,KeyEvent.KEYCODE_F,
		KeyEvent.KEYCODE_G,KeyEvent.KEYCODE_H,KeyEvent.KEYCODE_I,KeyEvent.KEYCODE_J,
		KeyEvent.KEYCODE_K,KeyEvent.KEYCODE_L,KeyEvent.KEYCODE_M,KeyEvent.KEYCODE_N,
		KeyEvent.KEYCODE_O,KeyEvent.KEYCODE_P,KeyEvent.KEYCODE_Q,KeyEvent.KEYCODE_R,
		KeyEvent.KEYCODE_S,KeyEvent.KEYCODE_T,KeyEvent.KEYCODE_U,KeyEvent.KEYCODE_V,
		KeyEvent.KEYCODE_W,KeyEvent.KEYCODE_X,KeyEvent.KEYCODE_Y,KeyEvent.KEYCODE_Z,
		KeyEvent.KEYCODE_SPACE,KeyEvent.KEYCODE_ENTER,KeyEvent.KEYCODE_SHIFT_LEFT,KeyEvent.KEYCODE_ALT_LEFT,
		KeyEvent.KEYCODE_DEL
	};
			
	public static int[] keyMapping = new int[emulatorInputValues.length];
	
	protected float dx;
	protected float dy;
				
	protected ArrayList<InputValue> buttons = new ArrayList<InputValue>();
	
	protected long pad_data = 0;
	
	protected int newtouch;
	protected int oldtouch;
	protected boolean touchstate;
		
	
	//////////////////
	
	protected ArrayList<InputValue> keys = new ArrayList<InputValue>();
	
	protected ArrayList<InputValue> pressedKeys = new ArrayList<InputValue>();
	
	protected int [] pressedKeysValues = new int[10];

	protected int numPressedKeys = 0;
	
	
	protected boolean isShift = false;
	protected boolean isSymbol = false;
	   
    /////////////////
	
	protected  int trackballSensitivity = 30;
	protected  boolean trackballEnabled = true;
		
	/////////////////
	
	final public static int STATE_SHOWING_CONTROLLER = 1;
	final public static int STATE_SHOWING_KEYBOARD = 2;
	final public static int STATE_SHOWING_NONE = 3;
	
	protected int state = STATE_SHOWING_CONTROLLER;
	
	final public static int TYPE_ACTION  = 1;
	final public static int TYPE_SWITCH  = 2;
	final public static int TYPE_OPACITY = 3;
	
	protected Xpectroid xoid = null;
	
	//protected Timer timer = new Timer();
	
	protected Handler handler = new Handler();
		
	protected Runnable finishKeyPress = new Runnable(){
		//@Override
	    public void run() {
			pressedKeys.clear();
			xoid.getInputView().invalidate();					
		}};
		
	protected Object lock = new Object();
	
	protected Runnable finishTrackBallMove = new Runnable(){
			//@Override
		    public void run() {
		    	//synchronized(lock){
		    	  //System.out.println("---> INIT C");	
		    	  //System.out.println("+CLEAR Set Pad "+pad_data+ " new:"+newtrack+" old:"+oldtrack);
		    	  //pad_data &= ~oldtrack;	
		    	  pad_data &= ~UP;
		    	  pad_data &= ~DOWN;
		    	  pad_data &= ~LEFT;
		    	  pad_data &= ~RIGHT;
				  Emulator.setPadData(pad_data);

				  //System.out.println("++CLEAR Set Pad "+pad_data+ " new:"+newtrack+" old:"+oldtrack);
				  //System.out.println("---> CLEAR");
		    	//}
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
	
	public void setTrackballSensitivity(int trackballSensitivity) {
		this.trackballSensitivity = trackballSensitivity;
	}
	
	public void setTrackballEnabled(boolean trackballEnabled) {
		this.trackballEnabled = trackballEnabled;
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
	
	protected boolean handlePADKey(int value, KeyEvent event){
				
		setPadData(event,emulatorInputValues[value]);
		
		Emulator.setPadData(pad_data);
   		
		return true;		
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
	
	protected boolean handleSpeccyKey( int value, KeyEvent event){
		   
		boolean b = false;

		int res = InputHandler.emulatorInputValues[value];

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
          
		 int value = -1;
		 for(int i=0; i<keyMapping.length; i++)
			 if(keyMapping[i]==keyCode)
				 value = i;
		  
         if(value >=0 && value <=11)
		     if(handlePADKey(value, event))return true;
         
         if(value >=12)
         {
             if(value == SPECKEY_DEL_POS)
             {	 
            	 handleSpeccyKey(SPECKEY_SHIFT_POS, event);
            	 value = SPECKEY_0_POS;
             } 
        	 if(handleSpeccyKey(value, event))return true;
         }    
         
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

		if(event.getAction() == MotionEvent.ACTION_DOWN /*|| event.getAction() == MotionEvent.ACTION_MOVE*/)
		{
		   
		   int x = (int)event.getX();
		   int y = (int)event.getY();
		   
		   for(int i =0; i< keys.size();i++)
		   {
			   InputValue iv = keys.get(i);
			   
			   if(iv.getRect().contains(x, y))
			   {	  
				  if(iv.getType()==TYPE_ACTION)
				  {	  
					  
					 int value = iv.getValue();
					 
					 numPressedKeys = 0;
					 
					 if(value==SPECKEY_SHIFT)
					 {	 
						 isShift = !isShift;
					 }	 
					 else if(value==SPECKEY_SYMB)
					 {
						 isSymbol = !isSymbol;
					 }
					 else
					 {
						 pressedKeysValues[numPressedKeys]=value;
					     numPressedKeys++;	 
					 }
					 
					 if(isShift)
					 {
						 pressedKeysValues[numPressedKeys]=SPECKEY_SHIFT;
					     numPressedKeys++;
					 }
					 
					 if(isSymbol)
					 {
						 pressedKeysValues[numPressedKeys]=SPECKEY_SYMB;
					     numPressedKeys++;
					 }
				     
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
		else if (event.getAction() == MotionEvent.ACTION_CANCEL || event.getAction() == MotionEvent.ACTION_UP)
		{
			numPressedKeys = 0;
			Emulator.setKeys(pressedKeysValues, numPressedKeys);
			handler.removeCallbacks(finishKeyPress);
			handler.postDelayed(finishKeyPress, 200);
		}
		return false;
	}
	
	/*
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
			handler4Key.postDelayed(runnable2Clear, 200);
		}
		return false;
	}
    */
	
	/*
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
	*/
	
	public boolean isSymbol() {
		return isSymbol;
	}

	public void setSymbol(boolean isSymbol) {
		this.isSymbol = isSymbol;
	}

	public boolean isShift() {
		return isShift;
	}

	public void setShift(boolean isShift) {
		this.isShift = isShift;
	}

	protected boolean handleTouchController(MotionEvent event) {

		int action = event.getAction();

		oldtouch = newtouch;

		int x = (int) event.getX();
		int y = (int) event.getY();

		if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_CANCEL) {
			touchstate = false;
			
		} else {

			touchstate = true;

			for (int j = 0; j < buttons.size(); j++) {
				InputValue iv = buttons.get(j);

				if (iv.getRect().contains(x, y)) {

					if (iv.getType() == TYPE_ACTION) {

						switch (action) {

						case MotionEvent.ACTION_DOWN:
						case MotionEvent.ACTION_POINTER_DOWN:
						case MotionEvent.ACTION_MOVE:
							
							newtouch = getButtonValue(iv.getValue());
							if (oldtouch != newtouch) 
								pad_data &= ~oldtouch;							
							pad_data |= newtouch ;
							break;
						}
					} else if (iv.getType() == TYPE_SWITCH) {
						if (event.getAction() == MotionEvent.ACTION_DOWN) {
							touchstate = false;
							oldtouch = 0;
							changeState();
							xoid.getMainHelper().updateViews();
							return true;
						}
					}
				}
			}			
		}

		if (!touchstate) {

			pad_data &= ~oldtouch;
			newtouch = 0;
			oldtouch = 0;
		}

		Emulator.setPadData(pad_data);
		return true;
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

	public boolean onTrackballEvent(MotionEvent event) {

		int gap = 0;

		if(!trackballEnabled)return false;
		
		int action = event.getAction();

		if (action == MotionEvent.ACTION_MOVE /*&& trackballEnabled*/) {

			int newtrack = 0;

			final float x = event.getX();
			final float y = event.getY();

			//float d = Math.max(Math.abs(x), Math.abs(y));

			// System.out.println("x: "+x+" y:"+y);

			if (y < -gap) {
				newtrack |= UP;
				// System.out.println("Up");
			} else if (y > gap) {
				newtrack |= DOWN;
				// System.out.println("Down");
			}

			if (x < -gap) {
				newtrack |= LEFT;
				// System.out.println("left");
			} else if (x > gap) {
				newtrack |= RIGHT;
				// System.out.println("right");
			}

			// System.out.println("Set Pad "+pad_data+
			// " new:"+newtrack+" old:"+oldtrack);

			handler.removeCallbacks(finishTrackBallMove);
			handler.postDelayed(finishTrackBallMove, (int) (/* 50 * d */150 * trackballSensitivity));// TODO
			
			if (newtrack != 0) {
				pad_data &= ~UP;
				pad_data &= ~DOWN;
				pad_data &= ~LEFT;
				pad_data &= ~RIGHT;
				pad_data |= newtrack;
			}

		} else if (action == MotionEvent.ACTION_DOWN) {
			pad_data |= B;

		} else if (action == MotionEvent.ACTION_UP) {
			pad_data &= ~B;
		}

		Emulator.setPadData(pad_data);

		return true;
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
	
}
