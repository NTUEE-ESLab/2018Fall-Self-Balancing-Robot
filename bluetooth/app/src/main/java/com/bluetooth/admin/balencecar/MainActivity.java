package com.bluetooth.admin.balencecar;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Bundle;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.MotionEvent;
import android.view.SoundEffectConstants;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;


public class MainActivity extends Activity {

    private BluetoothAdapter bluetoothAdapter;
    private Switch bluetoothSwitch;
    private Button bluetoothSearch;
    private Spinner bluetoothList;
    private Button bluetoothConnect;
    private ArrayAdapter<String> adapter;
    private List<String> list = new ArrayList<String>();
    private String strMacAddress;
    private static boolean booleanConnect = false;


    /*方向按钮定义*/
    private Button mButtonRun;
    private Button mButtonBack;
    private Button mButtonLeft;
    private Button mButtonRight;
    private Button mButtonStop;
    private Button mButtonLevo;
    private Button mButtonDextro;
    /*功能按钮*/

    //msg 定义
    private static final int msgShowConnect = 1;

    /**************service 命令*********/
    static final int CMD_STOP_SERVICE = 0x01;       // Main -> service
    static final int CMD_SEND_DATA = 0x02;          // Main -> service
    static final int CMD_SYSTEM_EXIT =0x03;         // service -> Main
    static final int CMD_SHOW_TOAST =0x04;          // service -> Main
    static final int CMD_CONNECT_BLUETOOTH = 0x05;  // Main -> service
    static final int CMD_RECEIVE_DATA = 0x06;       // service -> Main

    MyReceiver receiver;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        /*设置全屏*/
//        requestWindowFeature(Window.FEATURE_NO_TITLE);//隐藏标题
//        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);//设置全屏
//        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        setContentView(R.layout.activity_main);

        bluetoothSwitch = (Switch) findViewById(R.id.bluetoothswitch);
        bluetoothSearch = (Button) findViewById(R.id.buttonSearchBluetooth);
        bluetoothList = (Spinner) findViewById(R.id.list_bluetooth);
        bluetoothConnect = (Button) findViewById(R.id.buttonConnect);

        /*按钮监听按下弹起*/
        ButtonListener b = new ButtonListener();
        mButtonRun = (Button) findViewById(R.id.button_run);
        mButtonBack = (Button) findViewById(R.id.button_back);
        mButtonLeft = (Button) findViewById(R.id.button_left);
        mButtonRight = (Button) findViewById(R.id.button_right);
        mButtonStop = (Button) findViewById(R.id.button_stop);
        mButtonLevo = (Button) findViewById(R.id.button_levo);
        mButtonDextro = (Button) findViewById(R.id.button_dextro);


        //mButtonRun.setOnClickListener(b);
        mButtonRun.setOnTouchListener(b);
        mButtonBack.setOnTouchListener(b);
        mButtonLeft.setOnTouchListener(b);
        mButtonRight.setOnTouchListener(b);
        mButtonStop.setOnTouchListener(b);
        mButtonLevo.setOnTouchListener(b);
        mButtonDextro.setOnTouchListener(b);


        /*按钮监听按下弹起*/

