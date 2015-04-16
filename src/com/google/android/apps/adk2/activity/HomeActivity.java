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

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Calendar;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.text.Editable;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ScrollView;
//import android.widget.EditText;
//import android.widget.ImageView;
import android.widget.TextView;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;

import com.google.android.apps.adk2.R;

public class HomeActivity extends Activity implements OnClickListener, Runnable {

	private static final String TAG = "ADK_2012";
	private static final String ACTION_USB_PERMISSION = "com.google.android.DemoKit.action.USB_PERMISSION";

	/**
	 * The following two object have different life cycles.
	 * 
	 * mAccessory represents if an Accessory (usb host) connected.
	 * mFileDescriptor represents if file descriptor opened, aka, a connection
	 * established.
	 * 
	 * The former can be safely left alone when process/main thread killed. The
	 * latter must be closed properly otherwise a usb reset is required, which
	 * can only be performed from MCU side.
	 */
	private UsbAccessory mAccessory;
	private ParcelFileDescriptor mFileDescriptor;

	/**
	 * For command/response thread
	 */
	private String mCommand;
	private String mResponse;

	private Button mButton1;
	private Button mButton2;
	private Button mButton3;
	private Button mButton4;
	private Button mButton5;
	private Button mButton6;

	private TextView mLabel1;
	private TextView mConsole1;
	private int mConsoleLines = 0;

