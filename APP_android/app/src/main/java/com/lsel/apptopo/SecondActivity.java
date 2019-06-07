package com.lsel.apptopo;

import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.MediaPlayer;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
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
    private static final int SHAKE_WAIT_TIME_MS = 100;
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
    private TextView mAccx;
    private TextView mAccy;
    private TextView mAccz;
    private int estado = 2;
    private float gX = 0;
    private float gXlast = 0;
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
        // Get the sensors to use
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        mSensorAcc = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mSensorGyr = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

        // Instanciate the sound to use
        //soundAcc = MediaPlayer.create(this, R.raw.acc);
        soundGyro = MediaPlayer.create(this, R.raw.acc);

        mAccx = (TextView) findViewById(R.id.accele_x);
        mAccy = (TextView) findViewById(R.id.accele_y);
        mAccz = (TextView) findViewById(R.id.accele_z);

        btnStop = (Button) findViewById(R.id.stop);
        btnStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mqttHelper.publishToTopic("android","3 0 0");
               //estado = 3;
            }
        });
        btnPlay = (Button) findViewById(R.id.secondPlay);
        btnPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mqttHelper.publishToTopic("android","2 0 0");
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
                mAccx.setText(R.string.act_main_no_acuracy);
                mAccy.setText(R.string.act_main_no_acuracy);
                mAccz.setText(R.string.act_main_no_acuracy);
            }
            return;
        }

        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            mAccx.setText("x = " + Float.toString(event.values[0]));
            mAccy.setText("y = " + Float.toString(event.values[1]));
            mAccz.setText("z = " + Float.toString(event.values[2]));
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

        if ((now - mShakeTime) > SHAKE_WAIT_TIME_MS) {
            mShakeTime = now;
            if (event.values[0] > 0) {
                if ((event.values[0] > 3.5) && ((event.values[0] < 6))) {
                    if ((gX + 3) <= 50) {
                        gX = gX + 3;
                    } else gX = 50;
                } else {
                    if ((event.values[0] >= 6)) {
                        if ((gX + 7) <= 50) {
                            gX = gX + 7;
                        } else gX = 50;
                    }
                }
            }
            else{
                    if ((event.values[0] < -3.5) && ((event.values[0] > -6))) {
                        if ((gX - 3) >= -50) {
                            gX = gX - 3;
                        } else gX = -50;
                    } else {
                        if ((event.values[0] <= -6)) {
                            if ((gX - 7) >= -50) {
                                gX = gX - 7;
                            } else gX = -50;
                        }
                    }
                }

            if (gX != gXlast){
                msg = "" + estado + " " + gX + " " + 0;
                mqttHelper.publishToTopic("android",msg);
                estado = 0;
            }
            gXlast = gX;
            // / SensorManager.GRAVITY_EARTH;
            //float gY = event.values[1] - 5.0;/// SensorManager.GRAVITY_EARTH;
            //float gZ = event.values[2] - 5.0;/// SensorManager.GRAVITY_EARTH;

            // gForce will be close to 1 when there is no movement
            //double gForce = Math.sqrt(gX * gX + gY * gY + gZ * gZ);

            // Change background color if gForce exceeds threshold;
            // otherwise, reset the color
           // if (gForce > SHAKE_THRESHOLD) {

                //soundAcc.start();
            //}
        }
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
            String msg;
            // Change background color if rate of rotation around any
            // axis and in any direction exceeds threshold;
            // otherwise, reset the color
            if (Math.abs(event.values[0]) > ROTATION_THRESHOLD) {
                estado = 1;
                msg = "" + estado + " " + gX + " " + 0;
                mqttHelper.publishToTopic("android",msg);
                estado = 0;
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
