package com.sasken.player.databases;

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
import static com.sasken.player.databases.TableConstants.PRESET_NAME;
import static com.sasken.player.databases.TableConstants.STATUS;
import static com.sasken.player.databases.TableConstants.TABLE_10BANDS_PRESETS;
import static com.sasken.player.databases.TableConstants.TABLE_3BANDS_PRESETS;
import static com.sasken.player.databases.TableConstants.TABLE_5BANDS_PRESETS;
import static com.sasken.player.databases.TableConstants.TABLE_STATE;
import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class SettingsDB extends SQLiteOpenHelper {
   private static final String DATABASE_NAME = "settings.db";
   private static final int DATABASE_VERSION = 1;

   /** Create a helper object for the Events database */
   public SettingsDB(Context ctx) { 
      super(ctx, DATABASE_NAME, null, DATABASE_VERSION);
   }

   @Override
   public void onCreate(SQLiteDatabase db) { 
	   //Create STATE table
	   db.execSQL("CREATE TABLE "+TABLE_STATE
    		+" ("+_ID+" INTEGER PRIMARY KEY AUTOINCREMENT, "
    		+STATUS+" REAL,"
    		+PREAMP+" INTEGER,"
    		+BAND1+" INTEGER,"
    		+BAND2+" INTEGER,"
    		+BAND3+" INTEGER,"
    		+BAND4+" INTEGER,"
    		+BAND5+" INTEGER,"
    		+BAND6+" INTEGER,"
    		+BAND7+" INTEGER,"
    		+BAND8+" INTEGER,"
    		+BAND9+" INTEGER,"
    		+BAND10+" INTEGER);");
	   
	   //Create Presets Tables.
	   db.execSQL("CREATE TABLE "+TABLE_3BANDS_PRESETS
	    		+" ("+_ID+" INTEGER PRIMARY KEY AUTOINCREMENT, "
	    		+PRESET_NAME+" TEXT NOT NULL,"
	    		+PREAMP+" INTEGER,"
	    		+BAND1+" INTEGER,"
	    		+BAND2+" INTEGER,"
	    		+BAND3+" INTEGER);");
	   
	   db.execSQL("INSERT INTO "+TABLE_3BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+") VALUES ('ROCK', 			50, 55, 45, 60);");
	   db.execSQL("INSERT INTO "+TABLE_3BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+") VALUES ('JAZZ', 			50, 42, 45, 57);");
	   db.execSQL("INSERT INTO "+TABLE_3BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+") VALUES ('FULL BASS', 	50, 75, 45, 50);");
	   db.execSQL("INSERT INTO "+TABLE_3BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+") VALUES ('FULL TREBLE',	50, 50, 45, 75);");
	   db.execSQL("INSERT INTO "+TABLE_3BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+") VALUES ('VOCAL', 		50, 43, 60, 43);");
	   
	   db.execSQL("CREATE TABLE "+TABLE_5BANDS_PRESETS
	    		+" ("+_ID+" INTEGER PRIMARY KEY AUTOINCREMENT, "
	    		+PRESET_NAME+" TEXT NOT NULL,"
	    		+PREAMP+" INTEGER,"
	    		+BAND1+" INTEGER,"
	    		+BAND2+" INTEGER,"
	    		+BAND3+" INTEGER,"
	    		+BAND4+" INTEGER,"
	    		+BAND5+" INTEGER);");

	   db.execSQL("INSERT INTO "+TABLE_5BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+")VALUES ('ROCK', 		50, 65, 62, 50, 60, 62);");
	   db.execSQL("INSERT INTO "+TABLE_5BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+")VALUES ('JAZZ', 		50, 40, 40, 50, 70, 70);");
	   db.execSQL("INSERT INTO "+TABLE_5BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+")VALUES ('FULL BASS', 	50, 75, 70, 55, 45, 40);");
	   db.execSQL("INSERT INTO "+TABLE_5BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+")VALUES ('FULL TREBLE',	50, 40, 40, 55, 65, 75);");
	   db.execSQL("INSERT INTO "+TABLE_5BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+")VALUES ('VOCAL', 		50, 40, 45, 60, 45, 40);");

	   db.execSQL("CREATE TABLE "+TABLE_10BANDS_PRESETS
	    		+" ("+_ID+" INTEGER PRIMARY KEY AUTOINCREMENT, "
	    		+PRESET_NAME+" TEXT NOT NULL,"
	    		+PREAMP+" INTEGER,"
	    		+BAND1+" INTEGER,"
	    		+BAND2+" INTEGER,"
	    		+BAND3+" INTEGER,"
	    		+BAND4+" INTEGER,"
	    		+BAND5+" INTEGER,"
	    		+BAND6+" INTEGER,"
	    		+BAND7+" INTEGER,"
	    		+BAND8+" INTEGER,"
	    		+BAND9+" INTEGER,"
	    		+BAND10+" INTEGER);");

	   db.execSQL("INSERT INTO "+TABLE_10BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+", "+BAND6+", "+BAND7+", "+BAND8+", "+BAND9+", "+BAND10+")VALUES ('ROCK', 		50, 70, 75, 70, 45, 40, 48, 55, 75, 80, 80);");
	   db.execSQL("INSERT INTO "+TABLE_10BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+", "+BAND6+", "+BAND7+", "+BAND8+", "+BAND9+", "+BAND10+")VALUES ('JAZZ', 		50, 50, 50, 50, 50, 50, 50, 35, 35, 35, 30);");
	   db.execSQL("INSERT INTO "+TABLE_10BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+", "+BAND6+", "+BAND7+", "+BAND8+", "+BAND9+", "+BAND10+")VALUES ('FULL BASS', 	50, 75, 75, 75, 65, 55, 45, 35, 25, 25, 25);");
	   db.execSQL("INSERT INTO "+TABLE_10BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+", "+BAND6+", "+BAND7+", "+BAND8+", "+BAND9+", "+BAND10+")VALUES ('FULL TREBLE',	50, 25, 25, 25, 45, 55, 75, 80, 80, 80, 90);");
	   db.execSQL("INSERT INTO "+TABLE_10BANDS_PRESETS+" ("+PRESET_NAME+", "+PREAMP+", "+BAND1+", "+BAND2+", "+BAND3+", "+BAND4+", "+BAND5+", "+BAND6+", "+BAND7+", "+BAND8+", "+BAND9+", "+BAND10+")VALUES ('VOCAL', 		50, 40, 50, 60, 63, 65, 67, 65, 60, 60, 60);");
   }

   @Override
   public void onUpgrade(SQLiteDatabase db, int oldVersion, 
         int newVersion) {
      db.execSQL("DROP TABLE IF EXISTS " + TABLE_STATE);
      db.execSQL("DROP TABLE IF EXISTS " + TABLE_3BANDS_PRESETS);
      db.execSQL("DROP TABLE IF EXISTS " + TABLE_5BANDS_PRESETS);
      db.execSQL("DROP TABLE IF EXISTS " + TABLE_10BANDS_PRESETS);
      onCreate(db);
   }
}