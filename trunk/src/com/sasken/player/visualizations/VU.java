package com.sasken.player.visualizations;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;


public class VU extends SaskenVisualizer{

	private boolean debug = false;
	//private boolean draw = false;
	private Canvas myCanvas;
	private Rect barRect;
	private VUBars RightChannel, LeftChannel;
	private int w, h;  //view Width and view Height
	//private long barWidth = 0, prebarWidth = 0;
	private double rightChannelPow = -50, leftChannelPow = -50;
	private short[] rightChannelSamples;
	private short[] leftChannelSamples;
	private double[] avgPow = new double[2];
	private boolean VUon = false;
	private boolean VUSurface = false; //It is true if the surface for the Visualization has been created
	private int sampleRate = 0, numChannels = 1;
	private int limit = 6000/20;//32768/20;
	private SurfaceHolder myholder;
	private Canvas c;
	
	private long handle;
	
	public VU(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		myholder = getHolder();
		myholder.addCallback(this);
		barRect = new Rect(0,0,0,0);
		RightChannel = new VUBars();
		LeftChannel = new VUBars();
		RightChannel.init(VUBars.HORIZONTAL, VUBars.NOTFILL, 10, 3);
		LeftChannel.init(VUBars.HORIZONTAL, VUBars.NOTFILL, 10, 15);
	}

	@Override
	public void Init(int sampleRateInHz, int numOfChannels) {
		// TODO Auto-generated method stub
		
		//handle = Init();
		
		sampleRate = sampleRateInHz;
		numChannels = numOfChannels;
		if(debug)Log.d("VU", "About to setting up Channels");
		setNumOfChannels(numChannels);
		if(debug)Log.d("VU", "set up Channels");
		
		//setBuffer(50);
	}

	@Override
	public boolean startVisualization() {
		// TODO Auto-generated method stub
		VUon = true;
		return true;
	}

	@Override
	public boolean stopVisualization() {
		// TODO Auto-generated method stub
		VUon = false;
		return false;
	}

	@Override
	public void processBuffer(byte[] input) {
		// TODO Auto-generated method stub
		
		//barWidth = getAvgVolume(input);
		//barWidth = getPowVolume(input);

		// no vamos a procesar por el momento
		//avgPow = getPowVolume(input);
		
		getPowVolume(input, avgPow);
		
		if (numChannels == 1){
			leftChannelPow = avgPow[0];
			rightChannelPow = leftChannelPow;
		}else if(numChannels == 2){
			leftChannelPow = avgPow[0];
			rightChannelPow = avgPow[1];
		}
		
		
		/***
		short[] buffer = new short[input.length/2];
		
		buffer = copyByteArrayToShortArrayLE(input, buffer);
		
		if (numChannels == 1){
			leftChannelPow = getPowVolume(buffer);
			rightChannelPow = leftChannelPow;
		}else if(numChannels == 2){
			divideSamples(buffer);
			leftChannelPow = getPowVolume(leftChannelSamples);
			rightChannelPow = getPowVolume(rightChannelSamples);
		}**/
		
	}
	
	private void getValues(){
		avgPow = readPowValues();
		
		if (numChannels == 1){
			leftChannelPow = avgPow[0];
			rightChannelPow = leftChannelPow;
		}else if(numChannels == 2){
			leftChannelPow = avgPow[0];
			rightChannelPow = avgPow[1];
		}
	}
	
	@Override
	public boolean isVisualizationOn() {
		// TODO Auto-generated method stub
		return VUon;
	}
	
	static {
		System.loadLibrary("VU");
		System.loadLibrary("VI20Band");
		System.loadLibrary("VI10BandLeds");
	}
	
	public native void Start();
	
	public native void Stop();
	
	public native void getPowVolume(byte[] inputBuffer, double[] output);
	
	public native int setNumOfChannels(int numOfChannels);
	
	public native long Init();
	
	public native void Release();
	
	public native int[] getPowVolumeData(long handle);
	
	public native double[] readPowValues();
	
