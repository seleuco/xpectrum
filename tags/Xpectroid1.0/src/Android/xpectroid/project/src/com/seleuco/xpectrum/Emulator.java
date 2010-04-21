
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

package com.seleuco.xpectrum;

import java.nio.ByteBuffer;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;
import android.view.SurfaceHolder;

public class Emulator 
{
	
    protected static Xpectroid xoid = null;
    
    protected static boolean isEmulating = false;
	
	protected static SurfaceHolder holder = null;
	protected static Bitmap frameBitmap = Bitmap.createBitmap(320, 240, Bitmap.Config.RGB_565);;
	protected static Rect frameRect = null;
	protected static Rect emuRect = null;
	protected static Paint framePaint;
	protected static int width = 320;
	protected static int height = 240;

	protected static AudioTrack audioTrack;
		
	static{
		System.loadLibrary("xpectroid-jni");
	}
	
	public static int getWidth() {
		return width;
	}

	public static int getHeight() {
		return height;
	}
	
	synchronized public static void setHolder(SurfaceHolder value) {
		
		holder = value;
		if(holder!=null)
		{
			holder.setFormat(PixelFormat.OPAQUE);
			//holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
			holder.setKeepScreenOn(true);
		}
	}
	
	public static void setXpectroid(Xpectroid xoid) {
		Emulator.xoid = xoid;
	}
	
	//VIDEO
	public static void setFrameSize(int w, int h) {
		frameRect = new Rect(0,0,w,h);
	}
	
	public static void setEmuSize(boolean cropX, boolean cropY){		
		emuRect = new Rect();
		//256+64,192+48
        if(cropX )
        {
           width = 256+8;
       	   emuRect.left = 32-4;
    	   emuRect.right =  28+256+8;
        }
        else
        {
        	width = 320;
        	emuRect.left = 0;
        	emuRect.right = 320;
        }
        if(cropY)
        {
        	height = 192+8;
        	emuRect.top = 24-4;
        	emuRect.bottom = 20+192+8;
        }
        else
        {
        	height = 240;
        	emuRect.top = 0;
        	emuRect.bottom = 240;
        }
	}
	
	public static void setFrameFiltering(boolean value) {
		
		if(value)
		{
			framePaint = new Paint();
			framePaint.setFilterBitmap(true);
		}
		else
		{
			framePaint = null;
		}
	}

	synchronized static void bitblt(ByteBuffer sScreenBuff, boolean isZX) {
				
		try {

			if (holder==null)
				return;
			
			Canvas canvas = holder.lockCanvas();
			
			sScreenBuff.rewind();
			
			frameBitmap.copyPixelsFromBuffer(sScreenBuff);
			
		    Rect r = null;
			
			if(isZX) r = emuRect;
											
			canvas.drawBitmap(frameBitmap, r, frameRect, framePaint);
						
			holder.unlockCanvasAndPost(canvas);
						
		} catch (Throwable t) {
			Log.getStackTraceString(t);
		}
	}
	
	//SOUND
	static public void initAudio(int freq, boolean stereo)	
	{		
		int sampleFreq = freq;
		
		int channelConfig = stereo ? AudioFormat.CHANNEL_CONFIGURATION_STEREO : AudioFormat.CHANNEL_CONFIGURATION_MONO;
		int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
		int bufferSize = AudioTrack.getMinBufferSize(sampleFreq, channelConfig, audioFormat) * 2;
		
		//System.out.println("Buffer Size "+bufferSize);
		
		audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
				sampleFreq,
				channelConfig,
				audioFormat,
				bufferSize,
				AudioTrack.MODE_STREAM);
		
		audioTrack.play();				
	}
	
	public static void endAudio(){
		audioTrack.stop();
		audioTrack.release();	
		audioTrack = null;
	}
		
	public static void writeAudio(byte[] b, int sz)
	{
		if(audioTrack!=null)
		   audioTrack.write(b, 0, sz);
	}	
	
	
	//LIVE CYCLE
	public static void pause(){
		if(isEmulating)
		   pauseEmulation(true);
		
		if(audioTrack!=null)
		    audioTrack.pause();
	}
	
	public static void resume(){
		
		if(audioTrack!=null)
		    audioTrack.play();
		
		if(isEmulating)
		    pauseEmulation(false);
	}
	
	//EMULATOR
	public static void emulate(final String libPath,final String resPath){
		
		if (isEmulating)return;
		
		Thread t = new Thread(new Runnable(){
			public void run() {
				isEmulating = true;
				init(libPath,resPath);
			}			
		},"emulator-Thread");
		t.start();
	}
	
	//native
	protected static native void init(String libPath,String resPath);
		
	protected static native void pauseEmulation(boolean b);
	
	public static native void setPadData(long data);
	
	public static native boolean isEmulatorInInnerMenu();
	
	public static native void enabledExternalKeyboard(boolean b);
	
	public static native void setKeys(int keys[],int num);
}
