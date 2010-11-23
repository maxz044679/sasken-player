package com.sasken.player.visualizations;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

public class EmptyVisualizer extends SaskenVisualizer {
	
	private boolean debug = false;
	private boolean EmptyOn = false;
	private boolean visSurface = false; //It is true if the surface for the Visualization has been created
	private int w, h;  //view Width and view Height
	private SurfaceHolder myholder;
	private Canvas c;

	public EmptyVisualizer(Context context, AttributeSet attrs) {
		super(context, attrs);
		myholder = getHolder();
		myholder.addCallback(this);
	}

	@Override
	public void Init(int sampleRateInHz, int numOfChannels) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean startVisualization() {
		EmptyOn = true;
		return false;
	}

	@Override
	public boolean isVisualizationOn() {
		return EmptyOn = true;
	}

	@Override
	public boolean stopVisualization() {
		EmptyOn = false;
		return false;
	}

	@Override
	public void processBuffer(byte[] input) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		startVisualization();
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		if(debug)Log.d("VI_Empty", "Surface Created");
		visSurface = true;
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		if(debug)Log.d("VI_Empty", "Surface Destroyed");
		visSurface = false;
	}

	@Override
	public boolean isSurfaceCreated() {
		// TODO Auto-generated method stub
		return visSurface;
	}

	@Override
	public void drawOn() {
		// TODO Auto-generated method stub
		c = null;
		c = myholder.lockCanvas();
		c.drawColor(Color.BLACK);
		myholder.unlockCanvasAndPost(c);
	}

	@Override
	public void drawOff() {
		// TODO Auto-generated method stub
		
	}
}