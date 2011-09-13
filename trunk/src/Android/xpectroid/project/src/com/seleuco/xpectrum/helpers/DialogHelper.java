
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

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;

import com.seleuco.xpectrum.Xpectroid;

public class DialogHelper {
	
	public final static int DIALOG_EXIT = 1;
	public final static int DIALOG_ERROR = 2;
	public final static int DIALOG_INFO = 3;
	public final static int DIALOG_THANKS = 6;
	public final static int DIALOG_FULLSCREEN = 7;	
	
	protected Xpectroid xoid = null;
	
	protected String errorMsg;
	protected String infoMsg;
	
	public void setErrorMsg(String errorMsg) {
		this.errorMsg = errorMsg;
	}

	public void setInfoMsg(String infoMsg) {
		this.infoMsg = infoMsg;
	}
		
	public DialogHelper(Xpectroid value){
		xoid = value;
	}
	
	public Dialog createDialog(int id) {
	    Dialog dialog;
	    AlertDialog.Builder builder = new AlertDialog.Builder(xoid);
	    switch(id) {
	    case DIALOG_EXIT:
	    	
	    	builder.setMessage("Are you sure you want to exit?")
	    	       .setCancelable(false)
	    	       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
	    	           public void onClick(DialogInterface dialog, int id) {
	    	                //System.exit(0);
	    	                android.os.Process.killProcess(android.os.Process.myPid());   
	    	           }
	    	       })
	    	       .setNegativeButton("No", new DialogInterface.OnClickListener() {
	    	           public void onClick(DialogInterface dialog, int id) {
	    	                dialog.cancel();
	    	           }
	    	       });
	    	dialog = builder.create();
	        break;
	    case DIALOG_ERROR:
	    	builder.setMessage("Error")
	    	       .setCancelable(false)
	    	       .setPositiveButton("OK", new DialogInterface.OnClickListener() {
	    	           public void onClick(DialogInterface dialog, int id) {
	    	                System.exit(0);
	    	           }
	    	       });

	    	 dialog = builder.create();
	         break;
	    case DIALOG_INFO:
	    	builder.setMessage("Info")
	    	       .setCancelable(false)
	    	       .setPositiveButton("OK", new DialogInterface.OnClickListener() {
	    	           public void onClick(DialogInterface dialog, int id) {
	    	           }
	    	       });

	    	 dialog = builder.create();
	         break;
	    case DIALOG_THANKS:
	    	builder.setMessage("All my projects come to you free of charge and ad-free, because that's how i like my work to be, but if you want to thank me for my effort or colaborate in future developments, feel free to support my projects.")
	    	       .setCancelable(false)
	    	       .setPositiveButton("OK", new DialogInterface.OnClickListener() {
	    	           public void onClick(DialogInterface dialog, int id) {
	    	        	   xoid.getMainHelper().showDonate();
	    	           }
	    	       });

	    	 dialog = builder.create();
	         break;	         
	    default:
	        dialog = null;
	    }
	    return dialog;

	}


	public void prepareDialog(int id, Dialog dialog) {
		
		if(id==DIALOG_ERROR)
		{
			((AlertDialog)dialog).setMessage(errorMsg);
		}
		else if(id==DIALOG_INFO)
		{
			((AlertDialog)dialog).setMessage(infoMsg);
		}
	}
        
}
