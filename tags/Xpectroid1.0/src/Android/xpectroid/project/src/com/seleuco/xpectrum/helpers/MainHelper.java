
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

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Environment;
import android.view.Display;
import android.view.View;

import com.seleuco.xpectrum.Emulator;
import com.seleuco.xpectrum.R;
import com.seleuco.xpectrum.Xpectroid;
import com.seleuco.xpectrum.input.InputHandler;
import com.seleuco.xpectrum.views.EmulatorView;
import com.seleuco.xpectrum.views.InputView;

public class MainHelper {
	
	final static public  int SUBACTIVITY_USER_PREFS = 1;
	final static public  int SUBACTIVITY_AD = 2;
	final static public  int BUFFER_SIZE = 1024*48;
	
	final static public  String ROMS_DIR = "/ROMs/Xpectroid/";
	final static public  String MAGIC_FILE = "dont-delete-00001.bin";
	
	protected Xpectroid xoid = null;
	
	public MainHelper(Xpectroid value){
		xoid = value;
	}
	
	public String getLibDir(){	
		String cache_dir, lib_dir;
		try {
			cache_dir = xoid.getCacheDir().getCanonicalPath();				
			lib_dir = cache_dir.replace("cache", "lib");
		} catch (Exception e) {
			e.printStackTrace();
			lib_dir = "/data/data/com.seleuco.xpectrum/lib";
		}
		return lib_dir;
	}
	
	public String getResDir()
	{
		String res_dir;
		try {
			res_dir = Environment.getExternalStorageDirectory().getCanonicalPath()+ROMS_DIR;
		} catch (IOException e) {
			
			e.printStackTrace();
			res_dir = "/sdcard"+ROMS_DIR;
		}
		return res_dir;
	}
	
	public boolean ensureResDir(){
		File res_dir = new File(getResDir());
		
		boolean created = false;
		
		if(res_dir.exists() == false)
		{
			if(!res_dir.mkdirs())
			{
				xoid.getDialogHelper().setErrorMsg("Can't find/create:\n '"+getResDir()+"'");
				xoid.showDialog(DialogHelper.DIALOG_ERROR);
				return false;				
			}
			else
			{
               created= true;
			}
		}
		
		String str_sav_dir = getResDir()+"saves/";
		File sav_dir = new File(str_sav_dir);
		if(sav_dir.exists() == false)
		{
			
			if(!sav_dir.mkdirs())
			{
				xoid.getDialogHelper().setErrorMsg("Can't find/create:\n'"+str_sav_dir+"'");
				xoid.showDialog(DialogHelper.DIALOG_ERROR);
				return false;				
			}
		}
		
		if(created)
		{
			xoid.getDialogHelper().setInfoMsg("Created: \n'"+getResDir()+"'\nPut your sna, tzx, tap, sp, dsk, z80 files on it!.");
			xoid.showDialog(DialogHelper.DIALOG_INFO);			
		}
		
		return true;		
	}
	
