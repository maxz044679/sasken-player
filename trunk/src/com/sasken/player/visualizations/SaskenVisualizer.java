package com.sasken.player.visualizations;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public abstract class SaskenVisualizer extends SurfaceView implements SurfaceHolder.Callback{

	public SaskenVisualizer(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	public SaskenVisualizer(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}
	
	public SaskenVisualizer(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		// TODO Auto-generated constructor stub
	}

	public abstract void Init(int sampleRateInHz, int numOfChannels);
	public abstract boolean startVisualization();
	public abstract boolean isVisualizationOn();
	public abstract boolean stopVisualization();
	public abstract boolean isSurfaceCreated();
	public abstract void processBuffer(byte input[]);
	public abstract void drawOn();
	public abstract void drawOff();
}
