/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
package com.google.android.apps.adk2;

import java.util.Date;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.preference.Preference;
import android.preference.PreferenceManager;

public class Utilities {
	public static void centerAround(int x, int y, Drawable d) {
		int w = d.getIntrinsicWidth();
		int h = d.getIntrinsicHeight();
		int left = x - w / 2;
		int top = y - h / 2;
		int right = left + w;
		int bottom = top + h;
		d.setBounds(left, top, right, bottom);
	}

	public static int indexOf(int searchArray[], int itemToFind) {
		for (int i = 0; i < searchArray.length; ++i) {
			if (itemToFind == searchArray[i]) {
				return i;
			}
		}
		return -1;
	}

	public static int indexOf(String searchArray[], String itemToFind) {
		for (int i = 0; i < searchArray.length; ++i) {
			if (itemToFind.equals(searchArray[i])) {
				return i;
			}
		}
		return -1;
	}

	public static int stringToFourCC(String stringCode) {
		int theCode = 0;
		int count = Math.max(stringCode.length(), 4);
		for (int i = 0; i < count; ++i) {
			char c = stringCode.charAt(i);
			theCode <<= 8;
			theCode |= c;
		}
		return theCode;
	}

	public static String fourCCToString(int fourCC) {
		StringBuilder sb = new StringBuilder();
		sb.append('\'');
		sb.append((fourCC & 0xFF000000) >> 24);
		sb.append((fourCC & 0x00FF0000) >> 16);
		sb.append((fourCC & 0x0000FF00) >> 8);
		sb.append((fourCC & 0x000000FF));
		sb.append('\'');
		return sb.toString();
	}

	public static void enablePreference(PreferenceManager preferenceMgr,
			String prefKey, Boolean isEnabled) {
		Preference pref = preferenceMgr.findPreference(prefKey);
		if (pref != null) {
			preferenceMgr.findPreference(prefKey).setEnabled(isEnabled);
		}
	}

	public static String formatTime(Context context, Date dateToFormat) {
		java.text.DateFormat df = android.text.format.DateFormat
				.getTimeFormat(context);
		return df.format(dateToFormat);
	}

	public static String formatTime(Context context, int timeValue) {
		Date date = new Date();
		date.setHours(timeValue / 60);
		date.setMinutes(timeValue % 60);
		return formatTime(context, date);
	}

	public static int dateToTimeValue(int hours, int minutes) {
		return hours * 60 + minutes;
	}

	public static int dateToTimeValue(Date date) {
		return dateToTimeValue(date.getHours(), date.getMinutes());
	}

	private static final byte[] HEX_CHAR = new byte[] { '0', '1', '2', '3',
			'4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	/**
	 * Helper function that dump an array of bytes in hex form
	 * 
	 * @param buffer
	 *            The bytes array to dump
	 * @return A string representation of the array of bytes
	 */
	public static final String dumpBytes(byte[] buffer, int amountToDump) {
		if (buffer == null) {
			return "";
		}

		StringBuffer sb = new StringBuffer();

		amountToDump = Math.min(amountToDump, buffer.length);

		for (int i = 0; i < amountToDump; i++) {
			sb.append("0x")
					.append((char) (HEX_CHAR[(buffer[i] & 0x00F0) >> 4]))
					.append((char) (HEX_CHAR[buffer[i] & 0x000F])).append(" ");
		}

		return sb.toString();
	}

	public static int unsignedByte(byte b) {
		return b & 0xff;
	}

	public static int[] byteArrayToIntArray(byte[] byteArray) {
		int[] intArray = new int[byteArray.length];
		for (int i = 0; i < intArray.length; ++i) {
			intArray[i] = byteArray[i] & 0xff;
		}
		return intArray;
	}
}
