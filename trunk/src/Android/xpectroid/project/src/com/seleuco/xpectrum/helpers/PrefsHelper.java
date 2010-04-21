
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

package com.seleuco.xpectrum.helpers;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.preference.PreferenceManager;

import com.seleuco.xpectrum.Xpectroid;

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
	
	final static public String  PREF_PORTRAIT_CROP_X = "PREF_PORTRAIT_CROP_X";
	final static public String  PREF_PORTRAIT_CROP_Y = "PREF_PORTRAIT_CROP_Y";
	final static public String  PREF_LANDSCAPE_CROP_X = "PREF_LANDSCAPE_CROP_X";
	final static public String  PREF_LANDSCAPE_CROP_Y = "PREF_LANDSCAPE_CROP_Y";
	
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
		return getSharedPreferences().getBoolean(PREF_PORTRAIT_CROP_X,true);
	}
	
	public boolean isPortraitCropY(){
		return getSharedPreferences().getBoolean(PREF_PORTRAIT_CROP_Y,false);
	}
	
	public boolean isLandscapeCropX(){
		return getSharedPreferences().getBoolean(PREF_LANDSCAPE_CROP_X,false);
	}
	
	public boolean isLandscapeCropY(){
		return getSharedPreferences().getBoolean(PREF_LANDSCAPE_CROP_Y,false);
	}
}
