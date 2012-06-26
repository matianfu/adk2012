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

import android.graphics.Color;

public final class Preferences {

	// Preference Keys
	public static final String PREF_ALARM_ON = "alarm_on";
	public static final String PREF_ALARM_TIME = "alarm_time";
	public static final String PREF_ALARM_SOUND = "alarm_sound";
	public static final String PREF_COLOR = "color";
	public static final String PREF_DISPLAY = "display";
	public static final String PREF_BRIGHTNESS = "brightness";
	public static final String PREF_VOLUME = "volume";

	public static final String PREF_TIME = "time";

	public static final String PREF_COLOR_SENSOR = "color_sensor";
	public static final String PREF_LOCKED = "locked";
	
	public static final String PREF_LICENSE_TEXT = "license_text";

	public static final String[] SETTINGS_PREFERENCES = { PREF_ALARM_ON,
			PREF_ALARM_TIME, PREF_COLOR, PREF_BRIGHTNESS, PREF_VOLUME };

	// Defaults
	public static final int PREF_DEFAULT_COLOR = Color.argb(255, 128, 64, 32);
	public static final int DEFAULT_ALARM_TIME = 390; // 390 = 6:30am

}
