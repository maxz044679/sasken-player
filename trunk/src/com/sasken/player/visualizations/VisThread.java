package com.sasken.player.visualizations;

import android.util.Log;

public class VisThread extends Thread{
	
	private boolean debug = true;
	private boolean isRunning = false;
	private SaskenVisualizer myVU, myVis;
	
	public VisThread (SaskenVisualizer mySasVis1, SaskenVisualizer mySasVis2){
		myVU = mySasVis1;
		myVis = mySasVis2;
	}
	
	@Override
	public void run(){
		if(debug)Log.d("VisThread", "Thread Started");
		while(isRunning){
			//TODO call draw() in both visualizers
//			if(debug)Log.d("VisThread", "Thread is Running!!");
			if( myVU.isVisualizationOn() && myVU.isSurfaceCreated() ){
				myVU.drawOn();
			} else if( !myVU.isVisualizationOn() && myVU.isSurfaceCreated() ){
				myVU.drawOff();
			}
			if(myVis != null){
				if(myVis.isVisualizationOn() && myVis.isSurfaceCreated()){
					myVis.drawOn();
				}
			}
		}
		if(debug)Log.d("VisThread", "Thread Dead");
	}
	
	public void setRunning(boolean run){
		isRunning = run;
	}
	
	public void setVUVisualizer (SaskenVisualizer mySasVis){
		myVU = mySasVis;
	}
	
	public void setVisVisualizer (SaskenVisualizer mySasVis){
		myVis = mySasVis;
	}
}