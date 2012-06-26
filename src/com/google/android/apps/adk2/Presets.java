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

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;

import android.content.Context;
import android.content.SharedPreferences;

public class Presets {
	private ArrayList<Preset> mPresets;
	private final static String kFileName = "foo.dat";

	public static class Preset implements Serializable {
		private static final long serialVersionUID = 8075462052014816810L;
		private String mName;
		private Boolean mAlarmOn;
		private int mAlarmTime;
		private String mAlarmSound;
		private int mVolume;
		private int mColor;
		private int mBrightness;
		private int mDisplay;

		Preset(String name) {
			mName = name;
		}

		public String getName() {
			return mName;
		}

		public void setName(String name) {
			mName = name;
		}

		public String toString() {
			return mName;
		}

		public void applyToPreferences(SharedPreferences thePrefs) {
			SharedPreferences.Editor editor = thePrefs.edit();

			editor.putBoolean(Preferences.PREF_ALARM_ON, mAlarmOn);
			editor.putInt(Preferences.PREF_ALARM_TIME, mAlarmTime);
			editor.putString(Preferences.PREF_ALARM_SOUND, mAlarmSound);

			editor.putInt(Preferences.PREF_VOLUME, mVolume);

			editor.putInt(Preferences.PREF_COLOR, mColor);

			editor.putInt(Preferences.PREF_BRIGHTNESS, mBrightness);

			editor.putInt(Preferences.PREF_DISPLAY, mDisplay);

			editor.commit();
		}

		void extractFromPreferences(SharedPreferences thePrefs) {
			mAlarmOn = thePrefs.getBoolean(Preferences.PREF_ALARM_ON, false);
			mAlarmTime = thePrefs.getInt(Preferences.PREF_ALARM_TIME,
					Preferences.DEFAULT_ALARM_TIME);
			mAlarmSound = thePrefs.getString(Preferences.PREF_ALARM_SOUND, "");

			mVolume = thePrefs.getInt(Preferences.PREF_VOLUME, 0);

			mColor = thePrefs.getInt(Preferences.PREF_COLOR, 0);

			mBrightness = thePrefs.getInt(Preferences.PREF_BRIGHTNESS, 0);

			mDisplay = thePrefs.getInt(Preferences.PREF_DISPLAY, 0);
		}

	}

	public Presets() {
		mPresets = new ArrayList<Preset>();
	}

	public void save(Context context) throws IOException {
		FileOutputStream fos = context.openFileOutput(kFileName,
				Context.MODE_PRIVATE);
		ObjectOutputStream os = new ObjectOutputStream(fos);
		os.writeObject(mPresets);
		os.close();
	}

	@SuppressWarnings("unchecked")
	public void load(Context context) throws IOException,
			ClassNotFoundException {
		FileInputStream fis = context.openFileInput(kFileName);
		ObjectInputStream is = new ObjectInputStream(fis);
		Object readObject = is.readObject();
		if (readObject != null) {
			mPresets = (ArrayList<Preset>) readObject;
		}
	}

	public ArrayList<Preset> getPresets() {
		return mPresets;
	}

	public Preset makeNewPreset(SharedPreferences thePrefs) {
		Preset preset = new Preset("Untitled");
		preset.extractFromPreferences(thePrefs);
		mPresets.add(preset);
		return preset;
	}

}
