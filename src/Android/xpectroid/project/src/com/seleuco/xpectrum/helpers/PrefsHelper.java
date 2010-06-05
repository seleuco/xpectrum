
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

package com.seleuco.xpectrum.helpers;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.preference.PreferenceManager;
import android.view.Display;

import com.seleuco.xpectrum.Xpectroid;
import com.seleuco.xpectrum.input.InputHandler;

public class PrefsHelper implements OnSharedPreferenceChangeListener
{
	
	final static public String PREF_PORTRAIT_SCALING_MODE = "PREF_PORTRAIT_SCALING_MODE";
	final static public String PREF_PORTRAIT_TOUCH_CONTROLLER = "PREF_PORTRAIT_TOUCH_CONTROLLER";
	final static public String PREF_PORTRAIT_TOUCH_KEYBOARD = "PREF_PORTRAIT_TOUCH_KEYBOARD";
	final static public String PREF_PORTRAIT_BITMAP_FILTERING = "PREF_PORTRAIT_BITMAP_FILTERING";
	
	final static public String PREF_LANDSCAPE_SCALING_MODE = "PREF_LANDSCAPE_SCALING_MODE";
	final static public String PREF_LANDSCAPE_TOUCH_CONTROLLER = "PREF_LANDSCAPE_TOUCH_CONTROLLER";
	final static public String PREF_LANDSCAPE_TOUCH_KEYBOARD = "PREF_LANDSCAPE_TOUCH_KEYBOARD";
	final static public String PREF_LANDSCAPE_BITMAP_FILTERING = "PREF_LANDSCAPE_BITMAP_FILTERING";
	final static public String PREF_LANDSCAPE_CONTROLLER_TYPE = "PREF_LANDSCAPE_CONTROLLER_TYPE";
	
	final static public String  PREF_PORTRAIT_CROP_X = "PREF_PORTRAIT_CROP_X_v2";
	final static public String  PREF_PORTRAIT_CROP_Y = "PREF_PORTRAIT_CROP_Y";
	final static public String  PREF_LANDSCAPE_CROP_X = "PREF_LANDSCAPE_CROP_X_v2";
	final static public String  PREF_LANDSCAPE_CROP_Y = "PREF_LANDSCAPE_CROP_Y_v2";
	
	final static public String  PREF_DEFINED_KEYS = "PREF_DEFINED_KEYS";
	
	final static public String  PREF_TRACKBALL_SENSITIVITY = "PREF_TRACKBALL_SENSITIVITY";
	final static public String  PREF_TRACKBALL_NOMOVE = "PREF_TRACKBALL_NOMOVE";
	
	protected Xpectroid xoid = null;
	
	public PrefsHelper(Xpectroid value){
		xoid = value;
	}


	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
			String key) {
	}
	
	public void resume() {
		Context context = xoid.getApplicationContext();
		SharedPreferences prefs =
			  PreferenceManager.getDefaultSharedPreferences(context);
			prefs.registerOnSharedPreferenceChangeListener(this);
	}	

	public void pause() {

		Context context = xoid.getApplicationContext();
		SharedPreferences prefs =
			  PreferenceManager.getDefaultSharedPreferences(context);
			prefs.unregisterOnSharedPreferenceChangeListener(this);
	}
	
	protected SharedPreferences getSharedPreferences(){
		Context context = xoid.getApplicationContext();
		return PreferenceManager.getDefaultSharedPreferences(context);
	}

	public int getPortraitScaleMode(){
		return Integer.valueOf(getSharedPreferences().getString(PREF_PORTRAIT_SCALING_MODE,"2")).intValue();	
	}
	
	public int getLandscapeScaleMode(){
		return Integer.valueOf(getSharedPreferences().getString(PREF_LANDSCAPE_SCALING_MODE,"2")).intValue();	
	}
	
	public boolean isPortraitTouchController(){
		return getSharedPreferences().getBoolean(PREF_PORTRAIT_TOUCH_CONTROLLER,true);
	}
	
	public boolean isPortraitTouchKeyboard(){
		return getSharedPreferences().getBoolean(PREF_PORTRAIT_TOUCH_KEYBOARD,true);
	}
	
	public boolean isPortraitBitmapFiltering(){
		return getSharedPreferences().getBoolean(PREF_PORTRAIT_BITMAP_FILTERING,false);
	}	

	public boolean isLandscapeTouchController(){
		return getSharedPreferences().getBoolean(PREF_LANDSCAPE_TOUCH_CONTROLLER,true);
	}
	
	public boolean isLandscapeTouchKeyboard(){
		return getSharedPreferences().getBoolean(PREF_LANDSCAPE_TOUCH_KEYBOARD,true);
	}
	
	public boolean isLandscapeBitmapFiltering(){
		return getSharedPreferences().getBoolean(PREF_LANDSCAPE_BITMAP_FILTERING,false);
	}
	
	public int getLandscapeControllerType(){
		return Integer.valueOf(getSharedPreferences().getString(PREF_LANDSCAPE_CONTROLLER_TYPE,"1")).intValue();	
	}

	public boolean isPortraitCropX(){
		//HVGA 480x320 -> No crop
		Display d = xoid.getWindowManager().getDefaultDisplay();
		boolean def = true;
		if((d.getHeight()==480 && d.getWidth()==320) || (d.getHeight()==320 && d.getWidth()==480))
		    def = false;//Esta guarreria la hago por culpa de los tontos que no ven la opcion de crop
		
		return getSharedPreferences().getBoolean(PREF_PORTRAIT_CROP_X,def);
	}
	
	public boolean isPortraitCropY(){
		return getSharedPreferences().getBoolean(PREF_PORTRAIT_CROP_Y,false);
	}
	
	public boolean isLandscapeCropX(){
		return getSharedPreferences().getBoolean(PREF_LANDSCAPE_CROP_X,true);
	}
	
	public boolean isLandscapeCropY(){
		return getSharedPreferences().getBoolean(PREF_LANDSCAPE_CROP_Y,true);
	}
	
	public String getDefinedKeys(){
		
		SharedPreferences p = getSharedPreferences();
		
		StringBuffer defaultKeys = new StringBuffer(); 
		
		for(int i=0; i< InputHandler.defaultKeyMapping.length;i++)
			defaultKeys.append(InputHandler.defaultKeyMapping[i]+":");
			
		return p.getString(PREF_DEFINED_KEYS, defaultKeys.toString());
		
	}
	
	public int getTrackballSensitivity(){
		//return Integer.valueOf(getSharedPreferences().getString(PREF_TRACKBALL_SENSITIVITY,"3")).intValue();	
		return getSharedPreferences().getInt(PREF_TRACKBALL_SENSITIVITY,3);
	}
	
	public boolean isTrackballNoMove(){
		return getSharedPreferences().getBoolean(PREF_TRACKBALL_NOMOVE,false);
	}

	
}
