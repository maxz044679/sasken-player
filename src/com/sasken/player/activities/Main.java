package com.sasken.player.activities;

import static android.provider.BaseColumns._ID;
import static com.sasken.player.databases.TableConstants.BAND1;
import static com.sasken.player.databases.TableConstants.BAND10;
import static com.sasken.player.databases.TableConstants.BAND2;
import static com.sasken.player.databases.TableConstants.BAND3;
import static com.sasken.player.databases.TableConstants.BAND4;
import static com.sasken.player.databases.TableConstants.BAND5;
import static com.sasken.player.databases.TableConstants.BAND6;
import static com.sasken.player.databases.TableConstants.BAND7;
import static com.sasken.player.databases.TableConstants.BAND8;
import static com.sasken.player.databases.TableConstants.BAND9;
import static com.sasken.player.databases.TableConstants.PREAMP;
import static com.sasken.player.databases.TableConstants.STATUS;
import static com.sasken.player.databases.TableConstants.TABLE_STATE;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.ContentValues;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;

import com.sasken.player.R;
import com.sasken.player.databases.SettingsDB;
import com.sasken.player.decoders.AudioPlayer;
import com.sasken.player.decoders.SaskenDecoder;
import com.sasken.player.decoders.SaskenDecoder.OnPlaybackCompletedListener;
import com.sasken.player.equalizers.SaskenEqualizer;
import com.sasken.player.visualizations.SaskenVisualizer;
import com.sasken.player.visualizations.VI20Band;
import com.sasken.player.visualizations.VU;
import com.sasken.player.visualizations.VisThread;

public class Main extends Activity implements OnClickListener,
	OnPlaybackCompletedListener {

	/** SQL variables */
	private static String[] FROM = { STATUS, PREAMP, BAND1, BAND2, BAND3, BAND4, BAND5,
									BAND6, BAND7, BAND8, BAND9, BAND10};
	private static String[] STATUS_QRY = { _ID, STATUS};
	private static String ORDER_BY = _ID + " ASC";
	public static SettingsDB settings;
	private static SaskenEqualizer equalizer = null;
	VU myVU;
	VI20Band my20Band;
	SaskenVisualizer my10Band;
	SaskenVisualizer visualizer = null;
	public VisThread visThread;
	int visCount = 0;
	int vuCount = 0;
	int bufferSize = 100;
	byte buffer[];
	byte plus = 100;
	protected static final int REFRESH = 1000;
	AudioPlayer mPlayer;
	SaskenDecoder mDecoder;
	private List<String> mFiles = null;
	private final boolean mUseDirectDecoding = true;
	private final boolean mPlay = false;
	private int mSelected = 0;
	private final int numOfVis = 4;
	
	
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		myVU = (VU) findViewById(R.id.VU_Visualizer);
		equalizer = new SaskenEqualizer(); // Load equalizer shared library


		// Obtain the files from the SDCard
		GetFiles(new File("/sdcard").listFiles());
		DisplayFile();
		((Button) findViewById(R.id.ButtonPrevious)).setOnClickListener(this);
		((Button) findViewById(R.id.ButtonPlay)).setOnClickListener(this);
		((Button) findViewById(R.id.ButtonStop)).setOnClickListener(this);
		((Button) findViewById(R.id.ButtonVU)).setOnClickListener(this);
		((Button) findViewById(R.id.ButtonVisualizer)).setOnClickListener(this);

		/*------------------------------------*/
		myVU.Init(44100, 2);
		Log.d("Bla", "onCreate Ended");
		buffer = new byte[bufferSize];
		// buffer = fillBuffer(buffer, (short) 0);
		/*------------------------------------*/
		((Button) findViewById(R.id.ButtonEqSettings)).setOnClickListener(this);
		((Button) findViewById(R.id.ButtonNext)).setOnClickListener(this);
		visThread =  new VisThread(myVU, visualizer);
		visThread.setPriority(Thread.MIN_PRIORITY);
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		visThread.setRunning(true);
		if(visThread.getState() == Thread.State.NEW){
			visThread.start();
		}
		settings = new SettingsDB(this);
			try{
				if(emptyTable() == false){
					if(EqSettings.eq_status == false){
						getSettings(1);      
					}
					if(EqSettings.eqPanel3 == null){
						getSettings(2);      
					}
					if(EqSettings.eqPanel5 == null){
						getSettings(3);      
					}
					if(EqSettings.eqPanel10 == null){
						getSettings(4);      
					}
				}	
			} finally {
				settings.close(); 
				Log.d("EQ", "DB closed" );
			}
			Log.d("EQ", "onResume" );
	}
	
	@Override
	protected void onPause(){
		super.onPause();
	}

	private void DisplayFile() {
		EditText ed = (EditText) findViewById(R.id.EditTextSong);
		ed.setText(mFiles.get(mSelected));
	}

	private void GetFiles(File[] listFiles) {
		mFiles = new ArrayList<String>();
		for (File file : listFiles) {
			if (file.isFile()) {
				if (SaskenDecoder.CheckFile(file.getPath()) > 0) {
					Log.i("Main", "Found File '" + file.getPath() + "'");
					mFiles.add(file.getPath());
				}
			}
		}
	}

	private void PlayNewSong() {
		mDecoder = new SaskenDecoder(equalizer);

		EditText ed = (EditText) findViewById(R.id.EditTextSong);

		try {
			mDecoder.Init(ed.getText().toString());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		mDecoder.setOnCompletitionListener(this);
		mDecoder.Start();
	}

	Handler hRefresh = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub
			super.handleMessage(msg);
			switch (msg.what) {
			case REFRESH:
				break;
			}
		}

	};

	private void PlayNextSong() {
		if (mDecoder != null) {
			if (mDecoder.isPlaying()) {
				mDecoder.Stop();
			}
		}

		if (mSelected < mFiles.size() - 1) {
			mSelected++;

			DisplayFile();
			PlayNewSong();

		}

	}

