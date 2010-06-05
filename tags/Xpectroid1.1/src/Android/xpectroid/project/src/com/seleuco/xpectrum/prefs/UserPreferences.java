
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

package com.seleuco.xpectrum.prefs;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;

import com.seleuco.xpectrum.R;
import com.seleuco.xpectrum.helpers.PrefsHelper;
import com.seleuco.xpectrum.input.InputHandler;

public class UserPreferences extends PreferenceActivity implements OnSharedPreferenceChangeListener {
	
	private SharedPreferences settings;

    protected ListPreference mPrefPortraitMode;
    protected ListPreference mPrefLandsMode;
    protected ListPreference mPrefLandsControllerType;

	@Override
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		
		
		addPreferencesFromResource(R.xml.userpreferences);
		
		settings = PreferenceManager.getDefaultSharedPreferences(this);
		
        mPrefPortraitMode = (ListPreference)getPreferenceScreen().findPreference(PrefsHelper.PREF_PORTRAIT_SCALING_MODE);
        mPrefLandsMode = (ListPreference)getPreferenceScreen().findPreference(PrefsHelper.PREF_LANDSCAPE_SCALING_MODE);
        mPrefLandsControllerType = (ListPreference)getPreferenceScreen().findPreference(PrefsHelper.PREF_LANDSCAPE_CONTROLLER_TYPE);
    
	}
	
	  @Override
	    protected void onResume() {
	        super.onResume();
	        
	        // Setup the initial values
	        //mCheckBoxPreference.setSummary(sharedPreferences.getBoolean(key, false) ? "Disable this setting" : "Enable this setting");
	        mPrefPortraitMode.setSummary("Current value is '" + mPrefPortraitMode.getEntry()+"'"); 
	        mPrefLandsMode.setSummary("Current value is '" + mPrefLandsMode.getEntry()+"'"); 
	        mPrefLandsControllerType.setSummary("Current value is '" + mPrefLandsControllerType.getEntry()+"'"); 
	        
	        // Set up a listener whenever a key changes            
	        getPreferenceScreen().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
	    }

	    @Override
	    protected void onPause() {
	        super.onPause();

	        // Unregister the listener whenever a key changes            
	        getPreferenceScreen().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);    
	    }
	    
	    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
	        // Let's do something a preference value changes
	    	/*
	        if (key.equals(KEY_CHECKBOX_PREFERENCE)) {
	          mCheckBoxPreference.setSummary(sharedPreferences.getBoolean(key, false) ? "Disable this setting" : "Enable this setting");
	        }
	        else*/ 
	        if (key.equals(PrefsHelper.PREF_PORTRAIT_SCALING_MODE)) 
	        {
	            mPrefPortraitMode.setSummary("Current value is '" + mPrefPortraitMode.getEntry()+"'"); 
	        }
	        else if(key.equals(PrefsHelper.PREF_LANDSCAPE_SCALING_MODE))
	        {
	        	mPrefLandsMode.setSummary("Current value is '" + mPrefLandsMode.getEntry()+"'");	
	        }
	        else if(key.equals(PrefsHelper.PREF_LANDSCAPE_CONTROLLER_TYPE))
	        {	
	            mPrefLandsControllerType.setSummary("Current value is '" + mPrefLandsControllerType.getEntry()+"'");
	        }
	        
	    }

		@Override
		public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
				Preference pref) {
			
			if (pref.getKey().equals("defineKeys")) {
				startActivityForResult(new Intent(this, DefineKeys.class), 1);
			}                              
			else if (pref.getKey().equals("defaultsKeys")) {

				 AlertDialog.Builder builder = new AlertDialog.Builder(this);
			    	builder.setMessage("Are you sure to restore?")
		    	       .setCancelable(false)
		    	       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
		    	           public void onClick(DialogInterface dialog, int id) {
		    					SharedPreferences.Editor editor =  settings.edit();
		    					
		    					StringBuffer definedKeysStr = new StringBuffer(); 
		    					
		    					for(int i=0; i< InputHandler.defaultKeyMapping.length;i++)
		    					{	
		    						InputHandler.keyMapping[i] = InputHandler.defaultKeyMapping[i];
		    						definedKeysStr.append(InputHandler.defaultKeyMapping[i]+":");
		    					}
		    					editor.putString(PrefsHelper.PREF_DEFINED_KEYS, definedKeysStr.toString());
		    					editor.commit();
		    					//finish();
		    				
		    	           }
		    	       })
		    	       .setNegativeButton("No", new DialogInterface.OnClickListener() {
		    	           public void onClick(DialogInterface dialog, int id) {
		    	                dialog.cancel();
		    	           }
		    	       });
			    	Dialog dialog = builder.create();
			    	dialog.show();
			}
			
			return super.onPreferenceTreeClick(preferenceScreen, pref);
		}
	    
		@Override
		protected void onActivityResult(int requestCode, int resultCode, Intent data) {
			super.onActivityResult(requestCode, resultCode, data);
			
			if (resultCode == RESULT_OK && requestCode == 0) {
				setResult(RESULT_OK, data);
			} 
			else if (requestCode == 1) {
				SharedPreferences.Editor editor =  settings.edit();
					
				StringBuffer definedKeysStr = new StringBuffer(); 
				
				for(int i=0; i< InputHandler.keyMapping.length;i++)
					definedKeysStr.append(InputHandler.keyMapping[i]+":");
				
				editor.putString(PrefsHelper.PREF_DEFINED_KEYS, definedKeysStr.toString());
				editor.commit();
				return;
			}
			finish();
		}
}
