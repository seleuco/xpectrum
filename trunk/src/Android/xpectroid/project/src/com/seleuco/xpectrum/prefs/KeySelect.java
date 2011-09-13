
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
 
   Copyright (c) 2010 Dmitry Rodin
   
   Adapted by Seleuco.
   
*/

package com.seleuco.xpectrum.prefs;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.LinearLayout;


public class KeySelect extends Activity {

	protected int emulatorInputIndex;

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);

		emulatorInputIndex = getIntent().getIntExtra("emulatorInputIndex", 0);
		setTitle("Press button for \""+DefineKeys.emulatorInputLabels[emulatorInputIndex]+"\"");


		final Button chancelButton = new Button(this) {
			{
				setText("Cancel");
				setOnClickListener(new View.OnClickListener() {
					public void onClick(View v) {
						setResult(RESULT_CANCELED, new Intent());
						finish();
					}
				});
			}
		};

		final Button clearButton = new Button(this) {
			{
				setText("Clear");
				setOnClickListener(new View.OnClickListener() {
					public void onClick(View v) {
						setResult(RESULT_OK, new Intent().putExtra("androidKeyCode",  -1));
						finish();
					}
				});
			}
		};

		final View primaryView = new View(this) {
			{
				setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT, 1));
				setFocusable(true);
				setFocusableInTouchMode(true);
				requestFocus();
			}

            /*
			@Override
			public boolean onKeyPreIme (int keyCode, KeyEvent event) {

				setResult(RESULT_OK, new Intent().putExtra("androidKeyCode", keyCode));
				finish();
				return true;
			}
			*/
			@Override
			public boolean onKeyDown (int keyCode, KeyEvent event) {

				setResult(RESULT_OK, new Intent().putExtra("androidKeyCode", keyCode));
				finish();
				return true;
			}
		};

		final LinearLayout parentContainer = new LinearLayout(this) {
			{
				setOrientation(LinearLayout.VERTICAL);
				addView(chancelButton);
				addView(clearButton);
				addView(primaryView);
			}
		};

		setContentView(parentContainer, new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT));

	}

}