package com.sasken.player.visualizations;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Picture;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.PictureDrawable;

import com.sasken.player.R;

public class VUBars {

	public static final int HORIZONTAL = 1;
	public static final int VERTICAL = 2;
	public static final int FILL = 1;
	public static final int NOTFILL = 0;
	private int orientation;
	//private int numberOfBars;
	private int x;  //variable that holds where to draw the bars
	private int y;  //variable that holds where to draw the bars
	private int barWidth = 5, barHeight = 13;
	private int barGap = 6;
	private int numBarsOn = 0;
	private int barType = 0;
	private Canvas c;
	private RectF singleBar;
	private Rect bar, bkBar;
	private Paint greenOff, greenOn, yellowOff, yellowOn, redOff, redOn, bkBarPaint;
	//private Paint greenOnFX;
	private Context context; 
	
	private PictureDrawable barPictureDrawable;
	private Picture barPicture;

	public VUBars(){
		singleBar = new RectF(0,0,0,0);
		bar = new Rect(0,0,0,0);
		bkBar = new Rect(0,0,0,0);
		bkBarPaint = new Paint();
		greenOff = new Paint();
		greenOn = new Paint();
		//greenOnFX = new Paint();
		yellowOff = new Paint();
		yellowOn = new Paint();
		redOff = new Paint();
		redOn = new Paint();

		c = new Canvas();
		
		barPicture = new Picture();
		barPictureDrawable = new PictureDrawable(barPicture);
	}
	
	public VUBars(Context mContext){
		context = mContext;
		singleBar = new RectF(0,0,0,0);
		bar = new Rect(0,0,0,0);
		bkBar = new Rect(0,0,0,0);
		bkBarPaint = new Paint();
		greenOff = new Paint();
		greenOn = new Paint();
		//greenOnFX = new Paint();
		yellowOff = new Paint();
		yellowOn = new Paint();
		redOff = new Paint();
		redOn = new Paint();

		c = new Canvas();
		
		barPicture = new Picture();
		barPictureDrawable = new PictureDrawable(barPicture);
	}
	
	//public void init(Canvas canvas, int orientation, int howMany, int screenX, int screenY){
	public void init(int orientation, int type, int screenX, int screenY){
		//c = canvas;
		this.orientation = orientation;
		barType = type;
		x = screenX;
		y = screenY;
		initPaintsColors();
		updateBarDimentions();
	}
	
	private void initPaintsColors(){
		greenOff.setARGB(255, 0, 135, 0);
		greenOn.setColor(Color.GREEN);
		greenOn.setAlpha(255);
		greenOn.setAntiAlias(true);
		greenOn.setDither(true);
		//greenOnFX.setARGB(174, 0, 255, 0);
		yellowOff.setARGB(255, 140, 138, 0);
		yellowOn.setColor(Color.YELLOW);
		yellowOn.setAlpha(255);
		yellowOn.setAntiAlias(true);
		yellowOn.setDither(true);
		redOff.setARGB(255, 135, 0, 0);
		redOn.setColor(Color.RED);
		redOn.setAlpha(255);
		redOn.setAntiAlias(true);
		redOn.setDither(true);
		bkBarPaint.setARGB(255, 50, 50, 50);
	}
	