/*================================================
 * 
 *			   SQL METHODS
 *
 * 				_|	   |_
 * 				\		/
 * 				 \     /
 * 				  \   /
 * 				   \ /
 *					V
 *================================================*/
	
	public static boolean emptyTable(){
		SQLiteDatabase db = settings.getWritableDatabase();
		Cursor cursor = db.query(TABLE_STATE, FROM, null, null, null, null, ORDER_BY);
		if (cursor.getCount() == 0){
			cursor.close();
			return true;
		}else
			cursor.close();
			return false;
	}

	public static void addInitialRows(){
		SQLiteDatabase db = settings.getWritableDatabase();
		ContentValues values = null;
		if(EqSettings.eq_status && EqSettings.nBands != 0){
			values = new ContentValues();
			values.put(STATUS, EqSettings.nBands);
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "On/Off row first time Created!");
		}
		if(EqSettings.eq_status && EqSettings.nBands == 0){
			values = new ContentValues();
			values.put(STATUS, 1);
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "On/Off row first time Created!");
		}
		if(EqSettings.eq_status == false && EqSettings.nBands == 0){
			values = new ContentValues();
			values.put(STATUS, 0);
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "On/Off row first time Created!");
		}
		if(EqSettings.eq_status == false && EqSettings.nBands != 0){
			values = new ContentValues();
			values.put(STATUS, EqSettings.nBands*(-1));
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "On/Off row first time Created!");
		}
		
		if(EqSettings.eqPanel3 != null){
			values = new ContentValues();
			values.put(STATUS, 1);
			values.put(PREAMP, EqSettings.eqPanel3[0]);
			values.put(BAND1, EqSettings.eqPanel3[1]);
			values.put(BAND2, EqSettings.eqPanel3[2]);
			values.put(BAND3, EqSettings.eqPanel3[3]);
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "3bands row first time Created!");
		}else{
			values = new ContentValues();
			values.put(STATUS, 0);
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "3bands row first time Created!");
		}
		  	  
		if(EqSettings.eqPanel5 != null){
			values = new ContentValues();
			values.put(STATUS, 1);
			values.put(PREAMP, EqSettings.eqPanel5[0]);
			values.put(BAND1, EqSettings.eqPanel5[1]);
			values.put(BAND2, EqSettings.eqPanel5[2]);
			values.put(BAND3, EqSettings.eqPanel5[3]);
			values.put(BAND4, EqSettings.eqPanel5[4]);
			values.put(BAND5, EqSettings.eqPanel5[5]);
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "5bands row first time Created!");
		}else{
			values = new ContentValues();
			values.put(STATUS, 0);
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "5bands row first time Created!");
		}
		  	  
		if(EqSettings.eqPanel10 != null){
			values = new ContentValues();
			values.put(STATUS, 1);
			values.put(PREAMP, EqSettings.eqPanel10[0]);
			values.put(BAND1, EqSettings.eqPanel10[1]);
			values.put(BAND2, EqSettings.eqPanel10[2]);
			values.put(BAND3, EqSettings.eqPanel10[3]);
			values.put(BAND4, EqSettings.eqPanel10[4]);
			values.put(BAND5, EqSettings.eqPanel10[5]);
			values.put(BAND6, EqSettings.eqPanel10[6]);
			values.put(BAND7, EqSettings.eqPanel10[7]);
			values.put(BAND8, EqSettings.eqPanel10[8]);
			values.put(BAND9, EqSettings.eqPanel10[9]);
			values.put(BAND10, EqSettings.eqPanel10[10]);
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "10bands row first time Created!");
		}else{
			values = new ContentValues();
			values.put(STATUS, 0);
			db.insertOrThrow(TABLE_STATE, null, values);
			Log.d("EQ", "10bands row first time Created!");
		}
	}
		
	public static void updateRows() {
			      // Insert a new record into the Events data source.
			      // You would do something similar for delete and update.
			      SQLiteDatabase db = settings.getWritableDatabase();
			      ContentValues values = new ContentValues();
			      
			      if(EqSettings.eq_status && EqSettings.nBands != 0){
			  		  values.put(STATUS, EqSettings.nBands);
			  		  db.update(TABLE_STATE, values, _ID+"=1", null);
			  		  Log.d("EQ", "ON, status row updated" );
			      }
			      if(EqSettings.eq_status && EqSettings.nBands == 0){
			    	  values.put(STATUS, 1);
			    	  db.update(TABLE_STATE, values, _ID+"=1", null);
			  		  Log.d("EQ", "ON, status row updated" );
			      }
			      if(EqSettings.eq_status == false && EqSettings.nBands == 0){
			    	  values.put(STATUS, 0);
			    	  db.update(TABLE_STATE, values, _ID+"=1", null);
			  		  Log.d("EQ", "OFF, status row updated" );
			      }
			      if(EqSettings.eq_status == false && EqSettings.nBands != 0){
			    	  values.put(STATUS, EqSettings.nBands*(-1));
			    	  db.update(TABLE_STATE, values, _ID+"=1", null);
			  		  Log.d("EQ", "OFF, status row updated" );
			      }
			      
			      if(EqSettings.eqPanel3 != null){
			    	  values = new ContentValues();
			    	  values.put(STATUS, 1);
			    	  values.put(PREAMP, EqSettings.eqPanel3[0]);
			  		  values.put(BAND1, EqSettings.eqPanel3[1]);
			  		  values.put(BAND2, EqSettings.eqPanel3[2]);
			  		  values.put(BAND3, EqSettings.eqPanel3[3]);
			  		  db.update(TABLE_STATE, values, _ID+"=2", null);
			  		  Log.d("EQ", "3bands row updated" );
			      }
			  	  
			      if(EqSettings.eqPanel5 != null){
			    	  values = new ContentValues();
			    	  values.put(STATUS, 1);
			  		  values.put(PREAMP, EqSettings.eqPanel5[0]);
			  		  values.put(BAND1, EqSettings.eqPanel5[1]);
			  		  values.put(BAND2, EqSettings.eqPanel5[2]);
			  		  values.put(BAND3, EqSettings.eqPanel5[3]);
			  		  values.put(BAND4, EqSettings.eqPanel5[4]);
			  		  values.put(BAND5, EqSettings.eqPanel5[5]);
			  		  db.update(TABLE_STATE, values, _ID+"=3", null);
			  		  Log.d("EQ", "5bands row updated" );
			      }
			  	  
			      if(EqSettings.eqPanel10 != null){
			    	  values = new ContentValues();
			    	  values.put(STATUS, 1);
			  		  values.put(PREAMP, EqSettings.eqPanel10[0]);
			  		  values.put(BAND1, EqSettings.eqPanel10[1]);
			  		  values.put(BAND2, EqSettings.eqPanel10[2]);
			  		  values.put(BAND3, EqSettings.eqPanel10[3]);
			  		  values.put(BAND4, EqSettings.eqPanel10[4]);
			  		  values.put(BAND5, EqSettings.eqPanel10[5]);
			  		  values.put(BAND6, EqSettings.eqPanel10[6]);
			  		  values.put(BAND7, EqSettings.eqPanel10[7]);
			  		  values.put(BAND8, EqSettings.eqPanel10[8]);
			  		  values.put(BAND9, EqSettings.eqPanel10[9]);
			  		  values.put(BAND10, EqSettings.eqPanel10[10]);
			  		  db.update(TABLE_STATE, values, _ID+"=4", null);
			  		  Log.d("EQ", "10bands row updated" );
			      }
			   }

	private void getSettings(int row) {
	    // Perform a managed query. The Activity will handle closing
	    // and re-querying the cursor when needed.
	    SQLiteDatabase db = settings.getReadableDatabase();
	    Cursor cursor = null;
	    Log.d("EQ", "Inside getSettings()" );
	    switch (row){
	    
		  case 1:
				cursor = db.query(TABLE_STATE, STATUS_QRY, _ID+"=1", null, null, null, ORDER_BY);
				startManagingCursor(cursor);
			    cursor.moveToFirst();
				if(cursor.getInt(1) > 1){
					EqSettings.eq_status = true;
					EqSettings.nBands = cursor.getInt(1);
					Log.d("EQ", "EQ ON / nBands set to: "+EqSettings.nBands);
				}
				if(cursor.getInt(1) == 1){
					EqSettings.eq_status = true;
					Log.d("EQ", "EQ ON / nBands set to: 0");
				}
				if(cursor.getInt(1) == 0){
					Log.d("EQ", "EQ OFF / nBands set to: 0");
				}
				if(cursor.getInt(1) < 0){
					EqSettings.nBands = ((cursor.getInt(1))*(-1));
					Log.d("EQ", "EQ OFF / nBands set to: "+EqSettings.nBands);
				}
				cursor.close();
				break;
	    
		  case 2:
			cursor = db.query(TABLE_STATE, FROM, _ID+"=2", null, null, null, ORDER_BY);
			startManagingCursor(cursor);
		    Log.d("EQ", "cursor is being managed" );
			cursor.moveToFirst();
			if(cursor.getInt(0) == 1){
				EqSettings.eqPanel3 = new int[4];
				for(int i=0; i<=3;i++){
					EqSettings.eqPanel3[i]=cursor.getInt(i+1);
					Log.d("EQ", "eqPanel3["+i+"] = "+cursor.getInt(i+1));
				}
			}
			cursor.close();
			break;
			
		  case 3:
			cursor = db.query(TABLE_STATE, FROM, _ID+"=3", null, null, null, ORDER_BY);
			Log.d("EQ", "Inside getSettings() - case 2" );
			startManagingCursor(cursor);
		    Log.d("EQ", "cursor is being managed" );
			cursor.moveToFirst();
			if(cursor.getInt(0) == 1){
				EqSettings.eqPanel5 = new int[6];
				for(int i=0; i<=5;i++){
					EqSettings.eqPanel5[i]=cursor.getInt(i+1);
					Log.d("EQ", "eqPanel5["+i+"] = "+cursor.getInt(i+1));
				}
			}
			cursor.close();
			break;
			
		  case 4:
			cursor = db.query(TABLE_STATE, FROM, _ID+"=4", null, null, null, ORDER_BY);
			Log.d("EQ", "Inside getSettings() - case 3" );
			startManagingCursor(cursor);
		    Log.d("EQ", "cursor is being managed" );
			cursor.moveToFirst();
			if(cursor.getInt(0) == 1){
				EqSettings.eqPanel10 = new int[11];
				for(int i=0; i<=10;i++){
					EqSettings.eqPanel10[i]=cursor.getInt(i+1);
					Log.d("EQ", "eqPanel10["+i+"] = "+cursor.getInt(i+1));
				}
			}
			cursor.close();
			break;
	    }
	 }
