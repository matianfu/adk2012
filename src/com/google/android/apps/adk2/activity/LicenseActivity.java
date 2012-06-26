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
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.widget.TextView;

import com.google.android.apps.adk2.Preferences;
import com.google.android.apps.adk2.R;

public class LicenseActivity extends Adk2BaseActivity implements
		OnSharedPreferenceChangeListener {
	private TextView mLicenseTextView;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.licenses);

		mLicenseTextView = (TextView) findViewById(R.id.license_text);

		SharedPreferences preferences = PreferenceManager
				.getDefaultSharedPreferences(this);
		preferences.registerOnSharedPreferenceChangeListener(this);
		updateDisplay(preferences);
	}

	@Override
	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
			String key) {

		if (Preferences.PREF_LICENSE_TEXT.equals(key)) {
			updateDisplay(sharedPreferences);
		}
	}

	private void updateDisplay(SharedPreferences sharedPreferences) {
		String licenseText = sharedPreferences.getString(
				Preferences.PREF_LICENSE_TEXT, "");
		mLicenseTextView.setText(licenseText);
	}
}
