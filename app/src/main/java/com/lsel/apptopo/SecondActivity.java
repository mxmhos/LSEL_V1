package com.lsel.apptopo;

import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.MediaPlayer;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import helpers.MqttHelper;


public class SecondActivity extends AppCompatActivity implements SensorEventListener {

    /**
     * Constants for sensors
     */
    private static final float SHAKE_THRESHOLD = 1.1f;
    private static final int SHAKE_WAIT_TIME_MS = 200;
    private static final float ROTATION_THRESHOLD = 2.5f;
    private static final int ROTATION_WAIT_TIME_MS = 300;

    /**
     * The sounds to play when a pattern is detected
     */
    private static MediaPlayer soundAcc;
    private static MediaPlayer soundGyro;

    /**
     * Sensores
    */
    private SensorManager mSensorManager;
    private Sensor mSensorAcc;
    private Sensor mSensorGyr;
    private long mShakeTime = 0;
    private long mRotationTime = 0;
    /**
     * UI
     */
    private TextView mAccX;
    private TextView mPosX;
    private TextView mSendX;
    private TextView mAccY;
    private TextView mPosY;
    private TextView mSendY;
    private TextView test_text;
    private int gX = 0;
    private int gXlast = 0;
    private int xAct = 0;
    private int gY = 0;
    private int gYlast = 0;
    private int yAct = 0;
    private int test_num = 0;
    /**
     * Conexion MQTT
     */
    private MqttHelper mqttHelper;
    Button btnStop, btnPlay;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_second);
        startMqtt();
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        // Get the sensors to use
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        mSensorAcc = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mSensorGyr = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

        // Instanciate the sound to use
        //soundAcc = MediaPlayer.create(this, R.raw.acc);
        soundGyro = MediaPlayer.create(this, R.raw.acc);

        mAccX = (TextView) findViewById(R.id.accele_x);
        mPosX = (TextView) findViewById(R.id.pocision_x);
        mSendX = (TextView) findViewById(R.id.envio_x);
        mAccY = (TextView) findViewById(R.id.accele_y);
        mPosY = (TextView) findViewById(R.id.pocision_y);
        mSendY = (TextView) findViewById(R.id.envio_y);
        test_text = (TextView) findViewById(R.id.test_view);

        btnStop = (Button) findViewById(R.id.stop);
        btnStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mqttHelper.publishToTopic("MANDO/stop","1");
               //estado = 3;
            }
        });
        btnPlay = (Button) findViewById(R.id.secondPlay);
        btnPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mqttHelper.publishToTopic("MANDO/start","1");
                //estado = 2;
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        mSensorManager.registerListener(this, mSensorAcc, SensorManager.SENSOR_DELAY_NORMAL);
        mSensorManager.registerListener(this, mSensorGyr, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mSensorManager.unregisterListener(this);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {

        if (event.accuracy == SensorManager.SENSOR_STATUS_UNRELIABLE) {

            if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                mAccX.setText(R.string.act_main_no_acuracy);
                mPosX.setText(R.string.act_main_no_acuracy);
                mSendX.setText(R.string.act_main_no_acuracy);
            }
            return;
        }

        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            detectShake(event);

        }else if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE){
            detectRotation(event);
        }

    }

    // References:
    //  - http://jasonmcreynolds.com/?p=388
    //  - http://code.tutsplus.com/tutorials/using-the-accelerometer-on-android--mobile-22125

    /**
     * Detect a shake based on the ACCELEROMETER sensor
     *
     * @param event
    */
    private void detectShake(SensorEvent event) {
        long now = System.currentTimeMillis();
        String msg;

        //if ((now - mShakeTime) > SHAKE_WAIT_TIME_MS) {
            mShakeTime = now;
            xAct=Math.round(event.values[0]*10);
            yAct=Math.round(event.values[1]*10);


 // Procedimiento de envio de info X

            // Tabla de sensibilidad
            if (xAct>70){ xAct=150; } else
            if (xAct>60){ xAct=90; } else
            if (xAct>50){ xAct=65; } else
            if (xAct>40){ xAct=40; } else
            if (xAct>30){ xAct=20; } else
            if (xAct>20){ xAct=10; } else
            if (xAct>10){ xAct=3; } else
            if (xAct<-70){ xAct=-150; }else
            if (xAct<-60){ xAct=-90; } else
            if (xAct<-50){ xAct=-65; }else
            if (xAct<-40){ xAct=-40; } else
            if (xAct<-30){ xAct=-20; }else
            if (xAct<-20){ xAct=-10; } else
            if (xAct<-10){ xAct=-3; }else
                xAct=0;

            //comparo e incremento
            mAccX.setText("incX = " + Integer.toString(xAct));
            if (gX+xAct<-500) { if (gX!=-500) {gX=-500;}} else
            if (gX+xAct>500) { if (gX!=500) {gX=500;}} else
            if (xAct!=0) {gX=gX+xAct;}
            mPosX.setText("posX = " + Integer.toString(gX));
            mSendX.setText("sendX = " + Integer.toString(gX/10));

            // envio si hubo cambio
            if (gX != gXlast){
                msg = ""+gX/10+"";
                mqttHelper.publishToTopic("MANDO/x",msg);
            }
            gXlast = gX;

 // Procedimiento de envio de info Y

            // Tabla de sensibilidad
            if (yAct>90){ yAct=100; } else
            if (yAct>80){ yAct=40; } else
            if (yAct>70){ yAct=18; } else
            if (yAct>60){ yAct=9; } else
            if (yAct>50){ yAct=3; } else
            if (yAct>40){ yAct=1; } else
            if (yAct>30){ yAct=0; } else
            if (yAct<-70){ yAct=-150; }else
            if (yAct<-60){ yAct=-110; } else
            if (yAct<-50){ yAct=-75; }else
            if (yAct<-40){ yAct=-40; } else
            if (yAct<-30){ yAct=-25; }else
            if (yAct<-20){ yAct=-13; } else
            if (yAct<-10){ yAct=-4; }else
                yAct=0;

            //comparo e incremento
            mAccY.setText("incY = " + Integer.toString(yAct));
            if (gY+yAct<0) { if (gY!=0) {gY=0;}} else
            if (gY+yAct>500) { if (gY!=500) {gY=500;}} else
            if (yAct!=0) {gY=gY+yAct;}
            mPosY.setText("posY = " + Integer.toString(gY));
            mSendY.setText("sendY = " + Integer.toString(gY/10));

            // envio si hubo cambio
            if (gY != gYlast){
                msg = ""+gY/10+"";
                mqttHelper.publishToTopic("MANDO/y",msg);
            }
            gYlast = gY;

            test_num=test_num+1;
            test_text.setText("num = " + Integer.toString(test_num));
            if (test_num>100) {test_num=0;}
        //}
    }

    /**
     * Detect a rotation in on the GYROSCOPE sensor
     *
     * @param event
     */
    private void detectRotation(SensorEvent event) {
        long now = System.currentTimeMillis();

        if ((now - mRotationTime) > ROTATION_WAIT_TIME_MS) {
            mRotationTime = now;
            // Change background color if rate of rotation around any
            // axis and in any direction exceeds threshold;
            // otherwise, reset the color
            if (Math.abs(event.values[0]) > ROTATION_THRESHOLD) {
                mqttHelper.publishToTopic("MANDO/disparo","1");
                soundGyro.start();
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    //MQTT
    /* public void startGame(View v){vmetodo 0 para implementar el boton pero no recomendable
        Toast.makeText(this,"boton presionado", Toast.LENGTH_LONG).show();
    }*/
    @Override
    protected void onStart(){
        super.onStart();
        //Aqui declaramos lo que queremos que realice la aplicacion al iniciarse
    }

    private void startMqtt(){
        mqttHelper = new MqttHelper(getApplicationContext());
        mqttHelper.mqttAndroidClient.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {
                Log.w("Debug","Connected");
            }

            @Override
            public void connectionLost(Throwable throwable) {

            }

            @Override
            public void messageArrived(String topic, MqttMessage mqttMessage) throws Exception {
                Log.w("Debug",mqttMessage.toString());
                //dataReceived.setText(mqttMessage.toString());
                //mChart.addEntry(Float.valueOf(mqttMessage.toString()));
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {

            }
        });
    }

}
