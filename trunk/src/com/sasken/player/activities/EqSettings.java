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
import static com.sasken.player.databases.TableConstants.TABLE_10BANDS_PRESETS;
import static com.sasken.player.databases.TableConstants.TABLE_3BANDS_PRESETS;
import static com.sasken.player.databases.TableConstants.TABLE_5BANDS_PRESETS;
import android.app.Activity;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.sasken.player.R;
import com.sasken.player.databases.SettingsDB;
import com.sasken.player.equalizers.SaskenEqualizer;
import com.sasken.player.widget.VerticalSeekBar;
import com.sasken.player.widget.VerticalSeekBar.OnSeekBarChangeListener;

public class EqSettings extends Activity {

	public static boolean eq_status = false;
	public static int nBands = 0;
	public static int[] eqPanel3 = null;
	public static int[] eqPanel5 = null;
	public static int[] eqPanel10 = null;

	public VerticalSeekBar currentPanel[] = null;

	private static String[] PRESET_3B_QRY = {PREAMP,BAND1,BAND2,BAND3};
	private static String[] PRESET_5B_QRY = {PREAMP,BAND1,BAND2,BAND3,BAND4,BAND5};
	private static String[] PRESET_10B_QRY = {PREAMP,BAND1,BAND2,BAND3,BAND4,BAND5,BAND6,BAND7,BAND8,BAND9,BAND10};
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.eqsettings);
        
        // RADIO Buttons
        final RadioButton radio_3bands = (RadioButton) findViewById(R.id.radio_3bands);
        final RadioButton radio_5bands = (RadioButton) findViewById(R.id.radio_5bands);
        final RadioButton radio_10bands = (RadioButton) findViewById(R.id.radio_10bands);
        // Set listeners to radio buttons
        radio_3bands.setOnClickListener(radio_listener);
        radio_5bands.setOnClickListener(radio_listener);
        radio_10bands.setOnClickListener(radio_listener);
        
        // Not clickable by default
        radio_3bands.setClickable(false);
        radio_5bands.setClickable(false);
        radio_10bands.setClickable(false);
        
        // ON/OFF Button
        final ToggleButton enabler_button = (ToggleButton) findViewById(R.id.enabler_button);
        enabler_button.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                // Perform action on clicks
                if (enabler_button.isChecked()) {
                    eq_status = true; 
                    if (nBands != 0){
                    	displayBands();
                    	currentPanel = resumeBands();
                    }else if(nBands == 0){
                    	nBands = 3; // 3 bands is the default when EQ is enabled by the very first time.
                    	displayBands();
                    	currentPanel = setBands();
                    	saveBandProgress();
                    }
                    radio_3bands.setClickable(true);
                    radio_5bands.setClickable(true);
                    radio_10bands.setClickable(true);

                    Toast.makeText(EqSettings.this, "Equalizer is ON", Toast.LENGTH_SHORT).show();
                                        
                }else{
                    Toast.makeText(EqSettings.this, "Equalizer is OFF", Toast.LENGTH_SHORT).show();
                    eq_status = false;
                    
                    //Clean Bands Layout
                    RelativeLayout rLayout = (RelativeLayout)findViewById(R.id.bands_display_layout);
                	rLayout.removeAllViews();
                	
                    //Disable radio buttons
                    if (radio_3bands.isChecked()){
                    	radio_3bands.setChecked(false);
                    }else if (radio_5bands.isChecked()){
                    	radio_5bands.setChecked(false);
                    }else if (radio_10bands.isChecked()){
                    	radio_10bands.setChecked(false);
                    }
                    radio_3bands.setClickable(false);
                    radio_5bands.setClickable(false);
                    radio_10bands.setClickable(false);
                }
            }
        });
        if (eq_status) {
        	enabler_button.setChecked(true);
            if (nBands != 0){
            	displayBands();
            	currentPanel = resumeBands();
                radio_3bands.setClickable(true);
                radio_5bands.setClickable(true);
                radio_10bands.setClickable(true);
            }
        }
        
        // RESET Button
        final Button reset_button = (Button)findViewById(R.id.reset_button);
        reset_button.setOnClickListener(button_listener);
        
        // DONE Button
        final Button back_button = (Button)findViewById(R.id.back_button);
        back_button.setOnClickListener(button_listener);
        
        // PRESETS Spinner
	    Spinner spinner = (Spinner) findViewById(R.id.spinner);
	    ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
	            this, R.array.PresetsArray, android.R.layout.simple_spinner_item);
	    adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
	    spinner.setAdapter(adapter);
        
	    spinner.setOnItemSelectedListener(new MyOnItemSelectedListener());
	}

	@Override
	protected void onPause() {
		super.onPause();
		Main.settings = new SettingsDB(this);
		try{
			if(Main.emptyTable() == true){
				Log.d("EQ", "Table is empty." );
				Main.addInitialRows();
			}
			else{
				Main.updateRows();
				Log.d("EQ", "Updated Rows." );
			}
        } finally {
        	Main.settings.close(); 
        	Log.d("EQ", "DB closed" );
        }
		
		Log.d("EQ", "onPause" );
	}

