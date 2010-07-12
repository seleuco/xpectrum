
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

import android.view.MotionEvent;

import com.seleuco.xpectrum.Emulator;
import com.seleuco.xpectrum.Xpectroid;

public class InputHandlerExt extends InputHandler {
	

	protected int [] touchContrData = new int[10];
	protected InputValue [] touchKeyData = new InputValue[10];
	
	protected static int [] newtouches = new int[10];
	protected static int [] oldtouches = new int[10];
	protected static boolean [] touchstates = new boolean[10];
	
	public InputHandlerExt(Xpectroid value) {
		super(value);
	}
	
	/*
	@Override
	protected boolean handleTouchKeyboard(MotionEvent event) {
		for (int i = 0; i < event.getPointerCount(); i++) {
			int action = event.getAction();
			int actionEvent = action & MotionEvent.ACTION_MASK;
			// int actionPointerId = action &
			// MotionEvent.ACTION_POINTER_ID_MASK;
			int actionPointerId = event.getPointerId(i);
			int x = (int) event.getX(i);
			int y = (int) event.getY(i);
			if (actionEvent == MotionEvent.ACTION_DOWN
					|| actionEvent == MotionEvent.ACTION_POINTER_DOWN
					|| actionEvent == MotionEvent.ACTION_MOVE) {

				for (int j = 0; j < keys.size(); j++) {
					InputValue iv = keys.get(j);

					if (iv.getRect().contains(x, y)) {

						if (iv.getType() == TYPE_ACTION) {
							switch (actionEvent) {
							case MotionEvent.ACTION_DOWN:
							case MotionEvent.ACTION_POINTER_DOWN:
								touchKeyData[actionPointerId] = iv;
								if(!pressedKeys.contains(iv))
								{	
								    pressedKeys.add(iv);
								    xoid.getInputView().invalidate();
								}    
								break;
							case MotionEvent.ACTION_MOVE:
							
								InputValue iv_old = touchKeyData[actionPointerId];
								if(iv_old!=iv)
								{	
									if(iv_old!=null)
									   pressedKeys.remove(iv_old);
									touchKeyData[actionPointerId] = iv;
									pressedKeys.add(iv);
									xoid.getInputView().invalidate();
								}
								break;
							
							}
						} else if (iv.getType() == TYPE_SWITCH) {
							if (event.getAction() == MotionEvent.ACTION_DOWN) {
								changeState();
								xoid.getMainHelper().updateViews();
								return true;
							}
						}
					}
				}
			}else
			{
				InputValue iv = touchKeyData[actionPointerId];
				if(iv!=null)
				{	
				   pressedKeys.remove(iv);
				   xoid.getInputView().invalidate();
				}   
				touchKeyData[actionPointerId] = null;								
			}			
		}
		numPressedKeys = 0;
		for (int i = 0; i < touchKeyData.length; i++) {
			if (touchKeyData[i] != null) {
				pressedKeysValues[numPressedKeys] = touchKeyData[i].getValue();
				numPressedKeys++;
			}
		}

		Emulator.setKeys(pressedKeysValues, numPressedKeys);

		return true;
	}
	*/
	/*
	@Override
	protected boolean handleTouchController(MotionEvent event) {

		Log.d("IC", "handleTouchController: numFingers:"+event.getPointerCount());

		int action = event.getAction();
		int actionEvent = action & MotionEvent.ACTION_MASK;
		int pid = action >> MotionEvent.ACTION_POINTER_ID_SHIFT;

		for (int i = 0; i < event.getPointerCount(); i++) {

			// int actionPointerId = action &
			// MotionEvent.ACTION_POINTER_ID_MASK;
			int actionPointerId = event.getPointerId(i);
						
			int x = (int) event.getX(i);
			int y = (int) event.getY(i);
			
			String s = "";
			switch (actionEvent){
			case MotionEvent.ACTION_DOWN:s="ACTION_DOWN";break;
			case MotionEvent.ACTION_MOVE:s="ACTION_MOVE";break;
			case MotionEvent.ACTION_POINTER_DOWN:s="ACTION_POINTER_DOWN";break;
			case MotionEvent.ACTION_POINTER_UP:s="ACTION_POINTER_UP";break;
			case MotionEvent.ACTION_UP:s="ACTION_UP";break;
			default:s=""+actionEvent;
			}
			
			Log.d("IC", "pointerID: "+actionPointerId+" ACTION:"+s+" x:"+x+"y: "+y);
			
			if(actionEvent == MotionEvent.ACTION_UP || actionEvent == MotionEvent.ACTION_POINTER_UP || actionEvent == MotionEvent.ACTION_CANCEL)
			{
				Log.d("IC", "--> NOPing pad:"+pad_data+ " old: "+touchContrData[actionPointerId]);
				pad_data &= ~touchContrData[actionPointerId];				
				touchContrData[actionPointerId] = 0;
				Log.d("IC", "--> END NOPing pad:"+pad_data+ " new: "+touchContrData[actionPointerId]);
				//otro mirar el otro move si ACTION_POINTER_UP
				break;
			}
			else
			{		
				for (int j = 0; j < buttons.size(); j++) {
					InputValue iv = buttons.get(j);
										
					if (iv.getRect().contains(x, y)) {
						
						Log.d("IC", "Ping value:"+iv.getValue()+ " button: "+getButtonValue(iv.getValue())+" pad:"+pad_data+ " old:"+touchContrData[actionPointerId]);

						if (iv.getType() == TYPE_ACTION) {
							switch (actionEvent) {
							case MotionEvent.ACTION_DOWN:
							case MotionEvent.ACTION_POINTER_DOWN:
								pad_data |= getButtonValue(iv.getValue());
								touchContrData[actionPointerId] = getButtonValue(iv.getValue());
								break;
							
							case MotionEvent.ACTION_MOVE:
								if (getButtonValue(iv.getValue()) != touchContrData[actionPointerId]) {
									pad_data &= ~touchContrData[actionPointerId];
									pad_data |= getButtonValue(iv.getValue());
									touchContrData[actionPointerId] = getButtonValue(iv.getValue());
								}
								break;
							}
							Log.d("IC", "End Ping pad:"+pad_data+ " new: "+touchContrData[actionPointerId]);
						} else if (iv.getType() == TYPE_SWITCH) {
							if (event.getAction() == MotionEvent.ACTION_DOWN) {
								changeState();
								xoid.getMainHelper().updateViews();
								return true;
							}
						}
					}
				}
			} 
		}
		Emulator.setPadData(pad_data);
		return true;
	}
	*/

	
	/*
	@Override
	protected boolean handleTouchController(MotionEvent event) {

		int action = event.getAction();
		int actionEvent = action & MotionEvent.ACTION_MASK;
		int pid = action >> MotionEvent.ACTION_POINTER_ID_SHIFT;
		
		pad_data = 0;
		
		for (int i = 0; i < event.getPointerCount(); i++) {

			int actionPointerId = event.getPointerId(i);
						
			int x = (int) event.getX(i);
			int y = (int) event.getY(i);
			
			if(actionEvent == MotionEvent.ACTION_UP 
					|| (actionEvent == MotionEvent.ACTION_POINTER_UP && actionPointerId==pid) 
					|| actionEvent == MotionEvent.ACTION_CANCEL)
			{
                //nada
			}
			else
			{		
				for (int j = 0; j < buttons.size(); j++) {
					InputValue iv = buttons.get(j);
										
					if (iv.getRect().contains(x, y)) {
						
						if (iv.getType() == TYPE_ACTION) {
							switch (actionEvent) {
							case MotionEvent.ACTION_DOWN:
							case MotionEvent.ACTION_POINTER_DOWN:
								pad_data |= getButtonValue(iv.getValue());
								break;
							case MotionEvent.ACTION_MOVE:
							    pad_data |= getButtonValue(iv.getValue());
								break;
							}
						} else if (iv.getType() == TYPE_SWITCH) {
							if (event.getAction() == MotionEvent.ACTION_DOWN) {
								changeState();
								xoid.getMainHelper().updateViews();
								return true;
							}
						}
					}
				}
			} 
		}
		
		Emulator.setPadData(pad_data);
		return true;
	}
	*/
	