	public void constructBar(){
		int barShift = 0; //how much the next bar is going to move to the right or bottom
		int x=0, y=0;
		
		//Canvas c = barPictureDrawable.getPicture().beginRecording((barHeight+barGap)*10, barWidth);
		//Canvas c = barPictureDrawable.getPicture().beginRecording(140, 10);
		//Canvas c = barPictureDrawable.getPicture().beginRecording(300, 300);
		//c.drawColor(Color.BLACK);
		if (orientation == HORIZONTAL){
			c = barPictureDrawable.getPicture().beginRecording((barHeight+barGap)*10, barWidth);
			barShift = barHeight+barGap;
			//singleBar.set(x,y,x+barHeight, y+barWidth);
			for(int i = 0; i < 5; i++){
				singleBar.set(x+barShift*i, y, x+barHeight+barShift*i, y+barWidth);
				if(numBarsOn > i)
					c.drawRect(singleBar, greenOn);
				else
					c.drawRect(singleBar, greenOff);
			}
			for(int i = 5; i < 8; i++){
				singleBar.set(x+barShift*i, y, x+barHeight+barShift*i, y+barWidth);
				if(numBarsOn > i)
					c.drawRect(singleBar, yellowOn);
				else
					c.drawRect(singleBar, yellowOff);
			}
			for(int i = 8; i < 10; i++){
				singleBar.set(x+barShift*i, y, x+barHeight+barShift*i, y+barWidth);
				if(numBarsOn > i)
					c.drawRect(singleBar, redOn);
				else
					c.drawRect(singleBar, redOff);
			}
		}
		if (orientation == VERTICAL){
			c = barPictureDrawable.getPicture().beginRecording(barWidth, (barHeight+barGap)*10);
			barShift = barHeight+barGap;
			//singleBar.set(x,y, x+barWidth, y+barHeight);
			for(int i = 0; i < 2; i++){
				singleBar.set(x,y+barShift*i, x+barWidth, y+barHeight+barShift*i);
				if(numBarsOn > i)
					c.drawRect(singleBar, redOn);
				else
					c.drawRect(singleBar, redOff);
			}
			for(int i = 2; i < 5; i++){
				singleBar.set(x,y+barShift*i, x+barWidth, y+barHeight+barShift*i);
				if(numBarsOn > i)
					c.drawRect(singleBar, yellowOn);
				else
					c.drawRect(singleBar, yellowOff);
			}
			for(int i = 5; i < 10; i++){
				singleBar.set(x,y+barShift*i, x+barWidth, y+barHeight+barShift*i);
				if(numBarsOn > i)
					c.drawRect(singleBar, greenOn);
				else
					c.drawRect(singleBar, greenOff);
			}
			
		}
		barPictureDrawable.getPicture().endRecording();
	}
	
	public void drawBar(Canvas c){
		if(barType == NOTFILL){
			/*constructBar();
			if (orientation == HORIZONTAL)
				barPictureDrawable.setBounds(x, y, x+(barHeight+barGap)*10, y+barWidth);
				//barPictureDrawable.setBounds(x, y, x+140, y+10);
			if (orientation == VERTICAL)
				barPictureDrawable.setBounds(x, y, x+barWidth, y+(barHeight+barGap)*10);
			barPictureDrawable.draw(c);*/
			if (context == null){
				drawBarDirectly(c);
			}
			else if(context != null){
				c.drawBitmap(getBarDrawable(), x, y, null);
			}
		}
		else if(barType == FILL){
			drawBarRect(c);
		}
	}
	
	public void drawBarDirectly(Canvas c){
		int barShift = 0; //how much the next bar is going to move to the right or bottom
		//int x=0, y=0;
		
		if (orientation == HORIZONTAL){
			barShift = barHeight+barGap;
			for(int i = 0; i < 5; i++){
				singleBar.set(x+barShift*i, y, x+barHeight+barShift*i, y+barWidth);
				if(numBarsOn > i){
					c.drawRect(singleBar, greenOn);
					drawBarLight(c, singleBar, greenOn);
				}
				else
					c.drawRect(singleBar, greenOff);
			}
			for(int i = 5; i < 8; i++){
				singleBar.set(x+barShift*i, y, x+barHeight+barShift*i, y+barWidth);
				if(numBarsOn > i){
					c.drawRect(singleBar, yellowOn);
					drawBarLight(c, singleBar, yellowOn);
				}
				else
					c.drawRect(singleBar, yellowOff);
			}
			for(int i = 8; i < 10; i++){
				singleBar.set(x+barShift*i, y, x+barHeight+barShift*i, y+barWidth);
				if(numBarsOn > i){
					c.drawRect(singleBar, redOn);
					drawBarLight(c, singleBar, redOn);
				}
				else
					c.drawRect(singleBar, redOff);
			}
		}
		if (orientation == VERTICAL){
			barShift = barHeight+barGap;
			for(int i = 0; i < 2; i++){
				singleBar.set(x,y+barShift*i, x+barWidth, y+barHeight+barShift*i);
				if(numBarsOn > i){
					c.drawRect(singleBar, redOn);
					drawBarLight(c, singleBar, redOn);
				}
				else
					c.drawRect(singleBar, redOff);
			}
			for(int i = 2; i < 5; i++){
				singleBar.set(x,y+barShift*i, x+barWidth, y+barHeight+barShift*i);
				if(numBarsOn > i){
					c.drawRect(singleBar, yellowOn);
					drawBarLight(c, singleBar, yellowOn);
				}
				else
					c.drawRect(singleBar, yellowOff);
			}
			for(int i = 5; i < 10; i++){
				singleBar.set(x,y+barShift*i, x+barWidth, y+barHeight+barShift*i);
				if(numBarsOn > i){
					c.drawRect(singleBar, greenOn);
					drawBarLight(c, singleBar, greenOn);
				}
				else
					c.drawRect(singleBar, greenOff);
			}
			
		}
	}
	
