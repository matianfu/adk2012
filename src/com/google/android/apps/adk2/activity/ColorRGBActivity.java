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
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.google.android.apps.adk2.Preferences;
import com.google.android.apps.adk2.R;
import com.google.android.apps.adk2.views.Slider;

public class ColorRGBActivity extends Adk2BaseActivity implements
		Slider.SliderPositionListener, OnSharedPreferenceChangeListener {
	private View mOriginalColorView;
	private View mNewColorView;
	private TextView mTextHex;
	private TextView mTextR;
	private TextView mTextG;
	private TextView mTextB;
	private Slider mSeekBarR;
	private Slider mSeekBarG;
	private Slider mSeekBarB;
	private Drawable mGreen;
	private Drawable mRed;
	private Drawable mBlue;

	private int mOriginalColor;
	private int mNewColor;

	private final int kColorRange = 255;

	private Slider setupSeekBar(int seekBarId, Drawable color) {
		Slider theBar = (Slider) findViewById(seekBarId);
		theBar.setPositionListener(this);
		theBar.setSliderBackground(color);
		return theBar;
	}

	private TextView setupTextField(int textFieldId) {
		TextView theText = (TextView) findViewById(textFieldId);
		return theText;
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.color_rgb);

		Resources res = getResources();
		mRed = res.getDrawable(R.drawable.scrubber_horizontal_red_holo_dark);
		mGreen = res
				.getDrawable(R.drawable.scrubber_horizontal_green_holo_dark);
		mBlue = res.getDrawable(R.drawable.scrubber_horizontal_blue_holo_dark);

		mOriginalColorView = findViewById(R.id.original_color);
		mNewColorView = findViewById(R.id.new_color);
		mTextHex = setupTextField(R.id.color_hex);
		mTextR = setupTextField(R.id.color_r);
		mTextG = setupTextField(R.id.color_g);
		mTextB = setupTextField(R.id.color_b);
		mSeekBarR = setupSeekBar(R.id.seek_bar_r, mRed);
		mSeekBarG = setupSeekBar(R.id.seek_bar_g, mGreen);
		mSeekBarB = setupSeekBar(R.id.seek_bar_b, mBlue);

		mOriginalColor = PreferenceManager.getDefaultSharedPreferences(this)
				.getInt(Preferences.PREF_COLOR, Preferences.PREF_DEFAULT_COLOR);
		mNewColor = mOriginalColor;

		SharedPreferences preferences = PreferenceManager
				.getDefaultSharedPreferences(this);
		preferences.registerOnSharedPreferenceChangeListener(this);

		updateDisplay();
	}

	public void onPositionChange(Slider slider, double value) {
		switch (slider.getId()) {
		case R.id.seek_bar_r:
			mNewColor = (mNewColor & 0xff00ffff)
					| ((int) (value * kColorRange) << 16);
			break;

		case R.id.seek_bar_g:
			mNewColor = (mNewColor & 0xffff00ff)
					| ((int) (value * kColorRange) << 8);
			break;

		case R.id.seek_bar_b:
			mNewColor = (mNewColor & 0xffffff00) | (int) (value * kColorRange);
			break;
		}

		persistPreferences();
		updateDisplay();
	}

	public void onStartTrackingTouch(SeekBar seekBar) {
	}

	public void onStopTrackingTouch(SeekBar seekBar) {
	}

	private void persistPreferences() {
		SharedPreferences.Editor editor = PreferenceManager
				.getDefaultSharedPreferences(this).edit();
		editor.putInt(Preferences.PREF_COLOR, mNewColor);
		editor.apply();
	}

	private void updateDisplay() {
		mOriginalColorView.setBackgroundColor(mOriginalColor | 0xff000000);
		mNewColorView.setBackgroundColor(mNewColor | 0xff000000);

		int r, g, b;
		r = (mNewColor >> 16) & 0xff;
		g = (mNewColor >> 8) & 0xff;
		b = mNewColor & 0xff;

		mTextHex.setText(String.format("%06X", mNewColor & 0xffffff));
		mTextR.setText(String.format("%d", r));
		mTextG.setText(Integer.toString(g));
		mTextB.setText(Integer.toString(b));

		mSeekBarR.setPosition((double) r / (double) kColorRange);
		mSeekBarG.setPosition((double) g / (double) kColorRange);
		mSeekBarB.setPosition((double) b / (double) kColorRange);
	}

	@Override
	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
			String key) {
		if (Preferences.PREF_COLOR.equals(key)) {
			mOriginalColor = sharedPreferences.getInt(Preferences.PREF_COLOR,
					Preferences.PREF_DEFAULT_COLOR);
			mNewColor = mOriginalColor;
			updateDisplay();
		}

	}

}