	private int defineNumOfLedsOn(long amplitud){
		if ( (amplitud > 0) && (amplitud < limit) ) {
			return 1;
		}else if( (amplitud > limit) && (amplitud < limit*2) ){
			return 2;
		}else if ( (amplitud > limit*2) && (amplitud < limit*4) ){
			return 3;
		}else if ( (amplitud > limit*4) && (amplitud < limit*6) ){
			return 4;
		}else if ( (amplitud > limit*6) && (amplitud < limit*9) ){
			return 5;
		}else if ( (amplitud > limit*9) && (amplitud < limit*12) ){
			return 6;
		}else if ( (amplitud > limit*12) && (amplitud < limit*14) ){
			return 7;
		}else if ( (amplitud > limit*14) && (amplitud < limit*16) ){
			return 8;
		}else if ( (amplitud > limit*16) && (amplitud < limit*18) ){
			return 9;
		}else if ( (amplitud > limit*18)){
			return 10;
		}
		return 0;
	}
	
	private int defineNumOfLedsOnDB(double amplitud){
		if (amplitud <= -30){
			return 0;
		}else if ( (amplitud > -30) && (amplitud <= -27.0) ) {
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
	
	private void divideSamples( short[] samples){
		leftChannelSamples = new short[samples.length/2];
		rightChannelSamples = new short[samples.length/2];
		for (int i = 0; i < samples.length/2; i++){
			leftChannelSamples[i] = samples[i*2];
			rightChannelSamples[i] = samples[i*2+1];
		}
	}
	
	public short[] copyByteArrayToShortArrayLE(byte[] buffer8, short[] buffer16) {
		for(int i = 0; i < buffer16.length; i += 2){
			buffer16[i >>> 1] = (short) ((buffer8[i] & 0xff) | ((buffer8[i + 1] & 0xff) << 8));
		}
		return buffer16;
	}
	
	public double getRightChannelPower(){
		return rightChannelPow;
	}
	
	public double getLeftChannelPower(){
		return leftChannelPow;
	}
	
	public void changeBarType(){
		if(RightChannel.getBarType() == VUBars.FILL)
			RightChannel.setBarType(VUBars.NOTFILL);
		else if (RightChannel.getBarType() == VUBars.NOTFILL)
			RightChannel.setBarType(VUBars.FILL);
		
		if(LeftChannel.getBarType() == VUBars.FILL)
			LeftChannel.setBarType(VUBars.NOTFILL);
		else if (LeftChannel.getBarType() == VUBars.NOTFILL)
			LeftChannel.setBarType(VUBars.FILL);
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		// TODO Auto-generated method stub
		w = width;
		h = height;
		if(debug)Log.d("VU", "View Width = "+w);
		if(debug)Log.d("VU", "View Height = "+h);
		VUSurface = true;
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		VUSurface = false;
	}
	
	private void callback(double left, double right){
		Log.d("VU", "I'm been called from JNI! =D");
		leftChannelPow = left;
		rightChannelPow = right;
	}
	
	private void callbackStart(){
		Log.d("VU", "Global References Created");
	}

	@Override
	public boolean isSurfaceCreated() {
		// TODO Auto-generated method stub
		return VUSurface;
	}

	@Override
	public void drawOn() {
		//super.onDraw(c);
		if(VUon){
			if(debug)Log.d("VU", "Trying to draw something");
			getValues();
			c = null;
			c = myholder.lockCanvas();
			
			c.drawColor(Color.BLACK);
			
			if(debug)Log.d("VU", "leftChannelPow is "+leftChannelPow);
			if(debug)Log.d("VU", "rightChannelPow is "+rightChannelPow);
			
			RightChannel.setOnBars(defineNumOfLedsOnDB(rightChannelPow));
			LeftChannel.setOnBars(defineNumOfLedsOnDB(leftChannelPow));
			
			//RightChannel.constructBar();
			//LeftChannel.constructBar();
			//RightChannel.drawBar(c);
			//LeftChannel.drawBar(c);
			
			//RightChannel.drawBarDirectly(c);
			//LeftChannel.drawBarDirectly(c);
			
			//c.drawBitmap(LeftChannel.getBarDrawable(), 10, 3, null);
			//c.drawBitmap(LeftChannel.getBarDrawable(), 10, 13, null);
			
			//RightChannel.drawBarRect(c);
			//LeftChannel.drawBarRect(c);
			
			RightChannel.drawBar(c);
			LeftChannel.drawBar(c);
			
			myholder.unlockCanvasAndPost(c);
		}
	}

	@Override
	public void drawOff() {
		c = myholder.lockCanvas();
		c.drawColor(Color.BLACK);
		myholder.unlockCanvasAndPost(c);
	}
	
}
