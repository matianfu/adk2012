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
import android.preference.PreferenceManager;
import android.widget.RadioGroup;
import android.widget.RadioGroup.OnCheckedChangeListener;

import com.google.android.apps.adk2.Preferences;
import com.google.android.apps.adk2.R;
import com.google.android.apps.adk2.Utilities;

public class DisplayActivity extends Adk2BaseActivity implements
		OnCheckedChangeListener {

	RadioGroup mDisplayGroup;

	int mDisplayOption;

	final int kDisplayButtons[] = { R.id.radio_animation, R.id.radio_accell,
			R.id.radio_mag, R.id.radio_temperature, R.id.radio_hygro,
			R.id.radio_baro, R.id.radio_prox, R.id.radio_color };

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.display);

		mDisplayGroup = (RadioGroup) findViewById(R.id.display_group);
		mDisplayGroup.setOnCheckedChangeListener(this);

		mDisplayOption = PreferenceManager.getDefaultSharedPreferences(this)
				.getInt(Preferences.PREF_DISPLAY, 0);
		updateDisplay();
	}

	public void onCheckedChanged(RadioGroup group, int checkedId) {
		mDisplayOption = Utilities.indexOf(kDisplayButtons, checkedId);
		SharedPreferences.Editor editor = PreferenceManager
				.getDefaultSharedPreferences(this).edit();
		editor.putInt(Preferences.PREF_DISPLAY, mDisplayOption);
		editor.apply();
	}

	void updateDisplay() {
		mDisplayGroup.check(kDisplayButtons[mDisplayOption]);
	}

}