public VerticalSeekBar[] setBands(){
	VerticalSeekBar bands[] = setViewBands();
	bands = setDefaults(bands);
	bands = setListeners(bands);
	return bands;
}
	
public VerticalSeekBar[] setViewBands(){
        
	if (nBands == 3){
		VerticalSeekBar bands[] = {(VerticalSeekBar)findViewById(R.id.preamp_seekbar),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band01),
              						(VerticalSeekBar)findViewById(R.id.SeekBar_band02),
              						(VerticalSeekBar)findViewById(R.id.SeekBar_band03)};
		return bands;
	}
  	  
	if (nBands == 5){
		VerticalSeekBar bands[] = {(VerticalSeekBar)findViewById(R.id.preamp_seekbar),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band01),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band02),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band03),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band04),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band05)};
		return bands;
	}
  	  
	if (nBands == 10){
		VerticalSeekBar bands[] = {(VerticalSeekBar)findViewById(R.id.preamp_seekbar),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band01),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band02),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band03),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band04),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band05),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band06),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band07),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band08),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band09),
									(VerticalSeekBar)findViewById(R.id.SeekBar_band10)};
		return bands;
	}
	return null;
}	

public VerticalSeekBar[] setDefaults(VerticalSeekBar bands[]){

	for (int i=0; i<=nBands; i++){
		bands[i].setMax(100);
		bands[i].setProgress(50);
		
	}
	return bands;
}

public VerticalSeekBar[] setListeners(VerticalSeekBar bands[]){
	
	for(int i=0; i<=nBands; i++){
		bands[i].setOnSeekBarChangeListener(seekbar_listener);
		Log.d("EQ", "Listener setted on position :"+i);
	}
	return bands;
}

public int[] saveBands(VerticalSeekBar bands[]){

	int[] savedProgress = new int[bands.length];
	for(int i=0; i<=nBands ;i++){
		savedProgress[i]=bands[i].getProgress();
	}
	return savedProgress;
}

public VerticalSeekBar[] loadBands(int savedProgress[]){
	VerticalSeekBar bands[] = new VerticalSeekBar[savedProgress.length];
	bands = setViewBands();
	for(int i=0; i<savedProgress.length;i++){
		bands[i].setProgress(savedProgress[i]);
		savedProgress[i]=bands[i].getProgress();
	}
	bands = setListeners(bands);
	return bands;
}

public void displayBands(){
	RelativeLayout rLayout = prepareBandsLayout();
	rLayout.addView(inflateBands());
}

public RelativeLayout prepareBandsLayout(){
	//Prepare Relative Layout
	RelativeLayout rLayout = (RelativeLayout)findViewById(R.id.bands_display_layout);
	rLayout.removeAllViews();
	return rLayout;
}

