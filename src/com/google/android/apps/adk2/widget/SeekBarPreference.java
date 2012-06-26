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

import android.content.Context;
import android.content.res.TypedArray;
import android.preference.Preference;
import android.util.AttributeSet;
import android.view.View;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;

import com.google.android.apps.adk2.R;

public class SeekBarPreference extends Preference implements OnSeekBarChangeListener {
    private int mMax;
    private int mValue;
    private boolean mTrackingTouch;
    
    public SeekBarPreference(Context context) {
        this(context, null);
    }
    
    public SeekBarPreference(Context context, AttributeSet attrs) {
        this(context, attrs, android.R.attr.preferenceStyle);
    }
    
    public SeekBarPreference(Context context, AttributeSet attrs, int defStyle) {
       super(context, attrs, defStyle);
        
        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.SeekBarPreference, defStyle, 0);
        mMax = a.getInteger(R.styleable.SeekBarPreference_max, 0);
        a.recycle();
        
        setLayoutResource(R.layout.seekbar_preference);
    }
    
    @Override
    protected void onBindView(View view) {
        super.onBindView(view);

        SeekBar seekBar = (SeekBar)view.findViewById(R.id.seekbar);
        seekBar.setMax(mMax);
        seekBar.setProgress(mValue);
        seekBar.setEnabled(isEnabled());
        
        seekBar.setOnSeekBarChangeListener(this);
    }

    @Override
    protected void onSetInitialValue(boolean restoreValue, Object defaultValue) {
        setValue(restoreValue ? getPersistedInt(0) : (Integer)defaultValue);
    }

    @Override
    protected Object onGetDefaultValue(TypedArray a, int index) {
        return a.getInt(index, 0);
    }

    public void setValue(int value) {
        if (value > mMax) value = mMax;
        if (value < 0) value = 0;

        if (value != mValue) {
            mValue = value;
            persistInt(value);
            if (!mTrackingTouch)
                notifyChanged();
        }
    }

    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (progress != mValue) {
            if (callChangeListener(progress)) {
                setValue(progress);
            } else {
                seekBar.setProgress(progress);
            }
        }
    }

    public void onStartTrackingTouch(SeekBar seekBar) {        
        mTrackingTouch = true;
    }

    public void onStopTrackingTouch(SeekBar seekBar) {
        mTrackingTouch = false;
    }
}
