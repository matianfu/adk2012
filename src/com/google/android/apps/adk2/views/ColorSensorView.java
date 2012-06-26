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
package com.google.android.apps.adk2.views;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.util.AttributeSet;
import android.view.View;

public class ColorSensorView extends View {
	Paint mCircle;
	Paint mBorder;

	private final int kSize = 200;

	public ColorSensorView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initColorSensorView(context);
	}

	public ColorSensorView(Context context) {
		super(context);
		initColorSensorView(context);
	}

	public ColorSensorView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initColorSensorView(context);
	}

	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		setMeasuredDimension(kSize, kSize);
	}

	private void initColorSensorView(Context context) {
		mCircle = new Paint();
		mCircle.setARGB(255, 255, 0, 0);
		mCircle.setStyle(Style.FILL);

		mBorder = new Paint();
		mBorder.setARGB(255, 255, 255, 255);
		mBorder.setStyle(Style.STROKE);
	}

	public void setSensedColor(int color) {
		mCircle.setColor(color);
		invalidate();
	}

	@Override
	protected void onDraw(Canvas canvas) {
		canvas.drawRect(0, 0, kSize - 1, kSize - 1, mBorder);
		canvas.drawCircle(kSize / 2, kSize / 2, kSize / 8, mCircle);
	}
}