	/**
	 * Only DETACH intent can be received via Broadcast receiver. ATTACH intent
	 * can only be received by system launcher or onNewIntent if the activity is
	 * alive, either foreground or background.
	 * 
	 * BroadcastReceiver should be registered when Accessory found and
	 * unregistered when Accessory disconnected.
	 */
	private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action)) {
				// UsbAccessory accessory = UsbManager.getAccessory(intent);
				UsbAccessory accessory = (UsbAccessory) intent
						.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
				if (accessory != null && accessory.equals(mAccessory)) {
					Log.i(TAG, "Accessory Detached");
					closeAccessory();
					unregisterDetachReceiver();
					mAccessory = null;
				}
			}
		}
	};

	/**
	 * register broadcast receiver
	 */
	private void registerDetachReceiver() {
		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		registerReceiver(mUsbReceiver, filter);
	}

	/**
	 * unregister broadcast receiver
	 */
	private void unregisterDetachReceiver() {
		unregisterReceiver(mUsbReceiver);
	}

	/**
	 * get system usb manager, no need to hold a reference in this class
	 * 
	 * @return
	 */
	private UsbManager getUsbManager() {
		return (UsbManager) getSystemService(Context.USB_SERVICE);
	}

	/**
	 * request Accessory, query system usb manager and make sure we have the
	 * permission to access it.
	 * 
	 * @return
	 */
	private UsbAccessory requestAccessory() {
		UsbAccessory[] accessories = getUsbManager().getAccessoryList();
		UsbAccessory accessory = (accessories == null ? null : accessories[0]);
		if (accessory != null) {
			if (getUsbManager().hasPermission(accessory)) {
				return accessory;
			} else {
				Log.i(TAG, "requestAccessory: Permission failed");
				return null;
			}
		} else {
			Log.i(TAG, "requestAccessory: Accessory not found");
			return null;
		}
	}

	@Override
	protected void onNewIntent(Intent intent) {

		Log.i(TAG, "- onNewIntent");

		String action = intent.getAction();
		if (UsbManager.ACTION_USB_ACCESSORY_ATTACHED.equals(action)) {
			/*
			 * When this happens, it means the previous stream are invalid.
			 */
			if (mAccessory != null) {
				if (mFileDescriptor != null) {
					closeAccessory();
				}
			}

			mAccessory = null;

			UsbAccessory accessory = (UsbAccessory) intent
					.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
			if (accessory != null) {
				mAccessory = accessory;
				openAccessory();
				registerDetachReceiver();
			}
		}
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Log.i(TAG, ".");
		Log.i(TAG, ".");
		Log.i(TAG, ".");
		Log.i(TAG,
				"HomeActivity OnCreate @ "
						+ DateFormat.format("yyyy-MM-dd kk:mm:ss",
								System.currentTimeMillis()));

		setContentView(R.layout.home);
		mButton1 = (Button) findViewById(R.id.button1);
		mButton1.setOnClickListener(this);
		mButton2 = (Button) findViewById(R.id.button2);
		mButton2.setOnClickListener(this);
		mButton3 = (Button) findViewById(R.id.button3);
		mButton3.setOnClickListener(this);
		mButton4 = (Button) findViewById(R.id.button4);
		mButton4.setOnClickListener(this);
		mButton5 = (Button) findViewById(R.id.button5);
		mButton5.setOnClickListener(this);
		mButton6 = (Button) findViewById(R.id.button6);
		mButton6.setOnClickListener(this);

		mLabel1 = (TextView) findViewById(R.id.label1);
		mLabel1.setText("Disconnected");

		mConsole1 = (TextView) findViewById(R.id.console1);
		mConsole1.setSingleLine(false);

		mAccessory = requestAccessory();
		if (mAccessory != null) {
			registerDetachReceiver();
		}
	}

	@Override
	protected void onStart() {
		Log.i(TAG, "- onStart");
		super.onStart();
		openAccessory();
	}

	@Override
	protected void onRestart() {
		Log.i(TAG, "- onRestart");
		super.onRestart();
	}

	@Override
	public void onPause() {
		Log.i(TAG, "- onPause");
		super.onPause();
	}

	@Override
	public void onResume() {
		Log.i(TAG, "- onResume");
		super.onResume();
		doCommand("PING");
	}

	@Override
	public void onStop() {
		Log.i(TAG, "- onStop");
		super.onStop();
		closeAccessory();
	}

	@Override
	public void onDestroy() {
		Log.i(TAG, "- onDestroy");
		super.onDestroy();
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.button1:
			doCommand("PING");
			break;

		case R.id.button2:
			doCommand("SCAN");
			break;

		case R.id.button3:
			doCommand("CODE");
			break;

		case R.id.button4:
			doCommand("BATVOL");
			break;

		case R.id.button5:
			doCommand("BATCAP");
			break;

		case R.id.button6:
			doCommand("BATPCT");
			break;
		}
	}

	/**
	 * Open Accessory actually open file descriptor, establish a connection. In
	 * framework, /dev/usb_accessory will be opened.
	 */
	private void openAccessory() {

		if (mAccessory == null)
			return;

		mFileDescriptor = getUsbManager().openAccessory(mAccessory);

		if (mFileDescriptor != null) {
			mLabel1.setText("Accessory Connected");
			consolePuts("");
			consolePuts("");
			consolePuts("Connected");
			Log.i(TAG, "openAccessory success");
		} else {
			Log.i(TAG, "!!! openAccessory fail");
		}
	}

	/**
	 * Close open file descriptor for usb accessory. This function is crucial
	 * for maintaining robust connection. Failing to call this function before
	 * application exit (including low mem kill) will result in failure in
	 * subsequent reopen. No way to recover without usb reset or system reboot.
	 */
	private void closeAccessory() {

		if (mFileDescriptor != null) {
			try {
				mFileDescriptor.close();
				mLabel1.setText("Accessory Disconnected");
				consolePuts("Disconnected");	
				Log.i(TAG, "closeAccessory success");
			} catch (IOException e) {
				Log.i(TAG, "!!! mFileDescriptor closed with IOException, "
						+ e.getClass().toString() + ", " + e.getMessage());
			} finally {
				mFileDescriptor = null;
			}
		}
	}

	/**
	 * The write-read pattern to communicate. If write or read is blocked, the
	 * communication dies and no way to recover.
	 */
	public void run() {

		int read;
		byte[] buf = new byte[256];

		if (mFileDescriptor == null) {
			Log.i(TAG, "+++ invalid mFileDescritor");
			return;
		}

		FileDescriptor fd = mFileDescriptor.getFileDescriptor();
		FileInputStream is = new FileInputStream(fd);
		FileOutputStream os = new FileOutputStream(fd);

		try {
			Log.i(TAG, "+++ output stream write()");
			os.write(mCommand.getBytes());
			Log.i(TAG, "+++ input stream read()");
			read = is.read(buf);

			if (read > 0) {
				mResponse = new String(buf, 0, read);
				Log.i(TAG, "+++ input stream read() returns " + mResponse);
			} else if (read == 0) {
				Log.i(TAG, "+++ input stream read() returns zero byte");
			} else if (read == -1) {
				Log.i(TAG, "+++ input stream read() returns end-of-file");
			}
		} catch (IOException e) {
			Log.e(TAG, "+++ IOException", e);
			return;
		} finally {
			try {
				is.close();
			} catch (IOException e) {
			}
			try {
				os.close();
			} catch (IOException e) {
			}
		}
	}

	/**
	 * send command and get response
	 * 
	 * @param s
	 *            command string
	 */
	private void doCommand(String s) {

		// not connected?
		if (mFileDescriptor == null) {
			consolePuts("<CMD> " + String.format("%-8s", s) + "    配件尚未连接");
			Log.i(TAG, "<CMD> " + String.format("-%8s", s) + "    Accessory not connected."
					+ mResponse);
			return;
		}

		mCommand = null;
		mResponse = null;

		if (s == null || s.length() == 0)
			return;

		mCommand = s;

		Thread t = new Thread(this);
		t.start();

		while (t.isAlive()) {
		}

		consolePuts("<CMD> " + String.format("%-8s", mCommand) + "    <RSP> "
				+ mResponse);
		Log.i(TAG, "<CMD> " + String.format("%-8s", mCommand) + "    <RSP> "
				+ mResponse);
	}

	private void consolePuts(String s) {
		if (mConsoleLines >= 500) {
			Editable edit = mConsole1.getEditableText();
			edit.delete(0, edit.toString().indexOf('\n') + 1);
		}

		// mConsole1.append(s + Integer.toString(
		// Calendar.getInstance().get(Calendar.SECOND) ) + "\n");
		mConsole1.append(String.format("%04d : ", mConsoleLines));
		mConsole1.append(s + "\n");
		mConsoleLines++;
		
		ScrollView scroll = (ScrollView) findViewById(R.id.scroll1);
		scroll.fullScroll(View.FOCUS_DOWN);
	}
}
