package com.example.sockettest;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.view.Window;

public class SteerActivity extends Activity{

	PowerManager powerManager = null;
	WakeLock wakeLock = null;
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);  //È¥µô×î¶¥²ãµÄlabel
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_steer);
		
		this.powerManager = (PowerManager)this.getSystemService(Context.POWER_SERVICE);
		this.wakeLock = this.powerManager.newWakeLock(PowerManager.FULL_WAKE_LOCK, "My Lock");
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		this.wakeLock.acquire();
		super.onResume();
	}

	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		this.wakeLock.release();
		super.onPause();
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}

}
