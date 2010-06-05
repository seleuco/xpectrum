package com.seleuco.xpectrum.prefs;

import android.content.Context;
import android.content.res.TypedArray;
import android.preference.DialogPreference;
import android.util.AttributeSet;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.seleuco.xpectrum.R;

public class TrackBallPrefScreen extends DialogPreference implements
		SeekBar.OnSeekBarChangeListener {
	private SeekBar seekBar;
	private TextView valueView;
	private int minValue = 1, maxValue = 10;
	private int oldValue, newValue;

	
	public TrackBallPrefScreen(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		//minValue = attrs.getAttributeIntValue("", "minValue", 1);
		//maxValue = attrs.getAttributeIntValue("", "maxValue", 10);
		
		setDialogLayoutResource(R.layout.bar);
		setPositiveButtonText("Save");
		setNegativeButtonText("Cancel");
	}

	@Override
	protected void onBindDialogView(View view) {
		super.onBindDialogView(view);
		if (newValue < minValue)
			newValue = minValue;
		if (newValue > maxValue)
			newValue = maxValue;
		seekBar = (SeekBar) view.findViewById(R.id.seekbar);
		seekBar.setMax(maxValue - minValue);
		seekBar.setProgress(newValue - minValue);
		seekBar.setSecondaryProgress(newValue - minValue);
		seekBar.setOnSeekBarChangeListener(this);
		valueView = (TextView) view.findViewById(R.id.value);
		valueView.setText(Integer.toString(newValue));
	}

	public void onProgressChanged(SeekBar seekBar, int progress,
			boolean fromUser) {
		newValue = progress + minValue;
		valueView.setText(Integer.toString(newValue));
	}

	public void onStartTrackingTouch(SeekBar seekBar) {
	}

	public void onStopTrackingTouch(SeekBar seekBar) {
	}

	@Override
	protected void onDialogClosed(boolean positiveResult) {
		super.onDialogClosed(positiveResult);
		if (!positiveResult)
			newValue = oldValue;
		else {
			
			//InputHandler.trackballSensitivity = newValue;
			
			oldValue = newValue;
			persistInt(newValue);
		}
	}

	@Override
	protected Object onGetDefaultValue(TypedArray a, int index) {
		return a.getInteger(index, 0);
	}

	@Override
	protected void onSetInitialValue(boolean restoreValue, Object defaultValue) {
		oldValue = (restoreValue ? getPersistedInt(0)
				: ((Integer) defaultValue).intValue());
		newValue = oldValue;
	}
}