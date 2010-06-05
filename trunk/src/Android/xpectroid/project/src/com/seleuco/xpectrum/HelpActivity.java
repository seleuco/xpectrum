
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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import android.app.Activity;
import android.content.res.Resources.NotFoundException;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.admob.android.ads.AdManager;

public class HelpActivity extends Activity {

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch(keyCode)
		{		
		    case KeyEvent.KEYCODE_BACK:		
				return true;
		    case KeyEvent.KEYCODE_HOME:		
				return true;
		}
		
		return false;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);

		
		setContentView(R.layout.help);
		
		AdManager.setTestDevices( new String[] { AdManager.TEST_EMULATOR } );

		View adVw = this.findViewById(R.id.ad);
		adVw.setVisibility(View.VISIBLE);
		final Button button = (Button) findViewById(R.id.ButtonContinue);
		button.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				finish();
			}
		});
		TextView v = (TextView)this.findViewById(R.id.TextView01);
		//v.setText(R.string.help);
		try {
			v.setText(this.convertStreamToString(getResources().openRawResource(R.raw.readme)));
		} catch (NotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
		}
	}
	
	public String convertStreamToString(InputStream is) throws IOException {

		if (is != null) {
			StringBuilder sb = new StringBuilder();
			String line;

			try {
				BufferedReader reader = new BufferedReader(
						new InputStreamReader(is, "UTF-8"));
				while ((line = reader.readLine()) != null) {
					sb.append(line).append("\n");
				}
			} finally {
				is.close();
			}
			return sb.toString();
		} else {
			return "";
		}
	}
}