	@Override
	protected boolean handleTouchController(MotionEvent event) {

		int action = event.getAction();
		int actionEvent = action & MotionEvent.ACTION_MASK;
		int pid = action >> MotionEvent.ACTION_POINTER_ID_SHIFT;
				
		for (int i = 0; i < 10; i++) 
		{
		    touchstates[i] = false;
		    oldtouches[i] = newtouches[i];
		}
		
		for (int i = 0; i < event.getPointerCount(); i++) {

			int actionPointerId = event.getPointerId(i);
						
			int x = (int) event.getX(i);
			int y = (int) event.getY(i);
			
			if(actionEvent == MotionEvent.ACTION_UP 
					|| (actionEvent == MotionEvent.ACTION_POINTER_UP && actionPointerId==pid) 
					|| actionEvent == MotionEvent.ACTION_CANCEL)
			{
                //nada
			}	
			else
			{		
				//int id = i;
				int id = actionPointerId;
				touchstates[id] = true;
				
				for (int j = 0; j < buttons.size(); j++) {
					InputValue iv = buttons.get(j);
										
					if (iv.getRect().contains(x, y)) {
						
						if (iv.getType() == TYPE_ACTION) {
						
							switch (actionEvent) {
							
							case MotionEvent.ACTION_DOWN:
							case MotionEvent.ACTION_POINTER_DOWN:
							case MotionEvent.ACTION_MOVE:
															
								newtouches[id] = getButtonValue(iv.getValue());
					            
								if(oldtouches[id] != newtouches[id])	            
					            	pad_data &= ~(oldtouches[id]);
					            
								pad_data |= newtouches[id];
								break;
							}
						} else if (iv.getType() == TYPE_SWITCH) {
							if (event.getAction() == MotionEvent.ACTION_DOWN) {
								for (int ii = 0; ii < 10; ii++) 
								{
								    touchstates[ii] = false;
								    oldtouches[ii] = 0;
								}
								changeState();
								xoid.getMainHelper().updateViews();
								return true;
							}
						}
					}
				}	                	            
			} 
		}

		for (int i = 0; i < 10; i++) {
			if (!touchstates[i]) {
				boolean really = true;

				for (int j = 0; j < 10 && really; j++) {
					if (j == i)
						continue;
					really = (newtouches[j] & newtouches[i]) == 0;
				}

				if (really)
					pad_data &= ~(newtouches[i]);

				newtouches[i] = 0;
				oldtouches[i] = 0;
			}
		}
		
		
		Emulator.setPadData(pad_data);
		return true;
	}
}
