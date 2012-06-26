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
import android.os.Handler;
import android.os.Handler.Callback;
import android.os.Message;
import android.preference.PreferenceManager;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

import com.google.android.apps.adk2.Preferences;
import com.google.android.apps.adk2.R;
import com.google.android.apps.adk2.views.ColorSensorView;

public class ColorSensorActivity extends Adk2BaseActivity implements
		OnSharedPreferenceChangeListener, OnClickListener, Callback {

	private Handler mColorPollingHandler;
	private ColorSensorView mColorSensorView;
	private Button mCaptureButton;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		SharedPreferences preferences = PreferenceManager
				.getDefaultSharedPreferences(this);
		preferences.registerOnSharedPreferenceChangeListener(this);

		setContentView(R.layout.color_sensor);
		mColorSensorView = (ColorSensorView) findViewById(R.id.color_sensor_view);
		mCaptureButton = (Button) findViewById(R.id.color_sample_button);
		mCaptureButton.setOnClickListener(this);

		mColorPollingHandler = new Handler(this);
		getColorFromSensor();
		Message msg = mColorPollingHandler.obtainMessage();
		mColorPollingHandler.sendMessageDelayed(msg, 500);
	}

	@Override
	public void onDestroy() {
		mColorPollingHandler = null;
		super.onDestroy();
	}

	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
			String key) {
		if (Preferences.PREF_COLOR_SENSOR.equals(key))
			updatePreferencesDisplay(sharedPreferences);
	}

	private void updatePreferencesDisplay(SharedPreferences sharedPreferences) {
		mColorSensorView.setSensedColor(sharedPreferences.getInt(
				Preferences.PREF_COLOR_SENSOR, 0) | 0xff000000);
		getColorFromSensor();

	}

	private void getColorFromSensor() {
		HomeActivity h = HomeActivity.get();
		if (h != null) {
			h.getSensors();
		}
	}

	@Override
	public void onClick(View v) {
		SharedPreferences preferences = PreferenceManager
				.getDefaultSharedPreferences(this);
		SharedPreferences.Editor editor = preferences.edit();
		int newColor = preferences.getInt(Preferences.PREF_COLOR_SENSOR,
				0xFFFFFF);
		editor.putInt(Preferences.PREF_COLOR, newColor);
		editor.commit();
		finish();
	}


	public boolean handleMessage(Message msg) {
		getColorFromSensor();
		if (mColorPollingHandler != null) {
			Message newMsg = mColorPollingHandler.obtainMessage();
			mColorPollingHandler.sendMessageDelayed(newMsg, 500);
		}
		return true;
	}

}
