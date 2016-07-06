package com.example.sockettest;


/*import java.io.DataInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;*/

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import android.R.integer;
import android.app.Activity;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class SocketActivity extends Activity {
    /** Called when the activity is first created. */
	private Button startButton = null;
	private EditText xPos = null;
	private EditText yPos = null;
	private EditText zPos = null;
	private EditText hostIp = null;
	String ip = null;
	private SensorManager sensorMgr;  
    Sensor sensor;  
    private float x, y, z;  
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	this.requestWindowFeature(Window.FEATURE_NO_TITLE);  //去掉最顶层的label
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        startButton = (Button)findViewById(R.id.startListener);
        startButton.setOnClickListener(new StartSocketListener());
        startButton.setText("     开始     ");
        startButton.getBackground().setAlpha(100);
 
        xPos = (EditText)findViewById(R.id.xPos);
        yPos = (EditText)findViewById(R.id.yPos);
        zPos = (EditText)findViewById(R.id.zPos);
        xPos.setVisibility(View.INVISIBLE);
        yPos.setVisibility(View.INVISIBLE);
        zPos.setVisibility(View.INVISIBLE);
        hostIp = (EditText)findViewById(R.id.editText_hostIp);
        hostIp.setText("192.168.1.100");
        
       sensorMgr = (SensorManager) getSystemService(SENSOR_SERVICE);
        sensor = sensorMgr.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        
        SensorEventListener lsn = new SensorEventListener() {  
            @SuppressWarnings("deprecation")
			public void onSensorChanged(SensorEvent e) {  
                x = e.values[SensorManager.DATA_X];     int ix = new Float(x).intValue();
                y = e.values[SensorManager.DATA_Y];     int iy = new Float(y).intValue();
                z = e.values[SensorManager.DATA_Z];  	int iz = new Float(z).intValue();
                xPos.setText(""+ix);
                yPos.setText(""+iy);
                zPos.setText(""+iz);
            }  
              
            public void onAccuracyChanged(Sensor s, int accuracy) {  
            }  
        };  
        sensorMgr.registerListener(lsn, sensor, SensorManager.SENSOR_DELAY_GAME); 
    }
    
    class StartSocketListener implements OnClickListener{

		@Override
		public void onClick(View v) {
			ip = hostIp.getText().toString();
			setTitle("connect to ip" + ip);
			System.out.println("startClientThread");
			new Thread(new ClientThread()).start();
			
			Intent intent = new Intent();
			intent.setClass(SocketActivity.this, SteerActivity.class);
			startActivity(intent);
		}
    	
    }
    
    class ClientThread implements Runnable {

        int ix, iy, iz;
        int keyUp;		int oldKeyUp;
        int keyDown;	int oldKeyDown;
        int keyLeft;	int oldKeyleft;
        int keyRight;	int oldKeyRight;
		@Override
		public void run() {
			// TODO Auto-generated method stub
			try {
				//首先创建一个DatagramSocket对象
				System.out.println("ready");
				DatagramSocket socket = new DatagramSocket();
				InetAddress addr = InetAddress.getByName(ip);
				int port = 1234;
				//创建一个InetAddree
				while(true) {
					String x = xPos.getText().toString(); ix = Integer.parseInt(x);
					String y = yPos.getText().toString(); iy = Integer.parseInt(y);
					String z = zPos.getText().toString(); iz = Integer.parseInt(z);
					sensor();
					String sendUp;
					String sendDown;
					String sendLeft;
					String sendRight;
					
					if((oldKeyUp!=keyUp) || (oldKeyDown!=keyDown) || (oldKeyleft!=keyLeft) || (oldKeyRight!=keyRight)) {
						if(keyUp == 0) {
							sendUp = "00";
						}
						else {
							sendUp = "" + keyUp;
						}
						if(keyDown == 0) {
							sendDown = "00";
						}
						else {
							sendDown = "" + keyDown;
						}
						if(keyLeft == 0) {
							sendLeft = "00";
						}
						else {
							sendLeft = "" + keyLeft;
						}
						if(keyRight == 0) {
							sendRight = "00";
						}
						else {
							sendRight = "" + keyRight;
						}
						String sendStr = sendUp + sendDown + sendLeft + sendRight +"\0";
						System.out.println(sendStr);
						byte[] sendBuf;
						sendBuf = sendStr.getBytes();
						DatagramPacket sendPacket = new DatagramPacket(sendBuf,
								sendBuf.length, addr, port);
						socket.send(sendPacket);
						oldKeyUp = keyUp;
						oldKeyDown = keyDown;
						oldKeyleft = keyLeft;
						oldKeyRight = keyRight;
					}
					//					socket.close();
					//创建一个DatagramPacket对象，并指定要讲这个数据包发送到网络当中的哪个地址，以及端口号
//					Thread.sleep(100);
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
//				socket.close();
				e.printStackTrace();
			}
		}
    	
		void sensor() {
					if(iy > 2) {
						keyRight = 'D';
						keyLeft = 0;
					}
					else if(iy < -2){
						keyLeft = 'A';
						keyRight = 0;
					}
					else {
						keyLeft = 0;
						keyRight = 0;
					}

					if(iz > 1) {
						keyUp = 'W';
						keyDown = 0;
					}
					else if(iz < 0){
						keyDown = 'S';
						keyUp = 0;
					}
					else {
						keyUp = 0;
						keyDown = 0;
					}
				}

    }
    
}
