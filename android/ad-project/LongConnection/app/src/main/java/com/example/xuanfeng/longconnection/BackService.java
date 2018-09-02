package com.example.xuanfeng.longconnection;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.ref.WeakReference;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.Arrays;

public class BackService extends Service {
    private String TAG= "BackService";
    private final static String HEART_BEAT_RSP_ACTION = "com.example.xuanfeng.longconnection.heart.rep.action";

    private final static String MESSAGE_RSP_ACTION="com.example.xuanfeng.longconnection.message.rep.action";
    private final static String HOST = "192.168.0.1";
    private final static int PORT=8080;

    private Socket socket;

    private WeakReference<Socket> weakReference;
    private ReadMsgThread readMsgTask;


    Handler handler = new Handler();
    class  Mybind extends  InBackServiceAIDL.Stub {


        @Override
        public boolean sendMsg(String str) throws RemoteException {
            return sendMessage(str);
        }
    }



    public BackService() {
    }

    @Override
    public IBinder onBind(Intent intent) {

//        return (IBinder)iBackService;
        return null;


    }

    @Override
    public void onCreate() {
        super.onCreate();
        new InitSocketThread().start();
    }

    private class InitSocketThread extends Thread {
        @Override
        public void run() {
            super.run();
            initSocket();

        }
    }

    private Runnable heartRunnable = new Runnable() {
        @Override
        public void run() {
            if(null != weakReference )
            {
                Socket socket = weakReference.get();
                if(null != socket && socket.isConnected()) {
                    try {
                        if(sendMessage("")) {
                            handler.postDelayed(heartRunnable, 3000);
                        } else {
                            handler.removeCallbacks(heartRunnable);
                            realseSocket();
                            new InitSocketThread().start();
                        }
                    } catch (Exception e) {
                        Log.e(TAG, e.getMessage());
                    }

                }
            }
        }
    };

    private void realseSocket() {
        if(null != weakReference){
            Socket socket = weakReference.get();
            if(null != socket && socket.isClosed()) {
                try {
                    socket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            socket = null;
            weakReference = null;
        }
    }

    private boolean sendMessage(String str) {
        Socket socket = weakReference.get();
        if(null != socket && socket.isConnected()) {
            try {
                OutputStream os = socket.getOutputStream();

                os.write(str.getBytes());
                os.flush();
               return true;
            } catch (Exception e) {

                Log.e(TAG, e.getMessage());

            }

        }
        return false;
    }

    private void initSocket() {
        try {
            socket = new Socket(HOST, PORT);
            weakReference = new WeakReference<Socket>(socket);
            readMsgTask = new ReadMsgThread();
            readMsgTask.start();

            handler.postDelayed(heartRunnable, 3000);

        } catch (IOException e) {
            e.printStackTrace();
        }

    }


    private class ReadMsgThread extends Thread {
        @Override
        public void run() {
            super.run();
            if(null != weakReference) {
                Socket socket= weakReference.get();
                if(null != socket && socket.isConnected()) {

                    try {
                        InputStream is = socket.getInputStream();
                        byte[] bytes = new byte[1024];
                        int len  = 0;

                        while((len = is.read(bytes)) != -1) {
                            if(len > 0 ) {
                                String msg = new String(Arrays.copyOf(bytes, len));
                                if("ok".equals(msg)) {
                                    Intent intent = new Intent(HEART_BEAT_RSP_ACTION);
                                    sendBroadcast(intent);
                                    // 收到心跳消息，应该取消消息超时.

                                } else {
                                    Intent intent = new Intent(MESSAGE_RSP_ACTION);
                                    sendBroadcast(intent);
                                }


                            }
                        }
                    } catch (Exception e) {
                        Log.e(TAG, e.getMessage());
                    }


                }
            }
        }
    }


}
