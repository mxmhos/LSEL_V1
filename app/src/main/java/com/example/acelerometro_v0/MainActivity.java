package com.example.acelerometro_v0;

import android.support.annotation.NonNull;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.widget.EditText;
import android.widget.TextView;
import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.MqttPersistenceException;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.nio.charset.StandardCharsets;

public class MainActivity extends Activity implements SensorEventListener {
    private SensorManager mSensorManager; //Variable tipo SensorManager
    private Sensor mAccelerometer; //Variable tipo Sensor
    private TextView text1x;
    private TextView text1y;
    private TextView text2x;
    private TextView text2y;
    private TextView text3;
    private TextView text4;
    private String mTopic;
    private String mData;
    private String ipAdr = "192.168.1.202";
    private EditText edit;

    private int cnt=0;
    private int cont=0;
    private int boton=0;
    private int modXY=0;

    private int yLast=0, yCont=0, xNext=0, yNext=0, xAct=0, yAct=0;
    private int posX=0, posY=0;

    public MainActivity() {

     }


    public void mMando(){
        MemoryPersistence memPer = new MemoryPersistence();

        final MqttAndroidClient client = new MqttAndroidClient(
                getApplicationContext(), "tcp://"+ipAdr+":1883", MqttClient.generateClientId(), memPer);
        try {
            client.connect(null, new IMqttActionListener() {

                @Override
                public void onSuccess(IMqttToken mqttToken) {

                    MqttMessage message = new MqttMessage(mData.getBytes());
                    message.setQos(2);
                    message.setRetained(false);

                    try {
                        client.publish(mTopic, message);
                        client.disconnect();
                    } catch (MqttPersistenceException e) {e.printStackTrace();
                        } catch (MqttException e) { e.printStackTrace(); }
                }

                @Override
                public void onFailure(IMqttToken arg0, Throwable arg1) {
                    // TODO Auto-generated method stub
                }
            });
        } catch (MqttException e) { e.printStackTrace(); }
    }

    @Override
    protected void onResume() {
        super.onResume();
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mSensorManager.unregisterListener(this);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }//llamada cuando precision del sensor registrado cambia

    @Override
    public void onSensorChanged(final SensorEvent event) {
        xAct = Math.round(event.values[2]*10);
        yAct = Math.round(event.values[1]*10);
        text1x.setText(Integer.toString(xAct));
        text1y.setText(Integer.toString(yAct));

        if (xAct>70){ xAct=150; } else
        if (xAct>60){ xAct=100; } else
        if (xAct>50){ xAct=70; } else
        if (xAct>40){ xAct=45; } else
        if (xAct>30){ xAct=25; } else
        if (xAct>20){ xAct=13; } else
        if (xAct>10){ xAct=4; } else
        if (xAct<-70){ xAct=-150; }else
        if (xAct<-60){ xAct=-100; } else
        if (xAct<-50){ xAct=-70; }else
        if (xAct<-40){ xAct=-45; } else
        if (xAct<-30){ xAct=-25; }else
        if (xAct<-20){ xAct=-13; } else
        if (xAct<-10){ xAct=-4; }else
            xAct=0;

        if (yAct>90){ yAct=200; } else
        if (yAct>70){ yAct=150; } else
        if (yAct>60){ yAct=100; } else
        if (yAct>50){ yAct=70; } else
        if (yAct>40){ yAct=45; } else
        if (yAct>30){ yAct=25; } else
        if (yAct>20){ yAct=13; } else
        if (yAct>10){ yAct=4; } else
        if (yAct<-70){ yAct=-150; }else
        if (yAct<-60){ yAct=-100; } else
        if (yAct<-50){ yAct=-70; }else
        if (yAct<-40){ yAct=-45; } else
        if (yAct<-30){ yAct=-25; }else
        if (yAct<-20){ yAct=-13; } else
        if (yAct<-10){ yAct=-4; }else
            yAct=0;

        xAct=-xAct;
        if ((yLast<160) && (yCont<160) && (yAct<160)){
            xNext=posX+xAct;
            if (xNext<-500) { if (posX!=-500) {posX=-500; modXY=1; }} else
            if (xNext>500) { if (posX!=500) {posX=500; modXY=1;}} else
            if (xAct!=0) {posX=xNext; modXY=1;}

            yNext=posY+yCont;
            if (yNext<0) { if (posY!=0) {posY=0; modXY=1;}} else
            if (yNext>1000) { if (posY!=0) {posY=1000; modXY=1;}} else
            if (yCont!=0) {posY=yNext; modXY=1;}

            yCont=yLast;
            yLast=yAct;

            text2y.setText(Integer.toString(posY));
            text2x.setText(Integer.toString(posX));
        }
        else
            yCont=200;

        if ((yAct<0)&&(yCont>160)){
            text4.setText("MARTILLO!!!");
            yLast=0;
            yCont=0;
            cnt=0;
            boton=1;
        }

        if ((boton!=0)||(modXY==1)){
            mData=(""+boton+" "+posX/10+" "+posY/10);
            mTopic="android";
            boton=0;
            modXY=0;
            mMando();
        }

        if (cnt>5){
            text3.setText(Integer.toString(cont));
            text4.setText("");
            cont=cont+1;
            cnt=0;
        }
        else {
            cnt=cnt+1;
        }
    }//llamada cuando hay un nuevo evento del sensor.

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        text1x = (TextView) findViewById(R.id.texto1x);
        text1y = (TextView) findViewById(R.id.texto1y);
        text2x = (TextView) findViewById(R.id.texto2x);
        text2y = (TextView) findViewById(R.id.texto2y);
        text3 = (TextView) findViewById(R.id.texto3);
        text4 = (TextView) findViewById(R.id.texto4);
        edit = (EditText)findViewById(R.id.editText);


        final Button button1 = findViewById(R.id.button1);
        button1.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                ipAdr = edit.getText().toString();
                cnt=0;
                boton=2;
                mData=(""+boton+" "+posX+" "+posY);
                mTopic="android";
                boton=0;
                mMando();
                text4.setText("PLAY");
            }
        });

        final Button button2 = findViewById(R.id.button2);
        button2.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                cnt=0;
                boton=3;
                mData=(""+boton+" "+posX+" "+posY);
                mTopic="android";
                boton=0;
                mMando();
                text4.setText("STOP");
            }
        });

    }

}

