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

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.os.Message;
import android.view.MenuItem;
import android.view.View;

public class Adk2BaseActivity extends Activity {

	private boolean mWasPolling = false;

	static void maybeDisplayHomeAsUp(Activity activity) {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB)
			activity.getActionBar().setDisplayHomeAsUpEnabled(true);
	}

	static boolean maybeHandleHomeMenuItem(MenuItem item, Activity activity) {
		switch (item.getItemId()) {
		case android.R.id.home:
			activity.finish();
			return true;

		default:
			return false;
		}
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Adk2BaseActivity.maybeDisplayHomeAsUp(this);

		HomeActivity h = HomeActivity.get();
		if (h != null) {
			mWasPolling = h.startPollingSettings();
		}

	}

	@Override
	public void onDestroy() {
		HomeActivity h = HomeActivity.get();
		if (h != null && !mWasPolling) {
			h.stopPollingSettings();
		}
		super.onDestroy();
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		return Adk2BaseActivity.maybeHandleHomeMenuItem(item, this);
	}

	public void onClick(View v) {
		// TODO Auto-generated method stub
		
	}

	public boolean handleMessage(Message msg) {
		// TODO Auto-generated method stub
		return false;
	}

	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
			String key) {
		// TODO Auto-generated method stub
		
	}
}
