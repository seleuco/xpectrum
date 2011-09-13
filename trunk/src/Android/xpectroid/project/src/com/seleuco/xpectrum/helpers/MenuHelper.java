
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

import android.content.Intent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

import com.seleuco.xpectrum.HelpActivity;
import com.seleuco.xpectrum.R;
import com.seleuco.xpectrum.Xpectroid;
import com.seleuco.xpectrum.input.InputHandler;
import com.seleuco.xpectrum.prefs.UserPreferences;

public class MenuHelper {
	
	protected Xpectroid xoid = null;
	
	public MenuHelper(Xpectroid value){
		xoid = value;
	}
	
	public boolean createOptionsMenu(Menu menu) {
		
		MenuInflater inflater = xoid.getMenuInflater();		
		inflater.inflate(R.menu.menu, menu);        
		
		return true;		
	}
	
	public boolean prepareOptionsMenu(Menu menu) {
		
		return true;
	}

	public boolean optionsItemSelected(MenuItem item) {
	

		switch (item.getItemId()) {
		case (R.id.menu_donate_option):
			xoid.showDialog(DialogHelper.DIALOG_THANKS);
			return true;
		case (R.id.menu_quit_option):
			xoid.showDialog(DialogHelper.DIALOG_EXIT);
			return true;
		case R.id.menu_settings_option:
			Intent i = new Intent(xoid, UserPreferences.class);
			xoid.startActivityForResult(i, MainHelper.SUBACTIVITY_USER_PREFS);
			return true;
		case R.id.menu_help_option:
			Intent i2 = new Intent(xoid, HelpActivity.class);
			xoid.startActivityForResult(i2, MainHelper.SUBACTIVITY_HELP);
			return true;			
		case R.id.vkey_A:
			xoid.getInputHandler().handleVirtualKey(InputHandler.A);
			return true;
		case R.id.vkey_B:
			xoid.getInputHandler().handleVirtualKey(InputHandler.B);
			return true;
		case R.id.vkey_X:
			xoid.getInputHandler().handleVirtualKey(InputHandler.X);
			return true;
		case R.id.vkey_Y:
			xoid.getInputHandler().handleVirtualKey(InputHandler.Y);
			return true;
		case R.id.vkey_MENU:
			xoid.getInputHandler().handleVirtualKey(InputHandler.START);
			return true;
		case R.id.vkey_SELECT:
			xoid.getInputHandler().handleVirtualKey(InputHandler.SELECT);
			return true;
		}

		return false;

	}

}