/*================================================
* 					/\
* 				   /  \
* 				  /    \
*				 /      \
*				/_	    _\
*				  |    |
*
*			END OF SQL METHODS
*
*=================================================*/
	@Override
	public void onClick(View v) {
		int eq = 0;
		switch (v.getId()) {
		case R.id.ButtonPlay:
			if (EqSettings.eq_status) {
				switch(EqSettings.nBands)
				{
				case 10:
					++eq;
				case 7:
					++eq;
				case 5:
					++eq;
				case 3:
					equalizer.setID(eq);
					break;
				}
			} else {
				equalizer.enable = false;
			}

			if (mDecoder == null) {
				PlayNewSong();
			} else if (mDecoder.isPlaying()) {
				if (mDecoder.isPaused()) {
					mDecoder.Start();
				} else {
					mDecoder.Pause();
				}
			} else {
				mDecoder.Start();
			}
			//=============================================
			//// Code use to test the VU Visualizer
			////plus += 10;
			//buffer = fillBuffer(buffer, plus);
			//myVU.processBuffer(buffer);
			//myVU.postInvalidate();
			//=============================================
			break;
		case R.id.ButtonStop:
			mDecoder.Stop();
			break;
		case R.id.ButtonVU:
			if(myVU.isVisualizationOn() && vuCount < 1){
				myVU.stopVisualization();
				myVU.changeBarType();
				vuCount++;
				myVU.startVisualization();
			}
			else if(myVU.isVisualizationOn() && vuCount >= 1){
				myVU.stopVisualization();
				vuCount = 0;
			}else{
				myVU.startVisualization();
			}
			if(!visThread.isAlive()){
				Log.d("Main", "Thread not alive");
				visThread.start();
			}
			break;
		case R.id.ButtonVisualizer:
			displayVisualizaton(visCount);
			Log.d("Main", "Visualizator shown = "+visCount);
			visCount++;
			if(visCount >= numOfVis){
				visCount = 0;
			}
			break;
		case R.id.ButtonEqSettings:
			Intent i = new Intent(this, EqSettings.class);
			startActivity(i);
			break;
		case R.id.ButtonNext:
			PlayNextSong();
			break;
		}
	}
	
	public void displayVisualizaton(int vis){
		FrameLayout visFrameLayout = (FrameLayout)findViewById(R.id.Frame_Visualizer);
		visFrameLayout.removeAllViews();
		visFrameLayout.addView(inflateVisualization(vis));
		visThread.setVisVisualizer(visualizer);
	}
	
	public View inflateVisualization(int vis){
		View visualizationView = null;
		if(vis == 0){
			visualizationView = View.inflate(Main.this, R.layout.vi_10band_layout, null);
			visualizer = (SaskenVisualizer)visualizationView.findViewById(R.id.Band10_Visualizer);
		}
		else if (vis == 1){
			visualizationView = View.inflate(Main.this, R.layout.vi_20band_layout, null);
			visualizer = (SaskenVisualizer)visualizationView.findViewById(R.id.Band20_Visualizer);
		}
		else if (vis == 2){
			visualizationView = View.inflate(Main.this, R.layout.vi_10bandleds_layout, null);
			visualizer = (SaskenVisualizer)visualizationView.findViewById(R.id.BandLed10_Visualizer);
		}
		else if (vis == 3){
			visualizationView = View.inflate(Main.this, R.layout.empty_visualizer_layout, null);
			visualizer = (SaskenVisualizer)visualizationView.findViewById(R.id.Empty_Visualizer);
		}
		//if(mPlayer != null)
			//mPlayer.setVisualizer(visualizer);
		
		return visualizationView;
	}

/* ============================================= */
// Code use to test the VU Visualizer

	private byte[] fillBuffer(byte[] buffer, byte plus){
		for (int i = 0; i < bufferSize; i++){
			buffer[i] = plus;
		}
		return buffer;
	}

	@Override
	public void onPlaybackCompleted(SaskenDecoder sd) {
		Log.i("Main", "Playback Completed");
		PlayNextSong();
	}
}