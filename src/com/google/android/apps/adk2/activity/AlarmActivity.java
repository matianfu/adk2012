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
package com.google.android.apps.adk2.activity;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.util.Log;

import com.google.android.apps.adk2.ADK;
import com.google.android.apps.adk2.Preferences;
import com.google.android.apps.adk2.R;
import com.google.android.apps.adk2.Utilities;

public class AlarmActivity extends Adk2PreferenceActivity {
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		addPreferencesFromResource(R.xml.alarm_preferences);

		updatePreferencesDisplay();

		HomeActivity h = HomeActivity.get();
		if (h == null) {
			Log.i(ADK.TAG, "Can't find home activity");
		} else {
			Log.i(ADK.TAG, "h = " + h.toString());
		}
	}

	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
			String key) {
		if (Preferences.PREF_ALARM_ON.equals(key)
				|| Preferences.PREF_ALARM_TIME.equals(key)
				|| Preferences.PREF_ALARM_SOUND.equals(key))
			updatePreferencesDisplay();
	}

	void updatePreferencesDisplay() {
		boolean alarmOn = mPreferences.getBoolean(Preferences.PREF_ALARM_ON,
				true);

		Utilities.enablePreference(mPreferenceManager,
				Preferences.PREF_ALARM_TIME, alarmOn);
		Utilities.enablePreference(mPreferenceManager,
				Preferences.PREF_ALARM_SOUND, alarmOn);

		updateAlarmTime();
		updateAlarmSoundList();
	}

	private void updateAlarmTime() {
		int alarmTime = mPreferences.getInt(Preferences.PREF_ALARM_TIME,
				Preferences.DEFAULT_ALARM_TIME);
		String alarmTimeString = Utilities.formatTime(this, alarmTime);
		Preference alarmTimePref = mPreferenceManager
				.findPreference(Preferences.PREF_ALARM_TIME);
		if (alarmTimePref != null) {
			alarmTimePref.setSummary(alarmTimeString);
		}
	}

	private void updateAlarmSoundList() {
		ListPreference alarmSoundPref = (ListPreference) mPreferenceManager
				.findPreference(Preferences.PREF_ALARM_SOUND);
		if (alarmSoundPref != null) {
			HomeActivity h = HomeActivity.get();
			String[] alarmSoundsList = h.getAlarmSounds();
			String defaultAlarmSound = "";
			if (alarmSoundsList.length > 0) {
				defaultAlarmSound = alarmSoundsList[0];
			}
			String alarmSoundString = mPreferences.getString(
					Preferences.PREF_ALARM_SOUND, defaultAlarmSound);
			alarmSoundPref.setEntries(alarmSoundsList);
			alarmSoundPref.setEntryValues(alarmSoundsList);
			alarmSoundPref.setSummary(alarmSoundString);
			alarmSoundPref.setValue(alarmSoundString);
		}
	}
}
