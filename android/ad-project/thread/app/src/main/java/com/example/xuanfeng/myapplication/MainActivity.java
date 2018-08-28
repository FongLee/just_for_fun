package com.example.xuanfeng.myapplication;

import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.net.URL;

public class MainActivity extends AppCompatActivity {

    TextView tv;
    Button btn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btn = findViewById(R.id.btnDownload);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                readUrl("http://www.baidu.com");
            }
        });
    }

    private void readUrl(String str) {
        new AsyncTask<String, Void, String>() {


            @Override
            protected String doInBackground(String... strings) {

                URL url = new URL(arg0[0]);

                
                return null;
            }
        }.execute(str);

    }

}
