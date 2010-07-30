
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

package com.seleuco.xpectrum.util;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.BitmapFactory.Options;
import android.os.Build;

public abstract class UnscaledBitmapLoader {

	public static final UnscaledBitmapLoader instance;
	public static boolean canScale = true;

	static {
		canScale = (Integer.parseInt(Build.VERSION.SDK) >= 4);
		instance = Integer.parseInt(Build.VERSION.SDK) < 4 ? new Old() : new New();
	}

	public static Bitmap loadFromResource(Resources resources, int resId,
			BitmapFactory.Options options) {
		return instance.load(resources, resId, options);
	}

	public static void setDensity(Bitmap bmp) {
		instance.setDen(bmp);
	}

	private static class Old extends UnscaledBitmapLoader {
		@Override
		Bitmap load(Resources resources, int resId, Options options) {
			return BitmapFactory.decodeResource(resources, resId, options);
		}

		void setDen(Bitmap bmp) {
		}
	}

	private static class New extends UnscaledBitmapLoader {
		@Override
		Bitmap load(Resources resources, int resId, Options options) {
			if (options == null)
				options = new BitmapFactory.Options();
			options.inScaled = false;
			return BitmapFactory.decodeResource(resources, resId, options);
		}

		@Override
		void setDen(Bitmap bmp) {
			bmp.setDensity(Bitmap.DENSITY_NONE);
		}
	}

	abstract Bitmap load(Resources resources, int resId,
			BitmapFactory.Options options);

	abstract void setDen(Bitmap bmp);
}
