package com.example.androidex;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity{
	public native int driverOpen();
	public native int driverClose(int fd);
	public native void moveUp(int fd);
	public native void moveDown(int fd);
	public native void moveLeft(int fd);
	public native void moveRight(int fd);
	public native void checkEnding(int fd);
	
	private int fd;
	private boolean isStarted = false;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		System.loadLibrary("maze");
		
		// start button
		Button start = (Button)findViewById(R.id.start);
		OnClickListener start_click = new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(!isStarted) {
					fd = driverOpen();
					if(fd != -1) {
						isStarted = true;
						Toast myToast = Toast.makeText(getApplicationContext(), R.string.start, Toast.LENGTH_SHORT);
						myToast.show();
					}
					//checkThread check = new checkThread();
					//check.run(fd);
					Log.v("RETURN", String.valueOf(fd));
				}
				else {
					Toast myToast = Toast.makeText(getApplicationContext(), R.string.on, Toast.LENGTH_SHORT);
					myToast.show();
				}
			}
		};
		start.setOnClickListener(start_click);
		
		// end button
		Button end = (Button)findViewById(R.id.end);
		OnClickListener end_click = new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(isStarted) {
					driverClose(fd);
					isStarted = false;
					Toast myToast = Toast.makeText(getApplicationContext(), R.string.end, Toast.LENGTH_SHORT);
					myToast.show();
				}
				else {
					Toast myToast = Toast.makeText(getApplicationContext(), R.string.off, Toast.LENGTH_SHORT);
					myToast.show();
				}
			}
		};
		end.setOnClickListener(end_click);
		
		
		// up button
		Button up = (Button)findViewById(R.id.up);
		OnClickListener up_click = new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(isStarted) {
					moveUp(fd);
				}
				else {
					Toast myToast = Toast.makeText(getApplicationContext(), R.string.off, Toast.LENGTH_SHORT);
					myToast.show();
				}
			}
		};
		up.setOnClickListener(up_click);
		
		// down button
		Button down = (Button)findViewById(R.id.down);
		OnClickListener down_click = new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(isStarted) {
					moveDown(fd);
				}
				else {
					Toast myToast = Toast.makeText(getApplicationContext(), R.string.off, Toast.LENGTH_SHORT);
					myToast.show();
				}
			}
		};
		down.setOnClickListener(down_click);
		
		// left button
		Button left = (Button)findViewById(R.id.left);
		OnClickListener left_click = new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(isStarted) {
					moveLeft(fd);
				}
				else {
					Toast myToast = Toast.makeText(getApplicationContext(), R.string.off, Toast.LENGTH_SHORT);
					myToast.show();
				}
			}
		};
		left.setOnClickListener(left_click);
		
		// right button
		Button right = (Button)findViewById(R.id.right);
		OnClickListener right_click = new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(isStarted) {
					moveRight(fd);
				}
				else {
					Toast myToast = Toast.makeText(getApplicationContext(), R.string.off, Toast.LENGTH_SHORT);
					myToast.show();
				}
			}
		};
		right.setOnClickListener(right_click);
	}
	
	// check terminal conditionsssss
	private class checkThread extends Thread {
		public checkThread() {
			
		}
		public void run(int fd) {
			Log.v("Thread", "Start Thread");
			checkEnding(fd);
			if(isStarted){
				driverClose(fd);
				isStarted = false;
			}
			Toast myToast = Toast.makeText(getApplicationContext(), "Clear!", Toast.LENGTH_SHORT);
			myToast.show();
			Log.v("Thread", "End Thread");
		}
	}
}
