package com.example.xuanfeng.myapplication;

import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.URLConnection;

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

        tv = findViewById(R.id.tvResult);

    }

    private void readUrl(String str) {
        new AsyncTask<String, Float, String>() {


            @Override
            protected String doInBackground(String... strings) {

                try {
                    URL url = new URL("http://www.baidu.com");
                    URLConnection urlConnection =  url.openConnection();

                    long total = urlConnection.getContentLength();

                    InputStream iStream = urlConnection.getInputStream();
                    InputStreamReader inRead = new InputStreamReader(iStream);

                    BufferedReader br = new BufferedReader(inRead);
                    String line;
                    StringBuffer builder = new StringBuffer();
                    while((line = br.readLine()) !=  "") {
                        builder.append(line);
                        publishProgress((float)builder.toString().length() / total);
                    }
                    br.close();;
                    iStream.close();
                    return builder.toString();

                } catch (Exception e) {

                }



                return null;
            }

            @Override
            protected void onPostExecute(String s) {
                System.out.print(s);
                tv.setText(s);
                super.onPostExecute(s);
            }

            @Override
            protected void onProgressUpdate(Float... values) {
                System.err.print(values[0]);
                super.onProgressUpdate(values);
            }
        }.execute(str);



    }

}
