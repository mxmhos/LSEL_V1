package com.lsel.apptopo;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    public Button btn_play;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // Primera manera de implementar el click de un boton
        btn_play = (Button) findViewById(R.id.buttonPlay);
        btn_play.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this,SecondActivity.class);
                //intent.putExtra("play",2);
                //intent.putExtra("mqtt",(Serializable) mqttHelper);
                //bundle.putSerializable("mqtt", mqttHelper);
                //intent.putExtra("mqtt", mqttHelper);//para pasar parametros a la otra pantalla
                startActivity(intent);
                //Toast.makeText(MainActivity.this,"boton presionado", Toast.LENGTH_LONG).show();
            }
        });

    }
}
