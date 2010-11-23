package com.sasken.player.visualizations;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

public class VI10BandLeds extends SaskenVisualizer {
	
	private boolean debug = false;
	private boolean Band10Ledson = false;
	private boolean visSurface = false; //It is true if the surface for the Visualization has been created
	private int w, h;  //view Width and view Height
	private SurfaceHolder myholder;
	private Canvas c;
	private VUBars[] bars;
	private double[] powValues = new double[2];

	public VI10BandLeds(Context context, AttributeSet attrs) {
		super(context, attrs);
		myholder = getHolder();
		myholder.addCallback(this);
		bars = new VUBars[10];
	}
	
	public native double[] getPowValues();
	
	public void initializeVisualization(){
		int barsGap = 28; //Space between bars
		for (int i = 0; i < 10; i++){
			bars[i] = new VUBars();
			bars[i].init(VUBars.VERTICAL, VUBars.NOTFILL, 15+barsGap*i, 5);
			bars[i].setBarDimentions(6, 8, 15);
			bars[i].setOnBars(0);
		}
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		w = width;
		h = height;
		initializeVisualization();
		startVisualization();
		visSurface = true;
		
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		visSurface = false;
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		stopVisualization();
				
	}

	@Override
	public void Init(int sampleRateInHz, int numOfChannels) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean startVisualization() {
		Band10Ledson = true;
		return false;
	}

	@Override
	public boolean isVisualizationOn() {
		return Band10Ledson;
	}

	@Override
	public boolean stopVisualization() {
		Band10Ledson = false;
		return false;
	}

	@Override
	public boolean isSurfaceCreated() {
		return visSurface;
	}

	@Override
	public void processBuffer(byte[] input) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void drawOn() {
		c = null;
		c = myholder.lockCanvas();
		c.drawColor(Color.BLACK);
		getValues();
		setOnBars();
		drawBars(c);
		myholder.unlockCanvasAndPost(c);		
	}

	@Override
	public void drawOff() {
		// TODO Auto-generated method stub
		
	}
	
	private void getValues(){
		powValues = getPowValues();
	}
	
	private void setOnBars(){
		for(int i = 0; i < 10; i += 2){
			bars[i].setOnBars(defineNumOfLedsOnDB(powValues[0]));
			bars[i+1].setOnBars(defineNumOfLedsOnDB(powValues[1]));
		}
	}
	
	private void drawBars(Canvas c){
		for (int i = 0; i < 10; i++){
			bars[i].drawBar(c);
		}
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
	
}