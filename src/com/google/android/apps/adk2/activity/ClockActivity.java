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

import java.util.Date;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.preference.Preference;

import com.google.android.apps.adk2.Preferences;
import com.google.android.apps.adk2.R;
import com.google.android.apps.adk2.Utilities;

public class ClockActivity extends Adk2PreferenceActivity {

	private Handler mHandler = new Handler();

	private Runnable mUpdateTimeTask;

	final static int kUpdateTime = 500;
	int mHours = -1;
	int mMinutes = -1;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.xml.clock_preferences);

		updatePreferencesDisplay();

		mUpdateTimeTask = new Runnable() {
			public void run() {
				updatePreferencesDisplay();
				mHandler.postDelayed(mUpdateTimeTask, kUpdateTime);
			}
		};
		mHandler.postDelayed(mUpdateTimeTask, kUpdateTime);
	}

	@Override
	protected void onPause() {
		mHandler.removeCallbacks(mUpdateTimeTask);
		super.onPause();
	}

	@Override
	protected void onResume() {
		mHandler.postDelayed(mUpdateTimeTask, kUpdateTime);
		super.onResume();
	}

	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
			String key) {
	}

	private void updatePreferencesDisplay() {
		Preference clockTimePref = mPreferenceManager
				.findPreference(Preferences.PREF_TIME);
		if (clockTimePref != null) {
			Date d = new Date();
			if (d.getHours() != mHours || d.getMinutes() != mMinutes) {
				mHours = d.getHours();
				mMinutes = d.getMinutes();
				clockTimePref.setSummary(Utilities.formatTime(this, d));
			}
		}
	}
}
