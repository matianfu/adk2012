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

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.zip.GZIPInputStream;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Handler.Callback;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import com.android.future.usb.UsbAccessory;
import com.android.future.usb.UsbManager;
import com.google.android.apps.adk2.ADK;
import com.google.android.apps.adk2.BTConnection;
import com.google.android.apps.adk2.Connection;
import com.google.android.apps.adk2.Preferences;
import com.google.android.apps.adk2.R;
import com.google.android.apps.adk2.UsbConnection;
import com.google.android.apps.adk2.Utilities;

public class HomeActivity extends Activity implements OnClickListener,
		Callback, OnSharedPreferenceChangeListener, Runnable {

	private Handler mDeviceHandler;
	private Handler mSettingsPollingHandler;

	private UsbManager mUSBManager;
	private SharedPreferences mPreferences;

	private ByteArrayOutputStream mLicenseTextStream;

	private byte[] mSettingsBuffer = null;
	private byte[] mSettingsPayload = new byte[8];
	private byte[] mQueryBuffer = new byte[4];
	private byte[] mEmptyPayload = new byte[0];

	Connection mConnection;

	UsbAccessory mAccessory;

	private boolean mIgnorePrefChanges = false;
	private boolean mPollSettings = false;

	private long mIgnoreUpdatesUntil = System.currentTimeMillis();

	private String mLicenseText = "";
	private ArrayList<String> mSoundFiles;

	static final String TUNES_FOLDER = "/Tunes";

	static final byte CMD_GET_PROTO_VERSION = 1; // () -> (u8 protocolVersion)
	static final byte CMD_GET_SENSORS = 2; // () -> (sensors:
											// i32,i32,i32,i32,u16,u16,u16,u16,u16,u16,u16,i16,i16,i16,i16,i16,i16)
	static final byte CMD_FILE_LIST = 3; // FIRST: (char name[]) -> (fileinfo or
											// single zero byte) OR NONLATER: ()
											// -> (fileinfo or empty or single
											// zero byte)
	static final byte CMD_FILE_DELETE = 4; // (char name[0-255)) -> (char
											// success)
	static final byte CMD_FILE_OPEN = 5; // (char name[0-255]) -> (char success)
	static final byte CMD_FILE_WRITE = 6; // (u8 data[]) -> (char success)
	static final byte CMD_FILE_CLOSE = 7; // () -> (char success)
	static final byte CMD_GET_UNIQ_ID = 8; // () -> (u8 uniq[16])
	static final byte CMD_BT_NAME = 9; // (char name[]) -> () OR () -> (char
										// name[])
	static final byte CMD_BT_PIN = 10; // (char PIN[]) -> () OR () -> (char
										// PIN[])
	static final byte CMD_TIME = 11; // (timespec) -> (char success)) OR () >
										// (timespec)
	static final byte CMD_SETTINGS = 12; // () ->
											// (alarm:u8,u8,u8,brightness:u8,color:u8,u8,u8:volume:u8)
											// or
											// (alarm:u8,u8,u8,brightness:u8,color:u8,u8,u8:volume:u8)
											// > (char success)
	static final byte CMD_ALARM_FILE = 13; // () -> (char file[0-255]) OR (char
											// file[0-255]) > (char success)
	static final byte CMD_GET_LICENSE = 14; // () -> (u8 licensechunk[]) OR ()
											// if last sent
	static final byte CMD_DISPLAY_MODE = 15; // () -> (u8) OR (u8) -> ()
	static final byte CMD_LOCK = 16; // () -> (u8) OR (u8) -> ()

	private static final boolean gLogPackets = false;

	static final int DIALOG_NO_PRESETS_ID = 0;

	private static HomeActivity sHomeActivity = null;

	private static String curBtName = "<UNKNOWN>";

	public static HomeActivity get() {
		return sHomeActivity;
	}

	public boolean startPollingSettings() {
		boolean wasPolling = mPollSettings;
		mPollSettings = true;
		if (!wasPolling) {
			pollSettings();
		}
		return wasPolling;
	}

	public void stopPollingSettings() {
		mPollSettings = false;
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.home);

		mDeviceHandler = new Handler(this);
		mSettingsPollingHandler = new Handler(this);

		mPreferences = PreferenceManager.getDefaultSharedPreferences(this);
		mPreferences.registerOnSharedPreferenceChangeListener(this);

		mUSBManager = UsbManager.getInstance(this);

		mSoundFiles = new ArrayList<String>();

		setUpButton(R.id.clock_button, R.drawable.ic_clock, R.string.clock);
		setUpButton(R.id.alarm_button, R.drawable.ic_alarm, R.string.alarm);
		setUpButton(R.id.volume_button, R.drawable.ic_volume, R.string.volume);
		setUpButton(R.id.color_button, R.drawable.ic_color, R.string.color);
		setUpButton(R.id.brightness_button, R.drawable.ic_brightness,
				R.string.brightness);
		setUpButton(R.id.display_button, R.drawable.ic_display,
				R.string.display);
		setUpButton(R.id.presets_button, R.drawable.ic_presets,
				R.string.presets);
		updateLockDisplay();

		connectToAccessory();

		startLicenseUpload();
		sHomeActivity = this;

		startPollingSettings();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.home_menu, menu);
		return true;
	}

	@Override
	protected Dialog onCreateDialog(int id) {
		Dialog dialog = null;
		if (id == DIALOG_NO_PRESETS_ID) {
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setMessage("Unsupported feature.");
			dialog = builder.create();
		}
		return dialog;
	}

	private void changeBtName() {

		// This example shows how to add a custom layout to an AlertDialog
		LayoutInflater factory = LayoutInflater.from(this);
		final View textEntryView = factory.inflate(R.layout.alert_dialog, null);
		final EditText e = (EditText) textEntryView
				.findViewById(R.id.btname_edit);

		AlertDialog ad = new AlertDialog.Builder(this)
				.setIconAttribute(android.R.attr.alertDialogIcon)
				.setTitle("Set ADK Bluetooth Name")
				.setView(textEntryView)
				.setPositiveButton(R.string.set_bt_name_ok,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog,
									int whichButton) {

								curBtName = e.getText().toString();
								if (curBtName.equals(""))
									curBtName = "ADK 2012";

								byte b[] = null;
								try {
									b = curBtName.getBytes("UTF-8");
								} catch (UnsupportedEncodingException e1) {
									// well aren't you SOL....
									e1.printStackTrace();
								}
								byte b2[] = new byte[b.length + 1];
								for (int i = 0; i < b.length; i++)
									b2[i] = b[i];
								b2[b.length] = 0;

								sendCommand(CMD_BT_NAME, CMD_BT_NAME, b2);
							}
						})
				.setNegativeButton(R.string.set_bt_name_cancel,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog,
									int whichButton) {

								// user cancels
							}
						}).create();

		e.setText(curBtName);
		ad.show();
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.license_menu:
			showLicenses();
			return true;

		case R.id.disconnect_menu:
			disconnect();
			return true;

		case R.id.change_bt_name:
			changeBtName();
			return true;

		default:
			return false;
		}
	}

	private void disconnect() {
		finish();
	}

	private void showLicenses() {
		Intent showLicenseIntent = new Intent(this, LicenseActivity.class);
		startActivity(showLicenseIntent);
	}

	private void startLicenseUpload() {
		Log.i(ADK.TAG, "startLicenseUpload");
		mLicenseTextStream = new ByteArrayOutputStream();
		sendCommand(CMD_GET_LICENSE, 33);
	}

	private void pollSettings() {
		if (mPollSettings) {
			sendCommand(CMD_SETTINGS, CMD_SETTINGS);
			sendCommand(CMD_DISPLAY_MODE, CMD_DISPLAY_MODE);
			sendCommand(CMD_LOCK, CMD_LOCK);
			Message msg = mSettingsPollingHandler.obtainMessage(99);
			if (!mSettingsPollingHandler.sendMessageDelayed(msg, 500)) {
				Log.e(ADK.TAG, "faled to queue settings message");
			}
		}
	}

	@Override
	public void onPause() {
		super.onPause();
	}

	@Override
	public void onResume() {
		super.onResume();
		pollSettings();
	}

	@Override
	public void onDestroy() {
		sHomeActivity = null;
		if (mConnection != null) {
			try {
				mConnection.close();
			} catch (IOException e) {
			} finally {
				mConnection = null;
			}
		}
		super.onDestroy();
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.clock_button:
			startActivity(new Intent(this, ClockActivity.class));
			break;

		case R.id.alarm_button:
			startActivity(new Intent(this, AlarmActivity.class));
			break;

		case R.id.volume_button:
			startActivity(new Intent(this, VolumeActivity.class));
			break;

		case R.id.color_button:
			startActivity(new Intent(this, ColorActivity.class));
			break;

		case R.id.brightness_button:
			startActivity(new Intent(this, BrightnessActivity.class));
			break;

		case R.id.display_button:
			startActivity(new Intent(this, DisplayActivity.class));
			break;

		case R.id.presets_button:
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB)
				startActivity(new Intent(this, PresetsActivity.class));
			else
				showDialog(DIALOG_NO_PRESETS_ID);
			break;

		case R.id.lock_button:
			toggleLock();
			break;
		}
	}

	private void toggleLock() {
		boolean isLocked = mPreferences.getBoolean(Preferences.PREF_LOCKED,
				false);
		boolean newLocked = !isLocked;
		SharedPreferences.Editor editor = mPreferences.edit();
		editor.putBoolean(Preferences.PREF_LOCKED, newLocked);
		editor.commit();
	}

	private void setUpButton(int buttonID, int iconID, int labelID) {
		View button = findViewById(buttonID);
		button.setOnClickListener(this);
		((ImageView) button.findViewById(R.id.icon)).setImageResource(iconID);
		((TextView) button.findViewById(R.id.label)).setText(labelID);
	}

	public void connectToAccessory() {
		// bail out if we're already connected
		if (mConnection != null)
			return;

		if (getIntent().hasExtra(BTDeviceListActivity.EXTRA_DEVICE_ADDRESS)) {
			String address = getIntent().getStringExtra(
					BTDeviceListActivity.EXTRA_DEVICE_ADDRESS);
			Log.i(ADK.TAG, "want to connect to " + address);
			mConnection = new BTConnection(address);
			performPostConnectionTasks();
		} else {
			// assume only one accessory (currently safe assumption)
			UsbAccessory[] accessories = mUSBManager.getAccessoryList();
			UsbAccessory accessory = (accessories == null ? null
					: accessories[0]);
			if (accessory != null) {
				if (mUSBManager.hasPermission(accessory)) {
					openAccessory(accessory);
				} else {
					// synchronized (mUsbReceiver) {
					// if (!mPermissionRequestPending) {
					// mUsbManager.requestPermission(accessory,
					// mPermissionIntent);
					// mPermissionRequestPending = true;
					// }
					// }
				}
			} else {
				// Log.d(TAG, "mAccessory is null");
			}
		}

	}

	public void disconnectFromAccessory() {
		closeAccessory();
	}

	private void openAccessory(UsbAccessory accessory) {
		mConnection = new UsbConnection(this, mUSBManager, accessory);
		performPostConnectionTasks();
	}

	private void performPostConnectionTasks() {
		sendCommand(CMD_GET_PROTO_VERSION, CMD_GET_PROTO_VERSION);
		sendCommand(CMD_SETTINGS, CMD_SETTINGS);
		sendCommand(CMD_BT_NAME, CMD_BT_NAME);
		sendCommand(CMD_ALARM_FILE, CMD_ALARM_FILE);
		listDirectory(TUNES_FOLDER);

		Thread thread = new Thread(null, this, "ADK 2012");
		thread.start();
	}

	public void closeAccessory() {
		try {
			mConnection.close();
		} catch (IOException e) {
		} finally {
			mConnection = null;
		}
	}

	public void run() {
		int ret = 0;
		byte[] buffer = new byte[16384];
		int bufferUsed = 0;

		while (ret >= 0) {
			try {
				ret = mConnection.getInputStream().read(buffer, bufferUsed,
						buffer.length - bufferUsed);
				bufferUsed += ret;
				int remainder = process(buffer, bufferUsed);
				if (remainder > 0) {
					System.arraycopy(buffer, remainder, buffer, 0, bufferUsed
							- remainder);
					bufferUsed = remainder;
				} else {
					bufferUsed = 0;
				}
			} catch (IOException e) {
				break;
			}
		}
		Intent connectIntent = new Intent(this, ConnectActivity.class);
		connectIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		startActivity(connectIntent);
	}

	public int process(byte[] buffer, int bufferUsed) {
		if (gLogPackets) {
			Log.i(ADK.TAG,
					"read " + bufferUsed + " bytes: "
							+ Utilities.dumpBytes(buffer, bufferUsed));
		}
		ByteArrayInputStream inputStream = new ByteArrayInputStream(buffer, 0,
				bufferUsed);
		ProtocolHandler ph = new ProtocolHandler(mDeviceHandler, inputStream);
		ph.process();
		return inputStream.available();
	}

	public void listDirectory(String path) {
		mSoundFiles.clear();
		byte[] payload = new byte[path.length() + 1];
		for (int i = 0; i < path.length(); ++i) {
			payload[i] = (byte) path.charAt(i);
		}
		payload[path.length()] = 0;
		sendCommand(CMD_FILE_LIST, CMD_FILE_LIST, payload);
	}

	public void getSensors() {
		sendCommand(CMD_GET_SENSORS, CMD_GET_SENSORS);
	}

	public byte[] sendCommand(int command, int sequence, byte[] payload,
			byte[] buffer) {
		int bufferLength = payload.length + 4;
		if (buffer == null || buffer.length < bufferLength) {
			Log.i(ADK.TAG, "allocating new command buffer of length "
					+ bufferLength);
			buffer = new byte[bufferLength];
		}

		buffer[0] = (byte) command;
		buffer[1] = (byte) sequence;
		buffer[2] = (byte) (payload.length & 0xff);
		buffer[3] = (byte) ((payload.length & 0xff00) >> 8);
		if (payload.length > 0) {
			System.arraycopy(payload, 0, buffer, 4, payload.length);
		}
		if (mConnection != null && buffer[1] != -1) {
			try {
				if (gLogPackets) {
					Log.i(ADK.TAG,
							"sendCommand: "
									+ Utilities
											.dumpBytes(buffer, buffer.length));
				}
				mConnection.getOutputStream().write(buffer);
			} catch (IOException e) {
				Log.e(ADK.TAG, "accessory write failed", e);
			}
		}
		return buffer;
	}

	public void sendCommand(int command, int sequence, byte[] payload) {
		sendCommand(command, sequence, payload, null);
	}

	private void sendCommand(int command, int sequence) {
		sendCommand(command, sequence, mEmptyPayload, mQueryBuffer);
	}

	private void handleBtNameCommand(byte[] settingsBytes) {

		if (settingsBytes.length > 1 || settingsBytes[0] == 0) { // it's a name
																	// reply

			byte[] b = new byte[settingsBytes.length - 1];
			for (int i = 0; i < settingsBytes.length - 1; i++)
				b[i] = settingsBytes[i];

			curBtName = new String(b);
		}
	}

	private void handleSettingsCommand(byte[] settingsBytes) {

		if (System.currentTimeMillis() < mIgnoreUpdatesUntil) {
			return;
		}

		if (settingsBytes.length == 8) {
			int settings[] = Utilities.byteArrayToIntArray(settingsBytes);
			mIgnorePrefChanges = true;
			SharedPreferences.Editor editor = PreferenceManager
					.getDefaultSharedPreferences(this).edit();

			// (alarm:u8,u8,u8,brightness:u8,color:u8,u8,u8:volume:u8)
			int alarmTime = Utilities.dateToTimeValue(settings[0], settings[1]);
			editor.putInt(Preferences.PREF_ALARM_TIME, alarmTime);
			boolean alarmOn = (settings[2] != 0);
			editor.putBoolean(Preferences.PREF_ALARM_ON, alarmOn);

			editor.putInt(Preferences.PREF_BRIGHTNESS, settings[3]);

			int color = settings[4] << 16 | settings[5] << 8 | settings[6];
			editor.putInt(Preferences.PREF_COLOR, color);

			int volume = settings[7];
			editor.putInt(Preferences.PREF_VOLUME, volume);
			editor.apply();

			mIgnorePrefChanges = false;
		}
	}

	private void handleLicenseTextCommand(byte[] licenseTextBytes) {
		if (gLogPackets) {
			Log.i(ADK.TAG, "License text chunk");
			Log.i(ADK.TAG, Utilities.dumpBytes(licenseTextBytes,
					licenseTextBytes.length));
		}
		if (licenseTextBytes.length > 1 && licenseTextBytes[0] != 0) {
			mLicenseTextStream.write(licenseTextBytes, 1,
					licenseTextBytes.length - 1);
			sendCommand(CMD_GET_LICENSE, 33);
		} else {

			try {
				mLicenseTextStream.close();
				byte[] encodedArray = mLicenseTextStream.toByteArray();
				GZIPInputStream gis = new GZIPInputStream(
						new ByteArrayInputStream(encodedArray));
				byte[] decodedBuffer = new byte[128 * 1024]; // TODO: make this
																// buffer
																// smaller
				while (true) {
					int length = gis.read(decodedBuffer);
					if (length < 1) {
						SharedPreferences.Editor editor = PreferenceManager
								.getDefaultSharedPreferences(this).edit();
						editor.putString(Preferences.PREF_LICENSE_TEXT,
								mLicenseText);
						editor.commit();
						mLicenseText = "";
						break;
					}
					mLicenseText = mLicenseText
							+ new String(decodedBuffer, 0, length, "utf-8");
				}
			} catch (IOException e) {
				Log.i(ADK.TAG, "error = " + e.toString());
			}
		}
	}

	private void handleFileListCommand(byte[] fileListBytes) {
		if (gLogPackets)
			Log.i(ADK.TAG,
					"handleFileListCommand: "
							+ Utilities.dumpBytes(fileListBytes,
									fileListBytes.length));
		if (fileListBytes.length == 1 && fileListBytes[0] == 0) {
			return;
		}
		if (fileListBytes.length > 6) {
			String fileName = new String(fileListBytes, 5,
					fileListBytes.length - 6);
			if (gLogPackets)
				Log.i(ADK.TAG, "got file name '" + fileName + "'");
			mSoundFiles.add(fileName);
		}
		sendCommand(CMD_FILE_LIST, CMD_FILE_LIST);

	}

	private void handleAlarmFileCommand(byte[] alarmFileNameBytes) {
		if (alarmFileNameBytes.length > 1) {
			mIgnorePrefChanges = true;
			SharedPreferences.Editor editor = PreferenceManager
					.getDefaultSharedPreferences(this).edit();
			String alarmFileName = new String(alarmFileNameBytes, 0,
					alarmFileNameBytes.length - 1);
			alarmFileName = alarmFileName.replaceFirst("(?i)" + TUNES_FOLDER
					+ "/", "");
			editor.putString(Preferences.PREF_ALARM_SOUND, alarmFileName);
			editor.apply();
			mIgnorePrefChanges = false;
		}
	}

	private void handleGetSensorsCommand(byte[] sensorBytes) {
		if (gLogPackets)
			Log.i(ADK.TAG,
					"handleGetSensorsCommand: "
							+ Utilities.dumpBytes(sensorBytes,
									sensorBytes.length));
		if (sensorBytes.length > 23) {
			int sensorValues[] = Utilities.byteArrayToIntArray(sensorBytes);
			int proxNormalized[] = {
					sensorValues[20] | (sensorValues[21] << 8),
					sensorValues[22] | (sensorValues[23] << 8),
					sensorValues[24] | (sensorValues[25] << 8) };
			proxNormalized[2] *= 3;
			// find max
			int proxMax = 0;
			for (int i = 0; i < 3; i++)
				if (proxMax < proxNormalized[i])
					proxMax = proxNormalized[i];
			proxMax++;
			// normalize to 8-bits
			for (int i = 0; i < 3; i++)
				proxNormalized[i] = (proxNormalized[i] << 8) / proxMax;
			final int exp[] = { 0, 19, 39, 59, 79, 100, 121, 143, 165, 187,
					209, 232, 255, 279, 303, 327, 352, 377, 402, 428, 454, 481,
					508, 536, 564, 592, 621, 650, 680, 710, 741, 772, 804, 836,
					869, 902, 936, 970, 1005, 1040, 1076, 1113, 1150, 1187,
					1226, 1264, 1304, 1344, 1385, 1426, 1468, 1511, 1554, 1598,
					1643, 1688, 1734, 1781, 1829, 1877, 1926, 1976, 2026, 2078,
					2130, 2183, 2237, 2292, 2348, 2404, 2461, 2520, 2579, 2639,
					2700, 2762, 2825, 2889, 2954, 3020, 3088, 3156, 3225, 3295,
					3367, 3439, 3513, 3588, 3664, 3741, 3819, 3899, 3980, 4062,
					4146, 4231, 4317, 4404, 4493, 4583, 4675, 4768, 4863, 4959,
					5057, 5156, 5257, 5359, 5463, 5568, 5676, 5785, 5895, 6008,
					6122, 6238, 6355, 6475, 6597, 6720, 6845, 6973, 7102, 7233,
					7367, 7502, 7640, 7780, 7922, 8066, 8213, 8362, 8513, 8666,
					8822, 8981, 9142, 9305, 9471, 9640, 9811, 9986, 10162,
					10342, 10524, 10710, 10898, 11089, 11283, 11480, 11681,
					11884, 12091, 12301, 12514, 12731, 12951, 13174, 13401,
					13632, 13866, 14104, 14345, 14591, 14840, 15093, 15351,
					15612, 15877, 16147, 16421, 16699, 16981, 17268, 17560,
					17856, 18156, 18462, 18772, 19087, 19407, 19733, 20063,
					20398, 20739, 21085, 21437, 21794, 22157, 22525, 22899,
					23279, 23666, 24058, 24456, 24861, 25272, 25689, 26113,
					26544, 26982, 27426, 27878, 28336, 28802, 29275, 29756,
					30244, 30740, 31243, 31755, 32274, 32802, 33338, 33883,
					34436, 34998, 35568, 36148, 36737, 37335, 37942, 38559,
					39186, 39823, 40469, 41126, 41793, 42471, 43159, 43859,
					44569, 45290, 46023, 46767, 47523, 48291, 49071, 49863,
					50668, 51486, 52316, 53159, 54016, 54886, 55770, 56668,
					57580, 58506, 59447, 60403, 61373, 62359, 63361, 64378,
					65412 };
			for (int i = 0; i < 3; i++)
				proxNormalized[i] = (exp[proxNormalized[i]] + 128) >> 8;

			SharedPreferences.Editor editor = PreferenceManager
					.getDefaultSharedPreferences(this).edit();
			int color = (proxNormalized[0] << 16) | (proxNormalized[1] << 8)
					| proxNormalized[2];
			editor.putInt(Preferences.PREF_COLOR_SENSOR, color);
			editor.commit();
		}
	}

	private void handleLockCommand(byte[] lockedBytes) {
		if (gLogPackets)
			Log.i(ADK.TAG,
					"lockBytes: "
							+ Utilities.dumpBytes(lockedBytes,
									lockedBytes.length));
		if (lockedBytes.length > 0 && lockedBytes[0] != 1) {
			mIgnorePrefChanges = true;
			SharedPreferences.Editor editor = PreferenceManager
					.getDefaultSharedPreferences(this).edit();
			editor.putBoolean(Preferences.PREF_LOCKED, lockedBytes[0] == 2);
			editor.commit();
			mIgnorePrefChanges = false;
		}
	}

	public boolean handleMessage(Message msg) {
		if (msg.getTarget() == mDeviceHandler) {
			return handleDeviceMethod(msg);
		} else {
			pollSettings();
			return true;
		}
	}

	private boolean handleDeviceMethod(Message msg) {
		switch (msg.what) {
		case CMD_SETTINGS:
			handleSettingsCommand((byte[]) msg.obj);
			return true;
		case CMD_BT_NAME:
			handleBtNameCommand((byte[]) msg.obj);
		case CMD_GET_LICENSE:
			handleLicenseTextCommand((byte[]) msg.obj);
			return true;
		case CMD_FILE_LIST:
			handleFileListCommand((byte[]) msg.obj);
			return true;
		case CMD_ALARM_FILE:
			handleAlarmFileCommand((byte[]) msg.obj);
			return true;
		case CMD_GET_SENSORS:
			handleGetSensorsCommand((byte[]) msg.obj);
			return true;
		case CMD_LOCK:
			handleLockCommand((byte[]) msg.obj);
			return true;
		}
		return false;
	}

	public Object getAccessory() {
		return mAccessory;
	}

	private void updateLockDisplay() {
		boolean isLocked = mPreferences.getBoolean(Preferences.PREF_LOCKED,
				false);
		setUpButton(R.id.lock_button, isLocked ? R.drawable.ic_lock
				: R.drawable.ic_unlock, isLocked ? R.string.locked
				: R.string.unlocked);
	}

	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
			String key) {

		if (Preferences.PREF_LOCKED.equals(key)) {
			updateLockDisplay();
		}

		if (mIgnorePrefChanges) {
			return;
		}

		if (gLogPackets)
			Log.d(ADK.TAG, "changed: " + key);

		if (Utilities.indexOf(Preferences.SETTINGS_PREFERENCES, key) != -1) {
			if (gLogPackets)
				Log.d(ADK.TAG, "updating settings");
			updateSettings(sharedPreferences);
		} else if (Preferences.PREF_TIME.equals(key)) {
			updateTime(sharedPreferences);
		} else if (Preferences.PREF_DISPLAY.equals(key)) {
			updateDisplay(sharedPreferences);
		} else if (Preferences.PREF_LOCKED.equals(key)) {
			updateLocked(sharedPreferences);
		} else if (Preferences.PREF_ALARM_SOUND.equals(key)) {
			updateAlarmSound(sharedPreferences);
		}
	}

	private void updateSettings(SharedPreferences sharedPreferences) {
		mIgnoreUpdatesUntil = System.currentTimeMillis() + 1000;
		int color = sharedPreferences.getInt(Preferences.PREF_COLOR,
				Preferences.PREF_DEFAULT_COLOR);
		byte[] payload = mSettingsPayload;
		int alarmTimeValue = sharedPreferences.getInt(
				Preferences.PREF_ALARM_TIME, Preferences.DEFAULT_ALARM_TIME);
		payload[0] = (byte) (alarmTimeValue / 60);
		payload[1] = (byte) (alarmTimeValue % 60);
		boolean alarmOn = sharedPreferences.getBoolean(
				Preferences.PREF_ALARM_ON, false);
		payload[2] = (byte) (alarmOn ? 1 : 0);
		int brightness = sharedPreferences.getInt(Preferences.PREF_BRIGHTNESS,
				255);
		payload[3] = (byte) brightness;
		payload[4] = (byte) (((color >> 16) & 0xff));
		payload[5] = (byte) (((color >> 8) & 0xff));
		payload[6] = (byte) ((color & 0xff));
		int volume = sharedPreferences.getInt(Preferences.PREF_VOLUME, 128);
		payload[7] = (byte) volume;
		mSettingsBuffer = sendCommand(CMD_SETTINGS, CMD_SETTINGS, payload,
				mSettingsBuffer);
	}

	private void updateTime(SharedPreferences sharedPreferences) {
		int timeValue = sharedPreferences.getInt(Preferences.PREF_TIME, 0);
		byte[] payload = new byte[7];
		payload[0] = 0;
		payload[1] = 0;
		payload[2] = 0;
		payload[3] = 0;
		payload[4] = (byte) (timeValue / 60);
		payload[5] = (byte) (timeValue % 60);
		payload[6] = 0;
		sendCommand(CMD_TIME, CMD_TIME, payload);
	}

	private void updateDisplay(SharedPreferences sharedPreferences) {
		int displayValue = sharedPreferences
				.getInt(Preferences.PREF_DISPLAY, 0);
		byte[] payload = new byte[1];
		payload[0] = (byte) displayValue;
		sendCommand(CMD_DISPLAY_MODE, CMD_DISPLAY_MODE, payload);
	}

	private void updateLocked(SharedPreferences sharedPreferences) {
		boolean isLocked = sharedPreferences.getBoolean(
				Preferences.PREF_LOCKED, false);
		Log.i(ADK.TAG, "updating locked " + isLocked);
		byte[] payload = new byte[1];
		payload[0] = (byte) (isLocked ? 2 : 0);
		sendCommand(CMD_LOCK, CMD_LOCK, payload);
	}

	private void updateAlarmSound(SharedPreferences sharedPreferences) {
		String alarmSound = TUNES_FOLDER + "/"
				+ sharedPreferences.getString(Preferences.PREF_ALARM_SOUND, "");
		final int alarmSoundLength = alarmSound.length();
		if (alarmSoundLength > 0) {
			byte[] payload = new byte[alarmSoundLength + 1];
			alarmSound.getBytes(0, alarmSoundLength, payload, 0);
			payload[alarmSoundLength] = (byte) 0;
			sendCommand(CMD_ALARM_FILE, CMD_ALARM_FILE, payload);
		}
	}

	private static class ProtocolHandler {
		InputStream mInputStream;
		Handler mHandler;

		public ProtocolHandler(Handler handler, InputStream inputStream) {
			mHandler = handler;
			mInputStream = inputStream;
		}

		int readByte() throws IOException {
			int retVal = mInputStream.read();
			if (retVal == -1) {
				throw new RuntimeException("End of stream reached.");
			}
			return retVal;
		}

		int readInt16() throws IOException {
			int low = readByte();
			int high = readByte();
			if (gLogPackets) {
				Log.i(ADK.TAG, "readInt16 low=" + low + " high=" + high);
			}
			return low | (high << 8);
		}

		byte[] readBuffer(int bufferSize) throws IOException {
			byte readBuffer[] = new byte[bufferSize];
			int index = 0;
			int bytesToRead = bufferSize;
			while (bytesToRead > 0) {
				int amountRead = mInputStream.read(readBuffer, index,
						bytesToRead);
				if (amountRead == -1) {
					throw new RuntimeException("End of stream reached.");
				}
				bytesToRead -= amountRead;
				index += amountRead;
			}
			return readBuffer;
		}

		public void process() {
			mInputStream.mark(0);
			try {
				while (mInputStream.available() > 0) {
					if (gLogPackets)
						Log.i(ADK.TAG, "about to read opcode");
					int opCode = readByte();
					if (gLogPackets)
						Log.i(ADK.TAG, "opCode = " + opCode);
					if (isValidOpCode(opCode)) {
						int sequence = readByte();
						if (gLogPackets)
							Log.i(ADK.TAG, "sequence = " + sequence);
						int replySize = readInt16();
						if (gLogPackets)
							Log.i(ADK.TAG, "replySize = " + replySize);
						byte[] replyBuffer = readBuffer(replySize);
						if (gLogPackets) {
							Log.i(ADK.TAG,
									"replyBuffer: "
											+ Utilities.dumpBytes(replyBuffer,
													replyBuffer.length));
						}
						processReply(opCode & 0x7f, sequence, replyBuffer);
						mInputStream.mark(0);
					}
				}
				mInputStream.reset();
			} catch (IOException e) {
				Log.i(ADK.TAG, "ProtocolHandler error " + e.toString());
			}
		}

		boolean isValidOpCode(int opCodeWithReplyBitSet) {
			if ((opCodeWithReplyBitSet & 0x80) != 0) {
				int opCode = opCodeWithReplyBitSet & 0x7f;
				return ((opCode >= CMD_GET_PROTO_VERSION) && (opCode <= CMD_LOCK));
			}
			return false;
		}

		private void processReply(int opCode, int sequence, byte[] replyBuffer) {
			Message msg = mHandler.obtainMessage(opCode, sequence, 0,
					replyBuffer);
			mHandler.sendMessage(msg);
		}
	}

	public String[] getAlarmSounds() {
		String[] r = new String[mSoundFiles.size()];
		return mSoundFiles.toArray(r);
	}
}
