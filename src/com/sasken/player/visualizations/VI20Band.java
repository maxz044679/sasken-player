package com.sasken.player.visualizations;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

public class VI20Band extends SaskenVisualizer {
	
	private boolean debug = false;
	private boolean Band20on = false;
	private boolean visSurface = false; //It is true if the surface for the Visualization has been created
	private int w, h;  //view Width and view Height
	private SurfaceHolder myholder;
	private Canvas c;
	private VUBars[] bars;
	private Context myContext;
	private double[] powValues = new double[20];
	
	private boolean up = true;
	private int count = 0;
	
	public VI20Band(Context context, AttributeSet attrs) {
		super(context, attrs);
		myContext = context;
		myholder = getHolder();
		myholder.addCallback(this);
		bars = new VUBars[20];
	}
	
/*	static {
		System.loadLibrary("VI20Band");
	}*/
	
	public native void getValues(double[] values);
	
	public native double[] readPowValues();

	@Override
	public void Init(int sampleRateInHz, int numOfChannels) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean startVisualization() {
		if(debug)Log.d("VI_20Band", "Started Visualization");
		Band20on = true;
		return false;
	}

	@Override
	public boolean isVisualizationOn() {
		return Band20on;
	}

	@Override
	public boolean stopVisualization() {
		Band20on = false;
		return false;
	}

	@Override
	public void processBuffer(byte[] input) {
		// TODO Auto-generated method stub
		getValues(powValues);
	}
	
	private void getValues(){
		powValues = readPowValues();
	}
	
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		w = width;
		h = height;
		if(debug)Log.d("VI_20Band", "View Width = "+w);
		if(debug)Log.d("VI_20Band", "View Height = "+h);
		initializeVisualization();
		startVisualization();
		visSurface = true;
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		if(debug)Log.d("VI_20Band", "Surface Created");
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		if(debug)Log.d("VI_20Band", "Surface Destroyed");
		stopVisualization();
		visSurface = false;
	}
	
	public void initializeVisualization(){
		initializeBars();
		//constructBars();
		/*c = null;
		c = myholder.lockCanvas();
		c.drawColor(Color.BLACK);
		drawBars(c);
		myholder.unlockCanvasAndPost(c);*/
	}
	
	public void initializeBars(){
		int barsGap = 14; //Space between bars
		for (int i = 0; i < 20; i++){
			bars[i] = new VUBars();
			bars[i].init(VUBars.VERTICAL, VUBars.FILL, 10+barsGap*i, 5);
			bars[i].setBarDimentions(6, 14, 4);
			bars[i].setOnBars(0);
		}
	}
	
	public void setOnBars(){
		/*for (int i = 0; i < 20; i++){
			bars[i].setOnBars(0);
		}*/
		updateAmplitud();
	}
	
	public void constructBars(){
		for (int i = 0; i < 20; i++){
			bars[i].constructBar();
		}
	}
	
	public void drawBars(Canvas c){
		for (int i = 0; i < 20; i++){
			bars[i].drawBar(c);
			//bars[i].drawBarDirectly(c);
		}
	}
	
	public void updateAmplitud(){
		//Log.d("VI_20Band", "powValues[0] = "+powValues[0]+" "+defineNumOfLedsOnDB(powValues[0]));
		for(int i = 0; i < 20; i++){
			bars[i].setOnBars(defineNumOfLedsOnDB(powValues[i]));
		}
	}
	
	public void updateAmplitudDemo(){
		if(up)
			count++;
		if(!up)
			count--;
		if(count >= 10 && up)
			up = false;
		if(count <= 0 && !up)
			up = true;
		for(int i = 19; i > 0; i--){
			bars[i].setOnBars(bars[i-1].getOnBars());
		}
		bars[0].setOnBars(count);
	}
	
	private int defineNumOfLedsOnDB(double amplitud){
		if (amplitud <= -30.0){
			return 0;
		}else if ( (amplitud > -30.0) && (amplitud <= -27.0) ) {
			return 1;
		}else if( (amplitud > -27.0) && (amplitud <= -24.0) ){
			return 2;
		}else if ( (amplitud > -24.0) && (amplitud <= -21.0) ){
			return 3;
		}else if ( (amplitud > -21.0) && (amplitud <= -18.0) ){
			return 4;
		}else if ( (amplitud > -18.0) && (amplitud <= -15.0) ){
			return 5;
		}else if ( (amplitud > -15.0) && (amplitud <= -12.0) ){
			return 6;
		}else if ( (amplitud > -12.0) && (amplitud <= -9.0) ){
			return 7;
		}else if ( (amplitud > -9.0) && (amplitud <= -6.0) ){
			return 8;
		}else if ( (amplitud > -6.0) && (amplitud <= -3.0) ){
			return 9;
		}else if ( (amplitud > -3.0) && (amplitud < 0.0) || (amplitud >= 0.0)){
			return 10;
		}
		return 0;
	}
	
	private void callback(){
		//Log.d("VI_20Band", "I'm been called from JNI! =D");
	}

	@Override
	public boolean isSurfaceCreated() {
		// TODO Auto-generated method stub
		return visSurface;
	}

	@Override
	public void drawOn() {
		// TODO Auto-generated method stub
		if(debug)Log.d("VI_20Band", "Trying to draw something");
		if(Band20on && visSurface){
			if(debug)Log.d("VI_20Band", "Something drawed");
			c = null;
			c = myholder.lockCanvas();
			c.drawColor(Color.BLACK);
			getValues();
			setOnBars();
			//constructBars();
			drawBars(c);
			myholder.unlockCanvasAndPost(c);
		}
		
	}

	@Override
	public void drawOff() {
		// TODO Auto-generated method stub
		
	}
}