
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

package com.seleuco.xpectrum.views;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.SurfaceHolder.Callback;

import com.seleuco.xpectrum.Emulator;
import com.seleuco.xpectrum.Xpectroid;


public class EmulatorView extends SurfaceView implements Callback{
	
	final public static int MODE_ORIGINAL = 1;	
	final public static int MODE_SCALE = 2;
	final public static int MODE_STRETCH = 3;
	
	protected int scaleType = MODE_ORIGINAL;
	
	protected Xpectroid xoid = null;

	public int getScaleType() {
		return scaleType;
	}

	public void setScaleType(int scaleType) {
		this.scaleType = scaleType;
	}

	public void setXpectroid(Xpectroid xoid) {
		this.xoid = xoid;
	}
	
	public EmulatorView(Context context) {
		super(context);
		init();
	}

	public EmulatorView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public EmulatorView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();

	}
	
	protected void init(){
		this.getHolder().addCallback(this);
		this.requestFocus();
		this.setFocusableInTouchMode(true);
	}
		
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
   
		int widthSize = 1;
		int heightSize = 1;
		
	
		if (scaleType == MODE_STRETCH)// FILL ALL
		{
			widthSize = MeasureSpec.getSize(widthMeasureSpec);
			heightSize = MeasureSpec.getSize(heightMeasureSpec);
		} else if (scaleType == MODE_ORIGINAL || scaleType == MODE_SCALE) {
			
			int emu_w = Emulator.getWidth();
		    int emu_h = Emulator.getHeight();
		    
			int w = emu_w;
			int h = emu_h;

			widthSize = MeasureSpec.getSize(widthMeasureSpec);
			heightSize = MeasureSpec.getSize(heightMeasureSpec);
			
			if(heightSize==0)heightSize=1;
			if(widthSize==0)widthSize=1;

			float scale = 1.0f;

			if (scaleType == MODE_SCALE)
				scale = Math.min((float) widthSize / (float) w,
						(float) heightSize / (float) h);

			w = (int) (w * scale);
			h = (int) (h * scale);

			float desiredAspect = (float) emu_w / (float) emu_h;

			widthSize = Math.min(w, widthSize);
			heightSize = Math.min(h, heightSize);

			float actualAspect = (float) (widthSize / heightSize);

			if (Math.abs(actualAspect - desiredAspect) > 0.0000001) {

				boolean done = false;

				// Try adjusting width to be proportional to height
				int newWidth = (int) (desiredAspect * heightSize);

				if (newWidth <= widthSize) {
					widthSize = newWidth;
					done = true;
				}

				// Try adjusting height to be proportional to width
				if (!done) {
					int newHeight = (int) (widthSize / desiredAspect);
					if (newHeight <= heightSize) {
						heightSize = newHeight;
					}
				}
			}
		}

		setMeasuredDimension(widthSize, heightSize);
	}
		
	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {

		super.onSizeChanged(w, h, oldw, oldh);
		Emulator.setFrameSize(w, h);		
	}

	//@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		Emulator.setHolder(holder);		
	}

	//@Override
	public void surfaceCreated(SurfaceHolder holder) {
		Emulator.setHolder(holder);		
	}

	//@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		Emulator.setHolder(null);		
	}
}
