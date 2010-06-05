
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

import android.app.ListActivity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import com.seleuco.xpectrum.R;
import com.seleuco.xpectrum.input.InputHandler;

public class DefineKeys extends ListActivity {
	
	public static final String[] androidKeysLabels = { "UNKNOWN", "SOFT_LEFT",
		"SOFT_RIGHT", "HOME", "BACK", "CALL", "ENDCALL", "0", "1", "2",
		"3", "4", "5", "6", "7", "8", "9", "STAR", "POUND", "DPAD_UP",
		"DPAD_DOWN", "DPAD_LEFT", "DPAD_RIGHT", "DPAD_CENTER", "VOLUME_UP",
		"VOLUME_DOWN", "POWER", "CAMERA", "CLEAR", "A", "B", "C", "D", "E",
		"F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R",
		"S", "T", "U", "V", "W", "X", "Y", "Z", "COMMA", "PERIOD",
		"ALT_LEFT", "ALT_RIGHT", "SHIFT_LEFT", "SHIFT_RIGHT", "TAB",
		"SPACE", "SYM", "EXPLORER", "ENVELOPE", "ENTER", "DEL", "GRAVE",
		"MINUS", "EQUALS", "LEFT_BRACKET", "RIGHT_BRACKET", "BACKSLASH",
		"SEMICOLON", "APOSTROPHE", "SLASH", "AT", "NUM", "HEADSETHOOK",
		"FOCUS", "PLUS", "MENU", "NOTIFICATION", "SEARCH",
		"MEDIA_PLAY_PAUSE", "MEDIA_STOP", "MEDIA_NEXT", "MEDIA_PREVIOUS",
		"MEDIA_REWIND", "MEDIA_FAST_FORWARD", "MUTE" };
	
	public static final String[] emulatorInputLabels = {
        "STICK UP","STICK DOWN","STICK LEFT","STICK RIGHT",
        "STICK B","STICK X","STICK A","STICK Y",
        "STICK L1","STICK R1","STICK SELECT","STICK START",
		"SPECKEY 0", "SPECKEY 1", "SPECKEY 2", "SPECKEY 3", 
		"SPECKEY 4", "SPECKEY 5", "SPECKEY 6", "SPECKEY 7",
		"SPECKEY 8", "SPECKEY 9","SPECKEY A", "SPECKEY B", 
		"SPECKEY C", "SPECKEY D", "SPECKEY E", "SPECKEY F", 
		"SPECKEY G", "SPECKEY H", "SPECKEY I", "SPECKEY J",
		"SPECKEY K", "SPECKEY L", "SPECKEY M","SPECKEY N",
		"SPECKEY O", "SPECKEY P", "SPECKEY Q", "SPECKEY R", 
		"SPECKEY S", "SPECKEY T", "SPECKEY U", "SPECKEY V", 
		"SPECKEY W", "SPECKEY X", "SPECKEY Y", "SPECKEY Z", 
		"SPECKEY SPACE","SPECKEY ENTER","SPECKEY CAPS SHIFT","SPECKEY SYMBOL_SHIFT",
		"SPECKEY DEL"
	};

	protected int emulatorInputIndex = 0;

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_BLUR_BEHIND,
				WindowManager.LayoutParams.FLAG_BLUR_BEHIND);

		
		drawListAdapter();
	}

	private void drawListAdapter() {
		final Context context = this;

		ArrayAdapter<String> keyLabelsAdapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_list_item_1, DefineKeys.emulatorInputLabels) {
			@Override
			public View getView(final int position, final View convertView,
					final ViewGroup parent) {
				return new Modified(context, getItem(position), position);
			}
		};

		setListAdapter(keyLabelsAdapter);
	}

	@Override
	public void onListItemClick(ListView parent, View v, int position, long id) {
		emulatorInputIndex = position;
		startActivityForResult(new Intent(this, KeySelect.class).putExtra(
				"emulatorInputIndex", emulatorInputIndex), 0);
	}

	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);

		if (resultCode == RESULT_OK && requestCode == 0) {
			int androidKeyCode = data.getIntExtra("androidKeyCode", 0);
			for (int i = 0; i < InputHandler.keyMapping.length; i++)
				if (InputHandler.keyMapping[i] == androidKeyCode)
					InputHandler.keyMapping[i] = -1;
			
		   InputHandler.keyMapping[emulatorInputIndex] = androidKeyCode;
		}
		drawListAdapter();
	}
}

class Modified extends LinearLayout {



	public Modified(final Context context, final String keyLabel,
			final int position) {
		super(context);

		if (keyLabel != null) {

			setOrientation(HORIZONTAL);

			final TextView textView = new TextView(context);
			textView.setTextAppearance(context, R.style.ListText);

			final TextView textView2 = new TextView(context);
			textView2.setTextAppearance(context, R.style.ListTextSmall);

			textView.setText(keyLabel);
			textView.setPadding(10, 0, 0, 0);

			textView2.setText("?");

			if (InputHandler.keyMapping[position] != -1)
				textView2
						.setText(DefineKeys.androidKeysLabels[InputHandler.keyMapping[position]]);

			textView2.setGravity(Gravity.RIGHT);
			textView2.setPadding(0, 0, 10, 0);

			addView(textView, new LinearLayout.LayoutParams(
					LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
			addView(textView2, new LinearLayout.LayoutParams(
					LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT));

		} else {

			final View hiddenView = new View(context);
			hiddenView.setVisibility(INVISIBLE);
			addView(hiddenView, new LinearLayout.LayoutParams(0, 0));

		}
	}
}