	public void copyFiles(){
		
		try {
			
			File fm = new File(getResDir()+ File.separator + "saves/" + MAGIC_FILE);
			if(fm.exists())
				return;
			
			fm.createNewFile();
			
			// Create a ZipInputStream to read the zip file
			BufferedOutputStream dest = null;
			InputStream fis = xoid.getResources().openRawResource(R.raw.games);
			ZipInputStream zis = new ZipInputStream(

			new BufferedInputStream(fis));
			// Loop over all of the entries in the zip file
			int count;
			byte data[] = new byte[BUFFER_SIZE];
			ZipEntry entry;
			while ((entry = zis.getNextEntry()) != null) {
				if (!entry.isDirectory()) {

					String destination = this.getResDir();
					String destFN = destination + File.separator + entry.getName();
					// Write the file to the file system
					FileOutputStream fos = new FileOutputStream(destFN);
					dest = new BufferedOutputStream(fos, BUFFER_SIZE);
					while ((count = zis.read(data, 0, BUFFER_SIZE)) != -1) {
						dest.write(data, 0, count);
					}
					dest.flush();
					dest.close();
				}
				else
				{
					File f = new File(getResDir()+ File.separator + entry.getName());
					f.mkdirs();
				}
				
			}
			zis.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public int getscrOrientation() {
		Display getOrient = xoid.getWindowManager().getDefaultDisplay();
		//int orientation = getOrient.getOrientation();
		
		int orientation  = xoid.getResources().getConfiguration().orientation;


		// Sometimes you may get undefined orientation Value is 0
		// simple logic solves the problem compare the screen
		// X,Y Co-ordinates and determine the Orientation in such cases
		if (orientation == Configuration.ORIENTATION_UNDEFINED) {

			Configuration config = xoid.getResources().getConfiguration();
			orientation = config.orientation;

			if (orientation == Configuration.ORIENTATION_UNDEFINED) {
				// if height and widht of screen are equal then
				// it is square orientation
				if (getOrient.getWidth() == getOrient.getHeight()) {
					orientation = Configuration.ORIENTATION_SQUARE;
				} else { // if widht is less than height than it is portrait
					if (getOrient.getWidth() < getOrient.getHeight()) {
						orientation = Configuration.ORIENTATION_PORTRAIT;
					} else { // if it is not any of the above it will defineitly
								// be landscape
						orientation = Configuration.ORIENTATION_LANDSCAPE;
					}
				}
			}
		}
		return orientation; // return value 1 is portrait and 2 is Landscape
							// Mode
	}
	
	public void updateViews(){
		
		EmulatorView emuView =  xoid.getEmuView();
		InputView inputView =  xoid.getInputView();
		InputHandler inputHandler = xoid.getInputHandler();
		PrefsHelper prefshelper = xoid.getPrefsHelper();
		
		int state = xoid.getInputHandler().getInputHandlerState();
		
		if(this.getscrOrientation() == Configuration.ORIENTATION_PORTRAIT)
		{
			emuView.setScaleType(prefshelper.getPortraitScaleMode());	
			Emulator.setFrameFiltering(prefshelper.isPortraitBitmapFiltering());
			
			if(state == InputHandler.STATE_SHOWING_CONTROLLER && !prefshelper.isPortraitTouchController())
				inputHandler.changeState();
			
			if(state == InputHandler.STATE_SHOWING_KEYBOARD && !prefshelper.isPortraitTouchKeyboard())
				inputHandler.changeState();		
			
			if(state == InputHandler.STATE_SHOWING_NONE && (prefshelper.isPortraitTouchKeyboard() && prefshelper.isPortraitTouchController()))
			    inputHandler.changeState();	
			
			state = xoid.getInputHandler().getInputHandlerState();
			
			if(state == InputHandler.STATE_SHOWING_NONE)
			{	
				inputView.setVisibility(View.GONE);
			}	
			else
			{	
			    inputView.setVisibility(View.VISIBLE);
			}   

			if(state == InputHandler.STATE_SHOWING_CONTROLLER)
			{			    	
			    											
			   	inputView.setImageDrawable(xoid.getResources().getDrawable(R.drawable.controller_hs0));
			    	
			   	inputHandler.readControllerValues(R.raw.controller_hs0);
			}
			else if(state == InputHandler.STATE_SHOWING_KEYBOARD)
			{
			    				    				    	
			   	inputView.setImageDrawable(xoid.getResources().getDrawable(R.drawable.keyboard_hs0));
			    	
			  	inputHandler.readKeyboardValues(R.raw.keyboard_hs0);
			}
			
			Emulator.setEmuSize(prefshelper.isPortraitCropX(), prefshelper.isPortraitCropY());
		}
		else
		{
			emuView.setScaleType(xoid.getPrefsHelper().getLandscapeScaleMode());
			Emulator.setFrameFiltering(xoid.getPrefsHelper().isLandscapeBitmapFiltering());
			
			if(state == InputHandler.STATE_SHOWING_CONTROLLER && !prefshelper.isLandscapeTouchController())
				inputHandler.changeState();
			
			if(state == InputHandler.STATE_SHOWING_KEYBOARD && !prefshelper.isLandscapeTouchKeyboard())
				inputHandler.changeState();		
			
			if(state == InputHandler.STATE_SHOWING_NONE && (prefshelper.isLandscapeTouchKeyboard() && prefshelper.isLandscapeTouchController()))
			    inputHandler.changeState();	
			
			state = xoid.getInputHandler().getInputHandlerState();
			
			if(state == InputHandler.STATE_SHOWING_NONE)
			{	
				inputView.setVisibility(View.GONE);
			}	
			else
			{	
			    inputView.setVisibility(View.VISIBLE);
			}   

			if(state == InputHandler.STATE_SHOWING_CONTROLLER)
			{			    	
			    											
			   	inputView.setImageDrawable(xoid.getResources().getDrawable(R.drawable.controller_fs0));
			    	
			   	inputHandler.readControllerValues(R.raw.controller_fs0);
			}
			else if(state == InputHandler.STATE_SHOWING_KEYBOARD)
			{
			    				    				    	
			   	inputView.setImageDrawable(xoid.getResources().getDrawable(R.drawable.keyboard_fs0));
			    	
			  	inputHandler.readKeyboardValues(R.raw.keyboard_fs0);
			}
			
			Emulator.setEmuSize(prefshelper.isLandscapeCropX(), prefshelper.isLandscapeCropY());
		}
		
		int op = inputHandler.getOpacity();
		if (op != -1 && (state == InputHandler.STATE_SHOWING_CONTROLLER || state == InputHandler.STATE_SHOWING_KEYBOARD))
			inputView.setAlpha(op);
		
		emuView.invalidate();
		emuView.requestLayout();
		 
		inputView.invalidate();
		inputView.requestLayout();	  

	}
	
	public void activityResult(int requestCode, int resultCode, Intent data) {
		
		if(requestCode == SUBACTIVITY_USER_PREFS)
		    updateViews();
	}
	
	public boolean isLite(){
		return xoid.getApplicationContext().getPackageName().equals("com.seleuco.xpectrum_lite");
	}	
}