	public void drawBarRect(Canvas c){
		c.drawRect(bkBar, bkBarPaint);
		if (orientation == HORIZONTAL)
			if(numBarsOn <= 5){
				c.drawRect(bar, greenOff);
				bar.right = numBarsOn*(barHeight+barGap);
				c.drawRect(bar, greenOn);
			}
			else if(numBarsOn > 5 && numBarsOn <= 8){
				c.drawRect(bar, yellowOff);
				bar.right = numBarsOn*(barHeight+barGap);
				c.drawRect(bar, yellowOn);
			}
			else if(numBarsOn > 8 ){
				c.drawRect(bar, redOff);
				bar.right = numBarsOn*(barHeight+barGap);
				c.drawRect(bar, redOn);
			}
		if (orientation == VERTICAL){
			if(numBarsOn <= 5){
				c.drawRect(bar, greenOff);
				bar.top = (barHeight+barGap)*10-numBarsOn*(barHeight+barGap);
				c.drawRect(bar, greenOn);
			}
			else if(numBarsOn > 5 && numBarsOn <= 8){
				c.drawRect(bar, yellowOff);
				bar.top = (barHeight+barGap)*10-numBarsOn*(barHeight+barGap);
				c.drawRect(bar, yellowOn);
			}
			else if(numBarsOn > 8 ){
				c.drawRect(bar, redOff);
				bar.top = (barHeight+barGap)*10-numBarsOn*(barHeight+barGap);
				c.drawRect(bar, redOn);
			}
		}
		
	}
	
	private void drawBarLight(Canvas c, RectF rect, Paint paint){
		float left;
		float top;
		float right;
		float bottom;
		left = rect.left;
		top = rect.top;
		right = rect.right;
		bottom = rect.bottom;
		
		paint.setAlpha(40);
		
		for (int i = 3; i >= 0; i--){
			rect.set(rect.left-i, rect.top-i, rect.right+i, rect.bottom+i);
			c.drawRoundRect(rect, 5, 5, paint);
		}
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
		paint.setAlpha(255);
	}
	
	public void setOnBars(int bars){
		numBarsOn = bars;
	}
	
	public PictureDrawable getDrawable(){
		return barPictureDrawable;
	}
	
	public void setPosition(int xPos, int yPos){
		x = xPos;
		y = yPos;
		bar.left = x;
		bar.top += y;
		bar.bottom += y;
	}
	
	public Bitmap getBarDrawable(){
		switch (numBarsOn){
		case 0:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_0);
		case 1:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_1);
		case 2:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_2);
		case 3:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_3);
		case 4:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_4);
		case 5:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_5);
		case 6:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_6);
		case 7:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_7);
		case 8:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_8);
		case 9:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_9);
		case 10:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_10);
		default:
			return BitmapFactory.decodeResource(context.getResources(), R.drawable.bar_0);
		}
	}
	
	public int getOnBars(){
		return numBarsOn;
	}
	
	public int getBarType(){
		return barType;
	}
	
	public void setBarType(int type){
		barType = type;
	}
	
	public void setBarWidth(int w){
		barWidth = w;
	}
	
	public void setbarHeight(int h){
		barHeight = h;
	}
	
	public void setBarGap(int gap){
		barGap = gap;
	}
	
	public void setBarDimentions(int width, int height, int gap){
		barWidth = width;
		barHeight = height;
		barGap = gap;
		updateBarDimentions();
	}
	
	public void updateBarDimentions(){
		if(orientation == HORIZONTAL){
			bar.left = x;
			bar.top = y;
			bar.bottom = barWidth+y;
			bar.right = 4;
			bkBar.left = x;
			bkBar.top = y;
			bkBar.bottom = barWidth+y;
			bkBar.right = x+(barHeight+barGap)*10;
		}else if (orientation == VERTICAL){
			bar.left = x;
			bar.top = y+(barHeight+barGap)*10-4;
			bar.right = barWidth+x;
			bar.bottom = y+(barHeight+barGap)*10;
			bkBar.left = x;
			bkBar.top = y;
			bkBar.right = barWidth+x;
			bkBar.bottom = y+(barHeight+barGap)*10;
		}
	}
}
