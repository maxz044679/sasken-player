package com.sasken.player.databases;

import android.provider.BaseColumns;

public interface TableConstants extends BaseColumns {
   
	//State table
	public static final String TABLE_STATE = "state";
	//Columns in the Settings table
	public static final String STATUS = "status";
	public static final String PREAMP = "preamp";
	public static final String BAND1 = "band1";
	public static final String BAND2 = "band2";
	public static final String BAND3 = "band3";
	public static final String BAND4 = "band4";
	public static final String BAND5 = "band5";
	public static final String BAND6 = "band6";
	public static final String BAND7 = "band7";
	public static final String BAND8 = "band8";
	public static final String BAND9 = "band9";
	public static final String BAND10 = "band10";
	
	//Presets tables
	public static final String TABLE_3BANDS_PRESETS = "three_bands_presets";
	public static final String TABLE_5BANDS_PRESETS = "five_bands_presets";
	public static final String TABLE_10BANDS_PRESETS = "ten_bands_presets";
	//Columns in the Settings table
	public static final String PRESET_NAME = "name";
}