public View inflateBands(){
	View vBands;
	switch (nBands){
		
	case 3:
		vBands = View.inflate(EqSettings.this,R.layout.three_bands_panel,null);
		return vBands;
	case 5:
		vBands = View.inflate(EqSettings.this,R.layout.five_bands_panel,null);
		return vBands;
	case 10:
		vBands = View.inflate(EqSettings.this,R.layout.ten_bands_panel,null);
		return vBands;
	}
	return null;
};

public void saveBandProgress(){
	switch (nBands){
	case 3:
		//Log.d("EQ", "nBands == 3 , save to eqPanel3");
		eqPanel3 = saveBands(currentPanel);
		break;
	case 5:
		//Log.d("EQ", "nBands == 5 , save to eqPanel5");
		eqPanel5 = saveBands(currentPanel);
		break;
	case 10:
		//Log.d("EQ", "nBands == 10 , save to eqPanel10");
		eqPanel10 = saveBands(currentPanel);
		break;
	}
}

public VerticalSeekBar[] resumeBands(){
	VerticalSeekBar[] bands = new VerticalSeekBar[nBands];
	if(nBands == 3){
		bands = loadBands(eqPanel3);
	}
	if(nBands == 5){
		bands = loadBands(eqPanel5);
	}
	if(nBands == 10){
		bands = loadBands(eqPanel10);
	}
	
	return bands;
}

private void getPreset(int pos,int bands) {
    // Perform a managed query. The Activity will handle closing
    // and re-querying the cursor when needed.
    SQLiteDatabase db = Main.settings.getReadableDatabase();
    Cursor cursor = null;
    
    Log.d("EQ", "Inside getPreset()" );
    switch (bands){
    
	  case 3:
		  cursor = db.query(TABLE_3BANDS_PRESETS, PRESET_3B_QRY, _ID+"="+pos, null, null, null, null);
		  startManagingCursor(cursor);
		  cursor.moveToFirst();
		  for(int i=0; i<=nBands;i++){
			  currentPanel[i].setProgress(cursor.getInt(i));
		  }
		  cursor.close();
		  break;

	  case 5:
		  cursor = db.query(TABLE_5BANDS_PRESETS, PRESET_5B_QRY, _ID+"="+pos, null, null, null, null);
		  startManagingCursor(cursor);
		  cursor.moveToFirst();
		  for(int i=0; i<=nBands;i++){
			  currentPanel[i].setProgress(cursor.getInt(i));
		  }
		  cursor.close();
		  break;
		
	  case 10:
		  cursor = db.query(TABLE_10BANDS_PRESETS, PRESET_10B_QRY, _ID+"="+pos, null, null, null, null);
		  startManagingCursor(cursor);
		  cursor.moveToFirst();
		  for(int i=0; i<=nBands;i++){
			  currentPanel[i].setProgress(cursor.getInt(i));
		  }
		  cursor.close();
		  break;
    }
    Main.settings.close(); 
 }

/** Convert a string into an array */
static protected int[] fromBandsString(String string) {
   int[] eqPanel = new int[string.length()];
   for (int i = 0; i < eqPanel.length; i++) {
      eqPanel[i] = string.charAt(i) - '0';
   }
   return eqPanel;
}

//Listeners
private final OnClickListener button_listener = new OnClickListener(){

	@Override
	public void onClick(View v) {
		Button button = (Button) v;
        switch (button.getId()){
        	
        case R.id.reset_button:
        		currentPanel = setBands();
        		Toast.makeText(EqSettings.this, "Default Values", Toast.LENGTH_SHORT).show();
        	break;
        
        case R.id.back_button:
            finish();
            break;        	
        }
		
	}
};

