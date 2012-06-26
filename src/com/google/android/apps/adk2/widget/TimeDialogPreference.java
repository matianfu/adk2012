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
package com.google.android.apps.adk2.widget;

import java.util.Date;

import android.content.Context;
import android.preference.DialogPreference;
import android.util.AttributeSet;
import android.view.View;
import android.widget.TimePicker;

import com.google.android.apps.adk2.Preferences;
import com.google.android.apps.adk2.R;
import com.google.android.apps.adk2.Utilities;

public class TimeDialogPreference extends DialogPreference {
	private TimePicker mTimePicker;

	public TimeDialogPreference(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	@Override
	protected void onBindDialogView(View view) {
		super.onBindDialogView(view);
		mTimePicker = (TimePicker) view.findViewById(R.id.time_picker);

		int time = 0;
		if (getKey().equals(Preferences.PREF_ALARM_TIME)) {
			time = getPersistedInt(Preferences.DEFAULT_ALARM_TIME);
		} else {
			Date d = new Date();
			time = Utilities.dateToTimeValue(d);
		}
		mTimePicker.setCurrentHour(time / 60);
		mTimePicker.setCurrentMinute(time % 60);
	}

	@Override
	protected void onDialogClosed(boolean positiveResult) {
		super.onDialogClosed(positiveResult);

		if (positiveResult) {
			persistInt(Utilities.dateToTimeValue(mTimePicker.getCurrentHour(),
					mTimePicker.getCurrentMinute()));
		}
	}
}
