package com.sasken.player.equalizers;
import com.sasken.player.activities.EqSettings;
public class SaskenEqualizer {
	private static int id = 0;
	public static boolean change = false;
	public static boolean enable = false;

	public int[] getG() {
		return EqSettings.getTable();
	}

	public void setID(int nId) {
		id = nId;
		enable = true;
	}

	static {
		System.loadLibrary("equalizer");
	}
}