        /*检查手机是否支持蓝牙*/
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            //表明此手机不支持蓝牙
            Toast.makeText(MainActivity.this, "Bluetooth Devices are not found", Toast.LENGTH_SHORT).show();
            return;
        }
        if (bluetoothAdapter.isEnabled()) {
            bluetoothSwitch.setChecked(true);
        }

        /*添加蓝牙列表*/
        adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, list);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        bluetoothList.setAdapter(adapter);
        bluetoothList.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                strMacAddress = adapter.getItem(i);
                adapterView.setVisibility(View.VISIBLE);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        /*蓝牙总开关*/
        bluetoothSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if (b) {
                    if (!bluetoothAdapter.isEnabled()) { //蓝牙未开启，则开启蓝牙
                        Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                        startActivity(enableIntent);
                    } else {
                        Toast.makeText(MainActivity.this, "Bluetooth is on", Toast.LENGTH_SHORT).show();
                    }
                } else {
                    bluetoothAdapter.disable();
                    Toast.makeText(MainActivity.this, "Bluetooth is off", Toast.LENGTH_SHORT).show();
                }
            }
        });


        /*蓝牙搜索*/
        bluetoothSearch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (bluetoothAdapter == null) {
                    Toast.makeText(MainActivity.this, "Bluetooth Devices are not found", Toast.LENGTH_SHORT).show();
                    return;
                } else if (!bluetoothAdapter.isEnabled()) {
                    Toast.makeText(MainActivity.this, "Bluetooth Devices are not opened", Toast.LENGTH_SHORT).show();
                }

                Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
                if (pairedDevices.size() > 0) {
                    for (BluetoothDevice device : pairedDevices) {
                        adapter.remove(device.getAddress());
                        adapter.add(device.getAddress());
                    }
                } else {
                    //注册，当一个设备被发现时调用mReceive
                    Log.i("seach", "hhhhhh");
                    IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
                    registerReceiver(mReceiver, filter);
                }
            }
        });

        /*蓝牙连接或断开*/
        bluetoothConnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (strMacAddress == null) {
                    Toast.makeText(MainActivity.this, "Please search devices first", Toast.LENGTH_SHORT).show();
                } else {
                    if (booleanConnect == false) {

                        Intent i = new Intent(MainActivity.this, MyService.class);
                        i.putExtra("Mac", strMacAddress);
                        startService(i);

                        //bluetoothConnect.setEnabled(false);
                    }
                    else // 断开蓝牙
                    {
                        booleanConnect = false;
                        //stopService(new Intent(MainActivity.this, MyService.class));
                        bluetoothConnect.setText("Connect");

                        Intent intent = new Intent();//创建Intent对象
                        intent.setAction("android.intent.action.cmd");
                        intent.putExtra("cmd", CMD_STOP_SERVICE);
                        sendBroadcast(intent);//发送广播连接蓝牙


                    }

                }
            }
        });

    }


    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.e("fond:", "mReceiver");

            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // 已经配对的则跳过
                if (device.getBondState() != BluetoothDevice.BOND_BONDED) {
                    adapter.add(device.getAddress());
                }

            } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {  //搜索结束
                Log.e("fond:", "ACTION_DISCOVERY_FINISHED");
                if (adapter.getCount() == 0) {
                    Toast.makeText(MainActivity.this, "Devices are not found", Toast.LENGTH_SHORT).show();
                }
            }

        }
    };
    /*********************************************************************************************/
    @Override
    protected void onStart() {
        super.onStart();
        Log.i("OnStart", "Start");
    }
    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
        Log.i("onDestroy", "Destroy");
        if(receiver!=null){
            MainActivity.this.unregisterReceiver(receiver);
        }
    }
    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
        Log.i("onResume", "Resume");
        receiver = new MyReceiver();
        IntentFilter filter=new IntentFilter();
        filter.addAction("android.intent.action.bluetooth.admin.bluetooth");
        MainActivity.this.registerReceiver(receiver,filter);
    }

    public void showToast(String str){//显示提示信息
        Toast.makeText(getApplicationContext(), str, Toast.LENGTH_SHORT).show();
    }
    public class MyReceiver extends BroadcastReceiver{
        @Override
        public void onReceive(Context context, Intent intent) {
            // TODO Auto-generated method stub
            if(intent.getAction().equals("android.intent.action.bluetooth.admin.bluetooth")){
                Bundle bundle = intent.getExtras();
                int cmd = bundle.getInt("cmd");

                if(cmd == CMD_SHOW_TOAST){
                    String str = bundle.getString("str");
                    showToast(str);
                    if ("Connected! You can start controlling it!".equals(str))
                    {
                        booleanConnect = true;
                        bluetoothConnect.setEnabled(true);
                        bluetoothConnect.setText("Disconnect");
                    }
                    else if("Connect failed".equals(str))
                    {
                        booleanConnect = false;
                        bluetoothConnect.setEnabled(true);
                        bluetoothConnect.setText("Connect");
                    }
                }
                else if(cmd == CMD_SYSTEM_EXIT){
                    System.exit(0);
                }
                else if(cmd == CMD_RECEIVE_DATA)  //此处是可以接收蓝牙发送过来的数据可以解析，此例程暂时不解析返回来的数据，需要解析的在我们的全功能版会有
                {
//                    String strtemp = bundle.getString("str");
//                    int start = strtemp.indexOf("$");
//                    int end = strtemp.indexOf("#");
//
//                    if (start >= 0 && end > 0 && end > start && strtemp.length() > 23 )
//                    {
//                        String str = strtemp.substring(23);
//                        String strCSB = str.substring(0, str.indexOf(","));
//                        String strVolume = str.substring(str.indexOf(",")+1, str.indexOf("#"));
//                        tvCSB.setText(strCSB);
//                        tvVolume.setText(strVolume);
//                    }
                }

            }
        }
    }

    public void SendBlueToothProtocol(String value){
        Intent intent = new Intent();//创建Intent对象
        intent.setAction("android.intent.action.cmd");
        intent.putExtra("cmd", CMD_SEND_DATA);
        intent.putExtra("command", (byte)0x00);
        intent.putExtra("value", value);
        sendBroadcast(intent);//发送广播
    }
    /*********************************************************************************************/


    class ButtonListener implements View.OnClickListener, View.OnTouchListener {

        public void onClick(View v) {
            if (v.getId() == R.id.button_run) {
                //Log.d("test", "cansal button ---> click");
            }
        }

        public boolean onTouch(View v, MotionEvent event)
        {
            switch (v.getId()) {
                case R.id.button_run: {
                    if (event.getAction() == MotionEvent.ACTION_UP) {

                        mButtonRun.setBackgroundColor(Color.parseColor("#C0C0C0"));
                        SendBlueToothProtocol("$0,0,0,0,0,0,0,0,0,0#");
                    }
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        SendBlueToothProtocol("$1,0,0,0,0,0,0,0,0,0#");
                        v.performHapticFeedback(HapticFeedbackConstants.LONG_PRESS,HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING);
                        v.playSoundEffect(SoundEffectConstants.CLICK);
                        mButtonRun.setBackgroundColor(Color.parseColor("#CC0000"));

                    }
                }break;

                case R.id.button_back: {
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        mButtonBack.setBackgroundColor(Color.parseColor("#C0C0C0"));
                        SendBlueToothProtocol("$0,0,0,0,0,0,0,0,0,0#");
                    }
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        v.performHapticFeedback(HapticFeedbackConstants.LONG_PRESS,HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING);
                        v.playSoundEffect(SoundEffectConstants.CLICK);
                        mButtonBack.setBackgroundColor(Color.parseColor("#CC0000"));

                        SendBlueToothProtocol("$2,0,0,0,0,0,0,0,0,0#");

                    }
                }break;

                case R.id.button_left: {
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        mButtonLeft.setBackgroundColor(Color.parseColor("#C0C0C0"));

                        SendBlueToothProtocol("$0,0,0,0,0,0,0,0,0,0#");
                    }
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        v.performHapticFeedback(HapticFeedbackConstants.LONG_PRESS,HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING);
                        v.playSoundEffect(SoundEffectConstants.CLICK);
                        mButtonLeft.setBackgroundColor(Color.parseColor("#CC0000"));

                        SendBlueToothProtocol("$3,0,0,0,0,0,0,0,0,0#");

                    }
                }break;

                case R.id.button_right: {
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        mButtonRight.setBackgroundColor(Color.parseColor("#C0C0C0"));
                        SendBlueToothProtocol("$0,0,0,0,0,0,0,0,0,0#");
                    }
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        v.performHapticFeedback(HapticFeedbackConstants.LONG_PRESS,HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING);
                        v.playSoundEffect(SoundEffectConstants.CLICK);
                        mButtonRight.setBackgroundColor(Color.parseColor("#CC0000"));

                        SendBlueToothProtocol("$4,0,0,0,0,0,0,0,0,0#");

                    }
                }break;

                case R.id.button_stop: {
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        mButtonStop.setBackgroundColor(Color.parseColor("#C0C0C0"));
                    }
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        v.performHapticFeedback(HapticFeedbackConstants.LONG_PRESS,HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING);
                        v.playSoundEffect(SoundEffectConstants.CLICK);
                        mButtonStop.setBackgroundColor(Color.parseColor("#CC0000"));

                        SendBlueToothProtocol("$0,0,0,0,0,0,0,0,0,0#");
                    }
                } break;

                /*左旋*/
                case R.id.button_levo: {
                    if (event.getAction() == MotionEvent.ACTION_UP) {

                        SendBlueToothProtocol("$0,0,0,0,0,0,0,0,0,0#");
                        mButtonLevo.setBackgroundColor(Color.parseColor("#C0C0C0"));
                    }
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        v.performHapticFeedback(HapticFeedbackConstants.LONG_PRESS,HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING);
                        v.playSoundEffect(SoundEffectConstants.CLICK);

                        mButtonLevo.setBackgroundColor(Color.parseColor("#CC0000"));
                        SendBlueToothProtocol("$0,1,0,0,0,0,0,0,0,0#");
                    }
                } break;

                /*右旋*/
                case R.id.button_dextro: {
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        mButtonDextro.setBackgroundColor(Color.parseColor("#C0C0C0"));

                        SendBlueToothProtocol("$0,0,0,0,0,0,0,0,0,0#");


                    }
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        v.performHapticFeedback(HapticFeedbackConstants.LONG_PRESS,HapticFeedbackConstants.FLAG_IGNORE_GLOBAL_SETTING);
                        v.playSoundEffect(SoundEffectConstants.CLICK);

                        mButtonDextro.setBackgroundColor(Color.parseColor("#CC0000"));
                        SendBlueToothProtocol("$0,2,0,0,0,0,0,0,0,0#");

                    }
                }break;


            }
            return false;
        }

    }



}