private final OnClickListener radio_listener = new OnClickListener() {
       
		public void onClick(View v) {
            	
				//Perform action on clicks
                RadioButton rb = (RadioButton) v;
                switch (rb.getId()){
                	
                case R.id.radio_3bands:
                	Toast.makeText(EqSettings.this, "Switch to 3 Bands", Toast.LENGTH_SHORT).show();
                	
                	nBands = 3;
                    
                	displayBands();
                    if (eqPanel3 == null){
            			Log.d("EQ", "eqPanel3 == null");
                    	currentPanel = setBands();
                    	//Log.d("EQ", "set "+nBands+" Bands");
                    	saveBandProgress();
                    	break;
                    }else
                    	Log.d("EQ", "eqPanel3 not null");
                    	currentPanel = loadBands(eqPanel3);
                    break;
                	
                case R.id.radio_5bands:
                	Toast.makeText(EqSettings.this, "Switch to 5 Bands", Toast.LENGTH_SHORT).show();
                	
                	nBands = 5;
                	
                	displayBands();
                    if (eqPanel5 == null){
                    	Log.d("EQ", "eqPanel5 == null");
                    	currentPanel = setBands();
                    	//Log.d("EQ", "set "+nBands+" Bands");
                    	saveBandProgress();
                    	break;
                    }else
                    	Log.d("EQ", "eqPanel5 not null");
                    	currentPanel = loadBands(eqPanel5);
                    break;
                	
                case R.id.radio_10bands:
                	Toast.makeText(EqSettings.this, "Switch to 10 Bands", Toast.LENGTH_SHORT).show();
                	
                	nBands = 10;
                	
                	displayBands();
                    if (eqPanel10 == null){
                    	Log.d("EQ", "eqPanel10 == null");
                    	currentPanel = setBands();
                    	//Log.d("EQ", "set "+nBands+" Bands");
                    	saveBandProgress();
                    	break;
                    }else
                    	Log.d("EQ", "eqPanel10 not null");
                    	currentPanel = loadBands(eqPanel10);
                    break;
                    
                }
		}
	};

public class MyOnItemSelectedListener implements OnItemSelectedListener {

	public void onItemSelected(AdapterView<?> parent,
		View view, int pos, long id) {
		Log.d("EQ", "pos: "+pos);
		if(nBands != 0 && pos != 0){
			getPreset(pos,nBands);
	      	Toast.makeText(parent.getContext(), parent.getItemAtPosition(pos).toString()+" Preset", Toast.LENGTH_LONG).show();	
		}
		}
	public void onNothingSelected(AdapterView parent) {
		// Do nothing.
	}
}

private final OnSeekBarChangeListener seekbar_listener = new OnSeekBarChangeListener(){

@Override
	public void onStartTrackingTouch(VerticalSeekBar seekBar) {
		// TODO Auto-generated method stub
	}

	@Override
	public void onStopTrackingTouch(VerticalSeekBar seekBar) {
		// TODO Auto-generated method stub
	}

	@Override
	public void onProgressChanged(VerticalSeekBar seekBar, int progress, boolean fromUser) {
		// TODO Auto-generated method stub
		
		switch (seekBar.getId()){
		case R.id.preamp_seekbar:
			currentPanel[0].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[0].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band01:
			currentPanel[1].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[1].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band02:
			currentPanel[2].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[2].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band03:
			currentPanel[3].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[3].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band04:
			currentPanel[4].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[4].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band05:
			currentPanel[5].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[5].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band06:
			currentPanel[6].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[6].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band07:
			currentPanel[7].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[7].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band08:
			currentPanel[8].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[8].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band09:
			currentPanel[9].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[9].getProgress());
			saveBandProgress();
			break;
			
		case R.id.SeekBar_band10:
			currentPanel[10].setProgress(progress);
			//Log.d("EQ", "progress at: " + currentPanel[10].getProgress());
			saveBandProgress();
			break;
		}
		SaskenEqualizer.change = true;
	}
	};

	public static int[] getTable() {
		switch(nBands) {
		case 3:
			return eqPanel3;
		case 5:
			return eqPanel5;
		case 10:
			return eqPanel10;
		default:
			return null;
		}
	}
}