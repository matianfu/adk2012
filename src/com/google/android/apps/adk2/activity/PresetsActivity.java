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

import java.io.IOException;
import java.util.ArrayList;

import android.app.Dialog;
import android.app.ListActivity;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.SparseBooleanArray;
import android.view.ActionMode;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import com.google.android.apps.adk2.Presets;
import com.google.android.apps.adk2.R;

public class PresetsActivity extends ListActivity {

	Presets mPresets;
	boolean mWasPolling;
	ArrayAdapter<Presets.Preset> mAdapter;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		mPresets = new Presets();
		try {
			mPresets.load(this);
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}

		Adk2BaseActivity.maybeDisplayHomeAsUp(this);

		setContentView(R.layout.presets);

		mAdapter = new ArrayAdapter<Presets.Preset>(this,
				android.R.layout.simple_list_item_multiple_choice,
				mPresets.getPresets());
		setListAdapter(mAdapter);
		getListView().setMultiChoiceModeListener(new ModeCallback());
		getListView().setChoiceMode(ListView.CHOICE_MODE_MULTIPLE_MODAL);

		mWasPolling = HomeActivity.get().startPollingSettings();
	}

	protected void onDestroy() {
		if (!mWasPolling) {
			HomeActivity.get().stopPollingSettings();
		}
		super.onDestroy();
	}

	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		Presets.Preset p = mPresets.getPresets().get(position);
		SharedPreferences sharedPreferences = PreferenceManager
				.getDefaultSharedPreferences(this);
		p.applyToPreferences(sharedPreferences);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.presets_menu, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if (Adk2BaseActivity.maybeHandleHomeMenuItem(item, this)) {
			return true;
		}
		switch (item.getItemId()) {
		case R.id.new_preset:
			makeNewPreset();
			return true;

		default:
			return false;
		}
	}

	private void makeNewPreset() {
		SharedPreferences sharedPreferences = PreferenceManager
				.getDefaultSharedPreferences(this);
		Presets.Preset p = mPresets.makeNewPreset(sharedPreferences);
		onContentChanged();
		RenamePresetController rpc = new RenamePresetController(p, true);
		rpc.start();
	}

	void deletePreset(Presets.Preset preset) {
		mAdapter.remove(preset);
	}

	private void savePresets() {
		try {
			mPresets.save(this);
		} catch (IOException e) {
			// TODO Error message for save fail
			e.printStackTrace();
		}
	}

	void deleteSelectedItems() {
		ListView lv = getListView();
		SparseBooleanArray checkedPositions = lv.getCheckedItemPositions();
		int count = checkedPositions.size();
		Resources res = getResources();
		String presetsDeleted = res.getQuantityString(
				R.plurals.number_of_presets_deleted, count, count);
		Toast.makeText(PresetsActivity.this, presetsDeleted, Toast.LENGTH_SHORT)
				.show();
		ArrayList<Presets.Preset> presets = mPresets.getPresets();
		for (int i = count - 1; i >= 0; --i) {
			int index = checkedPositions.keyAt(i);
			if (index != -1) {
				Presets.Preset p = presets.get(index);
				deletePreset(p);
			}
		}
		savePresets();
	}

	void editSelectedItems() {
		ListView lv = getListView();
		SparseBooleanArray checkedPositions = lv.getCheckedItemPositions();
		int firstIndex = checkedPositions.keyAt(0);
		if (firstIndex != -1) {
			ArrayList<Presets.Preset> presets = mPresets.getPresets();
			Presets.Preset p = presets.get(firstIndex);
			RenamePresetController rpc = new RenamePresetController(p, false);
			rpc.start();
		}
	}

	private class RenamePresetController implements OnClickListener {
		Dialog mDialog;
		Presets.Preset mPreset;
		EditText mNameEdit;
		Boolean mDeleteOnCancel;

		public RenamePresetController(Presets.Preset preset,
				Boolean deleteOnCancel) {
			mPreset = preset;
			mDeleteOnCancel = deleteOnCancel;
		}

		public void start() {
			mDialog = new Dialog(PresetsActivity.this);
			mDialog.setContentView(R.layout.preset_name_dialog);
			Button cancel = (Button) mDialog
					.findViewById(R.id.preset_edit_cancel);
			cancel.setOnClickListener(this);
			Button rename = (Button) mDialog
					.findViewById(R.id.preset_edit_rename);
			if (mDeleteOnCancel) {
				rename.setText(R.string.create);
			}
			rename.setOnClickListener(this);
			mNameEdit = (EditText) mDialog.findViewById(R.id.preset_name_edit);
			mNameEdit.setText(mPreset.getName());
			mNameEdit.selectAll();
			mDialog.setTitle("Preset Name");
			mDialog.show();
		}

		public void onClick(View v) {
			if (v.getId() == R.id.preset_edit_rename) {
				mPreset.setName(mNameEdit.getText().toString());
				onContentChanged();
				savePresets();
			} else {
				if (mDeleteOnCancel) {
					deletePreset(mPreset);
				}
			}
			mDialog.dismiss();

		}
	}

	private class ModeCallback implements ListView.MultiChoiceModeListener {

		public boolean onCreateActionMode(ActionMode mode, Menu menu) {
			MenuInflater inflater = getMenuInflater();
			inflater.inflate(R.menu.list_select_menu, menu);
			mode.setTitle("Select Items");
			return true;
		}

		public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
			return true;
		}

		public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
			switch (item.getItemId()) {
			case R.id.delete_preset:
				deleteSelectedItems();
				mode.finish();
				break;
			case R.id.edit_preset:
				editSelectedItems();
				mode.finish();
				break;
			}
			return true;
		}

		public void onDestroyActionMode(ActionMode mode) {
		}

		public void onItemCheckedStateChanged(ActionMode mode, int position,
				long id, boolean checked) {
			final int checkedCount = getListView().getCheckedItemCount();
			switch (checkedCount) {
			case 0:
				mode.setSubtitle(null);
				break;
			case 1:
				mode.setSubtitle("One item selected");
				break;
			default:
				mode.setSubtitle("" + checkedCount + " items selected");
				break;
			}
		}

	}
}
