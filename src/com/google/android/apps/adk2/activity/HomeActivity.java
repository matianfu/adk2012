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
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
// import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context; /* new */
import android.content.Intent;
import android.content.IntentFilter;
//import android.content.Intent;
//import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelFileDescriptor;
import android.os.Handler.Callback;
import android.os.Message;
// import android.preference.PreferenceManager;
import android.util.Log;
//import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
//import android.widget.Button;
//import android.widget.EditText;
//import android.widget.ImageView;
//import android.widget.TextView;

// import com.android.future.usb.UsbAccessory;
import android.hardware.usb.UsbAccessory;
// import com.android.future.usb.UsbManager;
import android.hardware.usb.UsbManager;

import com.google.android.apps.adk2.ADK;
import com.google.android.apps.adk2.R;
import com.google.android.apps.adk2.Utilities;

public class HomeActivity extends Activity implements OnClickListener,
		Callback, Runnable {

	private Handler mDeviceHandler;
	private Handler mSettingsPollingHandler;
	private UsbManager mUSBManager;
	private UsbAccessory mAccessory;

	private FileInputStream mInputStream;
	private FileOutputStream mOutputStream;
	private ParcelFileDescriptor mFileDescriptor;

	private boolean mPollSettings = false;

	private static final String ACTION_USB_PERMISSION = "com.google.android.DemoKit.action.USB_PERMISSION";

	private static final boolean gLogPackets = true;

	static final int DIALOG_NO_PRESETS_ID = 0;

	private static HomeActivity sHomeActivity = null;

	private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action)) {
				// UsbAccessory accessory = UsbManager.getAccessory(intent);
				UsbAccessory accessory = (UsbAccessory) intent
						.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
				if (accessory != null && accessory.equals(mAccessory)) {
					Log.i(ADK.TAG, "Accessory Detached");
					closeAccessory();
				}
			}
		}
	};

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
	protected void onNewIntent(Intent intent) {

		Log.i(ADK.TAG, "onNewIntent, new intent received");
		String action = intent.getAction();
		if (UsbManager.ACTION_USB_ACCESSORY_ATTACHED.equals(action)) {
			UsbAccessory accessory = (UsbAccessory) intent
					.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
			if (accessory != null) {
				Log.i(ADK.TAG, "Accessory Attached");
				openAccessory(accessory);
			}
		}
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Log.i(ADK.TAG, "HomeActivity OnCreate");

		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_ATTACHED);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		registerReceiver(mUsbReceiver, filter);

		Log.i(ADK.TAG, "Broadcast receiver registered");

		setContentView(R.layout.home);
		// setContentView(R.layout.connect);
		// mBluetoothButton = (Button)
		// findViewById(R.id.connect_bluetooth_button);
		// mBluetoothButton.setOnClickListener(this);

		mDeviceHandler = new Handler(this);
		mSettingsPollingHandler = new Handler(this);

		// // mUSBManager = UsbManager.getInstance(this);
		mUSBManager = (UsbManager) getSystemService(Context.USB_SERVICE);

		connectToAccessory();
		sHomeActivity = this;
		startPollingSettings();
	}

	// private void disconnect() {
	// finish();
	// }

	private void pollSettings() {
		if (mPollSettings) {
			// sendCommand(CMD_SETTINGS, CMD_SETTINGS);
			// sendCommand(CMD_DISPLAY_MODE, CMD_DISPLAY_MODE);
			// sendCommand(CMD_LOCK, CMD_LOCK);
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
		closeAccessory();
		super.onDestroy();
	}

	public void onClick(View v) {
		// switch (v.getId()) {
	}

	public void connectToAccessory() {
		// bail out if we're already connected
		if (mAccessory != null) {
			Log.i(ADK.TAG, "connectToAccessory, mAccessory not null, already connected.");
			return;
		}

		UsbAccessory[] accessories = mUSBManager.getAccessoryList();
		UsbAccessory accessory = (accessories == null ? null : accessories[0]);
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
			Log.i(ADK.TAG, "accessory is null");
		}
	}

	public void disconnectFromAccessory() {
		closeAccessory();
	}

	private void openAccessory(UsbAccessory accessory) {

		mFileDescriptor = mUSBManager.openAccessory(accessory);

		if (mFileDescriptor != null) {
			mAccessory = accessory;

			FileDescriptor fd = mFileDescriptor.getFileDescriptor();
			mInputStream = new FileInputStream(fd);
			mOutputStream = new FileOutputStream(fd);

			mAccessory = accessory;
		}

		// performPostConnectionTasks();
	}

	private void closeAccessory() {

		if (mInputStream != null) {
			try {
				mInputStream.close();
			} catch (IOException e) {
			} finally {
				mInputStream = null;
			}
		}

		if (mOutputStream != null) {
			try {
				mOutputStream.close();
			} catch (IOException e) {
			} finally {
				mOutputStream = null;
			}
		}

		if (mFileDescriptor != null) {
			try {
				mFileDescriptor.close();
			} catch (IOException e) {
			} finally {
				mFileDescriptor = null;
			}
		}
	}

	// private void performPostConnectionTasks() {
	// byte[] buffer = new byte[100];
	// buffer[0] = (byte)((int)1 & 0xFF);
	// buffer[1] = (byte)((int)2 & 0xFF);
	// buffer[2] = (byte)((int)3 & 0xFF);
	// buffer[3] = (byte)((int)4 & 0xFF);
	// buffer[4] = (byte)((int)5 & 0xFF);
	// buffer[5] = (byte)((int)6 & 0xFF);
	// buffer[6] = (byte)((int)7 & 0xFF);
	// buffer[7] = (byte)((int)8 & 0xFF);
	// buffer[8] = (byte)((int)9 & 0xFF);
	// buffer[9] = (byte)((int)10 & 0xFF);
	//
	// try{
	// mConnection.getOutputStream().write(buffer, 0, 10);
	// }
	// catch(IOException e)
	// {
	//
	// }
	//
	// buffer[10] = (byte)((int)1 & 0xFF);
	// buffer[11] = (byte)((int)2 & 0xFF);
	// buffer[12] = (byte)((int)3 & 0xFF);
	// buffer[13] = (byte)((int)4 & 0xFF);
	// buffer[14] = (byte)((int)5 & 0xFF);
	// buffer[15] = (byte)((int)6 & 0xFF);
	// buffer[16] = (byte)((int)7 & 0xFF);
	// buffer[17] = (byte)((int)8 & 0xFF);
	// buffer[18] = (byte)((int)9 & 0xFF);
	// buffer[19] = (byte)((int)10 & 0xFF);
	//
	// try{
	// mConnection.getOutputStream().write(buffer, 10, 10);
	// }
	// catch(IOException e)
	// {
	//
	// }
	//
	// buffer[20] = (byte)11;
	// buffer[21] = (byte)12;
	// buffer[22] = (byte)13;
	// buffer[23] = (byte)14;
	// buffer[24] = (byte)15;
	// buffer[25] = (byte)16;
	// buffer[26] = (byte)17;
	// buffer[27] = (byte)18;
	// buffer[28] = (byte)19;
	// buffer[29] = (byte)20;
	//
	// try{
	// mConnection.getOutputStream().write(buffer, 20, 10);
	// }
	// catch(IOException e)
	// {
	//
	// }
	//
	// int i;
	//
	// for (i = 0; i < 100; i++)
	// {
	// buffer[i] = (byte)i;
	// }
	// try{
	// mConnection.getOutputStream().write(buffer, 10, 32);
	// mConnection.getOutputStream().write(buffer, 12, 32);
	// mConnection.getOutputStream().write(buffer, 14, 32);
	// mConnection.getOutputStream().write(buffer, 16, 32);
	// mConnection.getOutputStream().write(buffer, 18, 32);
	// }
	// catch(IOException e)
	// {
	//
	// }
	// // sendCommand(CMD_GET_PROTO_VERSION, CMD_GET_PROTO_VERSION);
	// // sendCommand(CMD_SETTINGS, CMD_SETTINGS);
	// // sendCommand(CMD_BT_NAME, CMD_BT_NAME);
	// // sendCommand(CMD_ALARM_FILE, CMD_ALARM_FILE);
	// // listDirectory(TUNES_FOLDER);
	//
	// // Thread thread = new Thread(null, this, "ADK 2012");
	// // thread.start();
	// }

	public void run() {
		int ret = 0;
		byte[] buffer = new byte[16384];
		int bufferUsed = 0;

		while (ret >= 0) {
			try {
				ret = mInputStream.read(buffer, bufferUsed, buffer.length
						- bufferUsed);
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
		if (mOutputStream != null && buffer[1] != -1) {
			try {
				if (gLogPackets) {
					Log.i(ADK.TAG,
							"sendCommand: "
									+ Utilities
											.dumpBytes(buffer, buffer.length));
				}
				mOutputStream.write(buffer);
			} catch (IOException e) {
				Log.e(ADK.TAG, "accessory write failed", e);
			}
		}
		return buffer;
	}

	public void sendCommand(int command, int sequence, byte[] payload) {
		sendCommand(command, sequence, payload, null);
	}

	// private void sendCommand(int command, int sequence) {
	// // sendCommand(command, sequence, mEmptyPayload, mQueryBuffer);
	// }

	// private void handleGetSensorsCommand(byte[] sensorBytes) {
	// if (gLogPackets)
	// Log.i(ADK.TAG,
	// "handleGetSensorsCommand: "
	// + Utilities.dumpBytes(sensorBytes,
	// sensorBytes.length));
	// if (sensorBytes.length > 23) {
	// int sensorValues[] = Utilities.byteArrayToIntArray(sensorBytes);
	// int proxNormalized[] = {
	// sensorValues[20] | (sensorValues[21] << 8),
	// sensorValues[22] | (sensorValues[23] << 8),
	// sensorValues[24] | (sensorValues[25] << 8) };
	// proxNormalized[2] *= 3;
	// // find max
	// int proxMax = 0;
	// for (int i = 0; i < 3; i++)
	// if (proxMax < proxNormalized[i])
	// proxMax = proxNormalized[i];
	// proxMax++;
	// // normalize to 8-bits
	// for (int i = 0; i < 3; i++)
	// proxNormalized[i] = (proxNormalized[i] << 8) / proxMax;
	// final int exp[] = { 0, 19, 39, 59, 79, 100, 121, 143, 165, 187,
	// 209, 232, 255, 279, 303, 327, 352, 377, 402, 428, 454, 481,
	// 508, 536, 564, 592, 621, 650, 680, 710, 741, 772, 804, 836,
	// 869, 902, 936, 970, 1005, 1040, 1076, 1113, 1150, 1187,
	// 1226, 1264, 1304, 1344, 1385, 1426, 1468, 1511, 1554, 1598,
	// 1643, 1688, 1734, 1781, 1829, 1877, 1926, 1976, 2026, 2078,
	// 2130, 2183, 2237, 2292, 2348, 2404, 2461, 2520, 2579, 2639,
	// 2700, 2762, 2825, 2889, 2954, 3020, 3088, 3156, 3225, 3295,
	// 3367, 3439, 3513, 3588, 3664, 3741, 3819, 3899, 3980, 4062,
	// 4146, 4231, 4317, 4404, 4493, 4583, 4675, 4768, 4863, 4959,
	// 5057, 5156, 5257, 5359, 5463, 5568, 5676, 5785, 5895, 6008,
	// 6122, 6238, 6355, 6475, 6597, 6720, 6845, 6973, 7102, 7233,
	// 7367, 7502, 7640, 7780, 7922, 8066, 8213, 8362, 8513, 8666,
	// 8822, 8981, 9142, 9305, 9471, 9640, 9811, 9986, 10162,
	// 10342, 10524, 10710, 10898, 11089, 11283, 11480, 11681,
	// 11884, 12091, 12301, 12514, 12731, 12951, 13174, 13401,
	// 13632, 13866, 14104, 14345, 14591, 14840, 15093, 15351,
	// 15612, 15877, 16147, 16421, 16699, 16981, 17268, 17560,
	// 17856, 18156, 18462, 18772, 19087, 19407, 19733, 20063,
	// 20398, 20739, 21085, 21437, 21794, 22157, 22525, 22899,
	// 23279, 23666, 24058, 24456, 24861, 25272, 25689, 26113,
	// 26544, 26982, 27426, 27878, 28336, 28802, 29275, 29756,
	// 30244, 30740, 31243, 31755, 32274, 32802, 33338, 33883,
	// 34436, 34998, 35568, 36148, 36737, 37335, 37942, 38559,
	// 39186, 39823, 40469, 41126, 41793, 42471, 43159, 43859,
	// 44569, 45290, 46023, 46767, 47523, 48291, 49071, 49863,
	// 50668, 51486, 52316, 53159, 54016, 54886, 55770, 56668,
	// 57580, 58506, 59447, 60403, 61373, 62359, 63361, 64378,
	// 65412 };
	// for (int i = 0; i < 3; i++)
	// proxNormalized[i] = (exp[proxNormalized[i]] + 128) >> 8;
	//
	// SharedPreferences.Editor editor = PreferenceManager
	// .getDefaultSharedPreferences(this).edit();
	// int color = (proxNormalized[0] << 16) | (proxNormalized[1] << 8)
	// | proxNormalized[2];
	// editor.putInt(Preferences.PREF_COLOR_SENSOR, color);
	// editor.commit();
	// }
	// }

	public boolean handleMessage(Message msg) {
		if (msg.getTarget() == mDeviceHandler) {
			// return handleDeviceMethod(msg);
			return true;
		} else {
			pollSettings();
			return true;
		}
	}

	public Object getAccessory() {
		return mAccessory;
	}

	// private void updateSettings(SharedPreferences sharedPreferences) {
	// mIgnoreUpdatesUntil = System.currentTimeMillis() + 1000;
	// int color = sharedPreferences.getInt(Preferences.PREF_COLOR,
	// Preferences.PREF_DEFAULT_COLOR);
	// byte[] payload = mSettingsPayload;
	// int alarmTimeValue = sharedPreferences.getInt(
	// Preferences.PREF_ALARM_TIME, Preferences.DEFAULT_ALARM_TIME);
	// payload[0] = (byte) (alarmTimeValue / 60);
	// payload[1] = (byte) (alarmTimeValue % 60);
	// boolean alarmOn = sharedPreferences.getBoolean(
	// Preferences.PREF_ALARM_ON, false);
	// payload[2] = (byte) (alarmOn ? 1 : 0);
	// int brightness = sharedPreferences.getInt(Preferences.PREF_BRIGHTNESS,
	// 255);
	// payload[3] = (byte) brightness;
	// payload[4] = (byte) (((color >> 16) & 0xff));
	// payload[5] = (byte) (((color >> 8) & 0xff));
	// payload[6] = (byte) ((color & 0xff));
	// int volume = sharedPreferences.getInt(Preferences.PREF_VOLUME, 128);
	// payload[7] = (byte) volume;
	// mSettingsBuffer = sendCommand(CMD_SETTINGS, CMD_SETTINGS, payload,
	// mSettingsBuffer);
	// }

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
				// int opCode = opCodeWithReplyBitSet & 0x7f;
				// return ((opCode >= CMD_GET_PROTO_VERSION) && (opCode <=
				// CMD_LOCK));
				return true;
			}
			return false;
		}

		private void processReply(int opCode, int sequence, byte[] replyBuffer) {
			Message msg = mHandler.obtainMessage(opCode, sequence, 0,
					replyBuffer);
			mHandler.sendMessage(msg);
		}
	}
}
