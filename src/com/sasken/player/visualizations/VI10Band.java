package com.sasken.player.visualizations;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

public class VI10Band extends SaskenVisualizer{
	
	private boolean debug = false;
	private boolean visSurface = false; //It is true if the surface for the Visualization has been created
	private boolean Band10on = false;
	private int w, h;  //view Width and view Height
	private int hn;   //is the Height normalized so the bars will draw in the whole view available.
	private Rect[] bars;
	private Paint barPaintGreen, barPaintYellow, barPaintRed;
	private double[] amplitud;
	private boolean up = true;
	private int count = 0;
	private SurfaceHolder myholder;
	private Canvas c;
	
	public VI10Band(Context context, AttributeSet attrs) {
		super(context, attrs);

		myholder = getHolder();
		myholder.addCallback(this);
		bars = new Rect[10];
		barPaintGreen = new Paint();
		barPaintYellow = new Paint();
		barPaintRed = new Paint();
		barPaintGreen.setColor(Color.GREEN);
		barPaintYellow.setColor(Color.YELLOW);
		barPaintRed.setColor(Color.RED);
		amplitud = new double[10];
		for (int i = 0; i < 10; i++){
			bars[i] = new Rect();
		}
		if(debug)Log.d("VI_10Band", "Construct VI_10Band(Context context, AttributeSet attrs) Called");
	}
	
	public void Init(int sampleRateInHz, int numOfChannels) {
		// TODO Auto-generated method stub
	}

	public boolean startVisualization() {
		if(debug)Log.d("VI_10Band", "Started Visualization");
		Band10on = true;
		return false;
	}

	public boolean isVisualizationOn() {
		return Band10on;
	}

	public boolean stopVisualization() {
		Band10on = false;
		return false;
	}

	public void processBuffer(byte[] input) {
		// TODO Auto-generated method stub
		
	}

	
	public void setBarsHeight(double[] amplitud){
		for(int i = 0; i < 10; i++){
			bars[i].top = hn*(int) ((int) (-1)*amplitud[i]);
			//bars[i].top = 0+h/50*5*i;
			//if(debug)Log.d("VI_10Band", "bar top "+i+" = "+bars[i].top);
			
		}
	}
	

/*	@Override
	public void onSizeChanged(int viewWidth, int viewHeight, int oldViewWidth, int oldViewHeight){
		w = viewWidth;
		h = viewHeight;
		hn = h/100;
		if(debug)Log.d("VI_10Band", "View Width = "+w);
		if(debug)Log.d("VI_10Band", "View Height = "+h);
		for(int i = 0; i < 10; i++){
			bars[i].set(0+w/10*i, 0, w/10+w/10*i, h);
		}		
	}*/
	
	public void initializeVisualization(){
		fillAmplitud();
		c = null;
		c = myholder.lockCanvas();
		c.drawColor(Color.BLACK);
		setBarsHeight(amplitud);
		for(int i = 0; i < 10; i++){
			c.drawRect(bars[i], barPaintGreen);
		}
		myholder.unlockCanvasAndPost(c);
	}
	
	public void fillAmplitud(){
		for(int i = 0 ; i < 10; i++){
			//amplitud[i] = i*(-10);
			amplitud[i] = -120;
			//if(debug)Log.d("VI_10Band", "Amplitud "+i+" = "+amplitud[i]);
		}
	}
	
	public void updateAmplitud(){
		if(up)
			count += -10;
		if(!up)
			count -= -10;
		if(count <= -100 && up)
			up = false;
		if(count >= 0 && !up)
			up = true;
		for(int i = 9; i > 0; i--){
			amplitud[i] = amplitud[i-1];
		}
		amplitud[0] = count;
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		// TODO Auto-generated method stub
		w = width;
		h = height;
		hn = h/100;
		if(debug)Log.d("VI_10Band", "View Width = "+w);
		if(debug)Log.d("VI_10Band", "View Height = "+h);
		for(int i = 0; i < 10; i++){
			bars[i].set(0+w/10*i, 0, w/10+w/10*i, h);
		}
		initializeVisualization();
		startVisualization();
		visSurface = true;
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		if(debug)Log.d("VI_10Band", "Surface Created");
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		if(debug)Log.d("VI_10Band", "Surface Destroyed");
		stopVisualization();
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
		if(debug)Log.d("VI_10Band", "Trying to draw something");
		if(debug)Log.d("VI_10Band", "Something drawed");
		c = null;
		c = myholder.lockCanvas();
		c.drawColor(Color.BLACK);
		updateAmplitud();
		setBarsHeight(amplitud);
		for(int i = 0; i < 10; i++){
			c.drawRect(bars[i], barPaintGreen);
		}
		myholder.unlockCanvasAndPost(c);
	}

	@Override
	public void drawOff() {
		// TODO Auto-generated method stub
		c = null;
		c = myholder.lockCanvas();
		c.drawColor(Color.BLACK);
		myholder.unlockCanvasAndPost(c);
	}
}