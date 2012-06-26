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
import android.content.Intent;
import android.content.res.TypedArray;
import android.preference.Preference;
import android.util.AttributeSet;
import android.util.Log;

import com.google.android.apps.adk2.ADK;
import com.google.android.apps.adk2.R;

public class ActivityPreference extends Preference {
    String mActivityClassName;
    
    public ActivityPreference(Context context) {
        this(context, null);
    }
    
    public ActivityPreference(Context context, AttributeSet attrs) {
        this(context, attrs, android.R.attr.preferenceStyle);
    }
    
    public ActivityPreference(Context context, AttributeSet attrs, int defStyle) {
       super(context, attrs, defStyle);
        
        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.ActivityPreference, defStyle, 0);
        mActivityClassName = a.getString(R.styleable.ActivityPreference_classname);
        a.recycle();
    }

    @Override
    protected void onClick() {
        try {
            Class<?> activityClass = Class.forName(mActivityClassName);
            Intent openNewIntent = new Intent(getContext(), activityClass);
            getContext().startActivity(openNewIntent);
        } catch (ClassNotFoundException e) {
            Log.e(ADK.TAG, "class not found in ActivityPreference", e);
        }
    }
}
