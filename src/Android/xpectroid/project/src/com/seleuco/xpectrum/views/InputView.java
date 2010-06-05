
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

package com.seleuco.xpectrum.views;

import java.util.ArrayList;

import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Paint.Style;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.widget.ImageView;

import com.seleuco.xpectrum.Xpectroid;
import com.seleuco.xpectrum.input.InputHandler;
import com.seleuco.xpectrum.input.InputValue;

public class InputView extends ImageView {
	
	protected Xpectroid xoid = null;
	protected Bitmap bmp = null;
	protected Paint pnt = new Paint();
	protected Rect rsrc = new Rect();
	protected Rect rdst = new Rect();
	protected float dx = 1;
	protected float dy = 1;
	  		 
	public void setXpectroid(Xpectroid xoid) {
		this.xoid = xoid;
	}

	public InputView(Context context) {
		super(context);
		init();
	}

	public InputView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public InputView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}
	
	protected void init(){		
		pnt.setARGB(255, 255, 255, 255);
		//p.setTextSize(25);
	    pnt.setStyle(Style.STROKE);		
	    
		pnt.setARGB(255,255,255,255);
		pnt.setTextSize(16);
	}

	@Override
	public void setImageDrawable(Drawable drawable) {
		BitmapDrawable bmpdrw = (BitmapDrawable)drawable;
		bmp = bmpdrw.getBitmap();
		super.setImageDrawable(drawable);
	}

	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
         
		if(xoid==null)
		{
			super.onMeasure(widthMeasureSpec, heightMeasureSpec);
			return;
		}

		int widthSize  = 1;
		int heightSize = 1;
		
		if(xoid.getMainHelper().getscrOrientation()==Configuration.ORIENTATION_LANDSCAPE)
		{
			widthSize = xoid.getWindowManager().getDefaultDisplay().getWidth();
			heightSize = xoid.getWindowManager().getDefaultDisplay().getHeight();
		}
		else
		{	
			int w = 1;//320;
			int h = 1;//240;
			
		
			if(bmp!=null)
			{
				w = bmp.getWidth();
				h = bmp.getHeight();
			}
			
			if(w==0)w=1;
			if(h==0)h=1;

			float desiredAspect = (float) w / (float) h;
			
			widthSize = xoid.getWindowManager().getDefaultDisplay().getWidth();
			heightSize = (int)(widthSize / desiredAspect);
		}		

		setMeasuredDimension(widthSize, heightSize);
	}
	
	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {

		super.onSizeChanged(w, h, oldw, oldh);
		
		
		int bw = 1;
		int bh = 1;
		
		if(bmp!=null)
		{
			bw = bmp.getWidth();
			bh = bmp.getHeight();	
		}
		
		if(bw==0)bw=1;
		if(bh==0)bh=1;
		
		dx = (float) w / (float) bw;
		dy = (float) h / (float) bh;
		
		if(xoid!=null)
		   xoid.getInputHandler().setFixFactor(dx ,dy);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		
		//debug
        /*
		ArrayList<InputValue> ids = xoid.getInputHandler().getAllInputData();
		Paint p2 = new Paint();
   	    p2.setARGB(255, 255, 255, 255);
		p2.setStyle(Style.STROKE);
		for(int i=0; i<ids.size();i++)
		{
		   Rect r = ids.get(i).getRect();
		   if(r!=null)
		      canvas.drawRect(r, p2);
		}
		*/

		//		
		if(xoid==null)return;
		
		InputHandler inputHandler = xoid.getInputHandler();

		ArrayList<InputValue> keys = inputHandler.getPressedKeys();
				
		if(inputHandler.isShift() && inputHandler.getInputHandlerState() == InputHandler.STATE_SHOWING_KEYBOARD)
		{
			canvas.drawText("Caps ON", 10,  20, pnt);
		}
		
		if(inputHandler.isSymbol() && inputHandler.getInputHandlerState() == InputHandler.STATE_SHOWING_KEYBOARD)
		{
			//canvas.drawText("Symbol ON", 10,  40, pnt);
			
			canvas.drawText("Symbol ON", getWidth() - 90,  20, pnt);
		}
		  
        if(keys.size()!=0)
        {	

		  for(int i=0; i<keys.size(); i++)
		  {	  
			  Rect r = keys.get(i).getRect();
			  
			  if(r==null)continue;
			  
			  int cx = r.centerX();
			  int cy = r.centerY();
			  						 			  	          		  			  			  
			  float ax =  this.dx;
			  float ay =  this.dy;
			  
			  int dx = 13;
			  int dy = 13;
			  
			  int cx_fix = (int)(cx / ax);
			  int cy_fix = (int)(cy / ay);
			  
			  rsrc.top = cy_fix-dy;
			  rsrc.left = cx_fix-dx;
			  
			  rsrc.bottom = cy_fix+dy;		  
			  rsrc.right = cx_fix+dx;
			  
			  int dx2 = (int)(dx * 2.2 * ax) ;
			  int dy2 = (int)(dy * 2.2 * ay) ;
			  
			  rdst.bottom = (int)(cy - dy);	
			  rdst.top = (int)(rdst.bottom - (dy2*2) );
			  rdst.left = (int)(cx - dx2);
			  	  
			  rdst.right = (int)(cx + dx2);
			  
			  canvas.drawBitmap(bmp, rsrc, rdst, null);
			  //canvas.drawRect(r3, p);
		  }
        }		
	}	
}
