
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

package com.seleuco.xpectrum;

import android.app.Activity;
import android.app.Dialog;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;

import com.seleuco.xpectrum.helpers.DialogHelper;
import com.seleuco.xpectrum.helpers.MainHelper;
import com.seleuco.xpectrum.helpers.MenuHelper;
import com.seleuco.xpectrum.helpers.PrefsHelper;
import com.seleuco.xpectrum.input.InputHandler;
import com.seleuco.xpectrum.input.InputHandlerFactory;
import com.seleuco.xpectrum.views.EmulatorView;
import com.seleuco.xpectrum.views.InputView;

public class Xpectroid extends Activity {
		
	protected EmulatorView emuView = null;
	protected InputView inputView = null;
	
	protected MainHelper mainHelper = null;
	protected MenuHelper menuHelper = null;
	protected PrefsHelper prefsHelper = null;
	protected DialogHelper dialogHelper = null;
	
	protected InputHandler inputHandler = null;
	
	public MenuHelper getMenuHelper() {
		return menuHelper;
	}
    	
    public PrefsHelper getPrefsHelper() {
		return prefsHelper;
	}
    
    public MainHelper getMainHelper() {
		return mainHelper;
	}
    
    public DialogHelper getDialogHelper() {
		return dialogHelper;
	}
    
	public EmulatorView getEmuView() {
		return emuView;
	}

	public InputView getInputView() {
		return inputView;
	}

    public InputHandler getInputHandler() {
		return inputHandler;
	}

	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS,WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);
        
        setContentView(R.layout.main);
        
        prefsHelper = new PrefsHelper(this);
        
        dialogHelper  = new DialogHelper(this);
        
        mainHelper = new MainHelper(this);
        
        if(!mainHelper.ensureResDir())
        	return;
        
        mainHelper.copyFiles();
        
        menuHelper = new MenuHelper(this);
        
        
        //inputHandler = new InputHandler(this);
        inputHandler = InputHandlerFactory.createInputHandler(this);
        
        emuView = (EmulatorView) this.findViewById(R.id.EmulatorView);
        inputView = (InputView) this.findViewById(R.id.InputView);
        
        emuView.setXpectroid(this);
        inputView.setXpectroid(this);
        
        Emulator.setXpectroid(this);        
        
        mainHelper.updateViews();
               
        emuView.setOnKeyListener(inputHandler);
        emuView.setOnTouchListener(inputHandler);
        inputView.setOnTouchListener(inputHandler);	  
        
        if(mainHelper.getscrOrientation() == Configuration.ORIENTATION_LANDSCAPE)
        {	        
        	View frame = this.findViewById(R.id.EmulatorFrame);
	        frame.setOnTouchListener(inputHandler);        	
        }
                
        Emulator.emulate(mainHelper.getLibDir(),mainHelper.getResDir());     
        
        /* 
        if(!mainHelper.isLite())
        {
			Intent i = new Intent(this, HelpActivity.class);
			startActivityForResult(i, MainHelper.SUBACTIVITY_HELP);
        }
        */
        
    }
    
	//MENU STUFF
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {		
		
		if(menuHelper!=null)
		{
		   if(menuHelper.createOptionsMenu(menu))return true;
		}  
		
		return super.onCreateOptionsMenu(menu);
	}

	@Override
	public boolean onPrepareOptionsMenu(Menu menu) {
		if(menuHelper!=null)
		{	
		   if(menuHelper.prepareOptionsMenu(menu)) return true;
		}   
		return super.onPrepareOptionsMenu(menu); 
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if(menuHelper!=null)
		{
		   if(menuHelper.optionsItemSelected(item))
			   return true;
		}
		return super.onOptionsItemSelected(item);
	}

	//ACTIVITY
    @Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		if(mainHelper!=null)
		   mainHelper.activityResult(requestCode, resultCode, data);
	}
	
	//LIVE CYCLE
	@Override
	protected void onResume() {
		super.onResume();
		if(prefsHelper!=null)
		   prefsHelper.resume();
		Emulator.resume();
		//System.out.println("OnResume");
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		if(prefsHelper!=null)
		   prefsHelper.pause();
		Emulator.pause();
		//System.out.println("OnPause");
	}
	
	@Override
	protected void onStart() {
		super.onStart();
		//System.out.println("OnStart");
	}

	@Override
	protected void onStop() {
		super.onStop();
		//System.out.println("OnStop");
	}

	//Dialog Stuff
	@Override
	protected Dialog onCreateDialog(int id) {
		if(dialogHelper!=null)
		{	
			Dialog d = dialogHelper.createDialog(id);
			if(d!=null)return d;
		}
		return super.onCreateDialog(id);		
	}

	@Override
	protected void onPrepareDialog(int id, Dialog dialog) {
		super.onPrepareDialog(id, dialog);
		if(dialogHelper!=null)
		   dialogHelper.prepareDialog(id, dialog);
	}
        
}