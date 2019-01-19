#include "./PinChangeInt.h"
#include "./MsTimer2.h"
#include "./BalanceCar.h"
#include "./KalmanFilter.h"
#include "./I2Cdev.h"
#include "./MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"


MPU6050 mpu;
BalanceCar balancecar;
KalmanFilter kalmanfilter;
int16_t ax, ay, az, gx, gy, gz;

#define IN1M 7
#define IN2M 6
#define IN3M 13
#define IN4M 12
#define PWMA 9
#define PWMB 10
#define STBY 8

#define PinA_left 2
#define PinA_right 4

byte inByte; //port receive
int num;
double Setpoint;                                       //angle DIP
double Setpoints, Outputs = 0;                         //velocity DIP
double kp = 30, ki = 0.0, kd = 0.56;
double kp_speed =3.12, ki_speed = 0.068, kd_speed = 0.0;//coefficient
double kp_turn = 28, ki_turn = 0, kd_turn = 0.29;
const double PID_Original[6] = {30, 0.0, 0.56,3.12, 0.068, 0.0};//original

double setp0 = 0, dpwm = 0, dl = 0;
float value;

//********************angle data*********************//
float Q;
float Angle_ax; //tilt
float Angle_ay;
float K1 = 0.05; // weight
float angle0 = 0.00;
int slong;
//********************angle data*********************//

//***************Kalman_Filter*********************//
float Q_angle = 0.001, Q_gyro = 0.005;
float R_angle = 0.5 , C_0 = 1;
float timeChange = 5; //sample period
float dt = timeChange * 0.001;//sampel time
//***************Kalman_Filter*********************//

//*********************************************
//******************** speed count ************
//*********************************************

volatile long count_right = 0;
volatile long count_left = 0;
int speedcc = 0;

//////////////////////countpluse/////////////////////////
int lz = 0;
int rz = 0;
int rpluse = 0;
int lpluse = 0;
int sumam;
/////////////////////countpluse////////////////////////////

//////////////spin coefficient///////////////////////////////
int turncount = 0;
float turnoutput = 0;
//////////////spin coefficient///////////////////////////////

//////////////bluetooth/wifi///////////////////
#define run_car     '1'//up
#define back_car    '2'//down
#define left_car    '3'//left
#define right_car   '4'//right
#define stop_car    '0'//stop
/*car state*/
enum {
  enSTOP = 0,
  enRUN,
  enBACK,
  enLEFT,
  enRIGHT,
  enTLEFT,
  enTRIGHT
} enCarState;
int incomingByte = 0;
String inputString = ""; 
boolean newLineReceived = false;
boolean startBit  = false;
int g_carstate = enSTOP;
String returntemp = "";
boolean g_autoup = false;
int g_uptimes = 5000;

int front = 0;
int back = 0;
int turnl = 0;
int turnr = 0;
int spinl = 0;
int spinr = 0;
int bluetoothvalue;
//////////////bluetooth control///////////////////

//////////////////ultrasonic//////////////////

int chaoshengbo = 0;
int tingzhi = 0;
int jishi = 0;

//////////////////ultrasonic//////////////////


//////////////////////countpluse///////////////////////
void countpluse()
{

  lz = count_left;
  rz = count_right;

  count_left = 0;
  count_right = 0;

  lpluse = lz;
  rpluse = rz;

  if ((balancecar.pwm1 < 0) && (balancecar.pwm2 < 0))
  {
    rpluse = -rpluse;
    lpluse = -lpluse;
  }
  else if ((balancecar.pwm1 > 0) && (balancecar.pwm2 > 0))
  {
    rpluse = rpluse;
    lpluse = lpluse;
  }
  else if ((balancecar.pwm1 < 0) && (balancecar.pwm2 > 0))
  {
    rpluse = rpluse;
    lpluse = -lpluse;
  }
  else if ((balancecar.pwm1 > 0) && (balancecar.pwm2 < 0))
  {
    rpluse = -rpluse;
    lpluse = lpluse;
  }

  //lift
  balancecar.stopr += rpluse;
  balancecar.stopl += lpluse;

  //interrupt
  balancecar.pulseright += rpluse;
  balancecar.pulseleft += lpluse;
  sumam = balancecar.pulseright + balancecar.pulseleft;
}
////////////////////countpluse///////////////////////



//////////////////angle PD////////////////////
void angleout()
{
  balancecar.angleoutput = kp * (kalmanfilter.angle + angle0) + kd * kalmanfilter.Gyro_x;//PD 角度环控制
}
//////////////////angle PD////////////////////

//////////////////////////////////////////////////////////
//////////////////interrupt////////////////////
/////////////////////////////////////////////////////////
void inter()
{
  sei();
  countpluse();
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  kalmanfilter.Angletest(ax, ay, az, gx, gy, gz, dt, Q_angle, Q_gyro, R_angle, C_0, K1);                                   //获取angle 角度和卡曼滤波
  angleout();

  speedcc++;
  if (speedcc >= 8)
  {
    Outputs = balancecar.speedpiout(kp_speed, ki_speed, kd_speed, front, back, setp0);
    speedcc = 0;
  }
  turncount++;
  if (turncount > 4)
  {
    turnoutput = balancecar.turnspin(turnl, turnr, spinl, spinr, kp_turn, kd_turn, kalmanfilter.Gyro_z);                              //旋转子函数
    turncount = 0;
  }
  balancecar.posture++;
  balancecar.pwma(Outputs, turnoutput, kalmanfilter.angle, kalmanfilter.angle6, turnl, turnr, spinl, spinr, front, back, kalmanfilter.accelz, IN1M, IN2M, IN3M, IN4M, PWMA, PWMB);                            //小车总PWM输出
 
}
//////////////////////////////////////////////////////////
//////////////////interrupt///////////////////
/////////////////////////////////////////////////////////
void SendAutoUp()
{
  g_uptimes --;
  if ((g_autoup == true) && (g_uptimes == 0))
  {
    String CSB, VT;
    char temp[10]={0};
    float fgx;
    float fay;
    float leftspeed;
    float rightspeed;
   
    fgx = gx;
    fay = ay;
    leftspeed = balancecar.pwm1;
    rightspeed = balancecar.pwm2;
    
    double Gx = (double)((fgx - 128.1f) / 131.0f);
    double Ay = ((double)fay / 16384.0f) * 9.8f;
    
   if(leftspeed > 255 || leftspeed < -255)
      return;
   if(rightspeed > 255 || rightspeed < -255)
      return;
   if((Ay < -20) || (Ay > 20))
      return;
   if((Gx < -3000) || (Gx > 3000))
      return; 
      
    returntemp = "";

    memset(temp, 0x00, sizeof(temp));
    //sprintf(temp, "%3.1f", leftspeed);
    dtostrf(leftspeed, 3, 1, temp);  // 相當於 %3.2f
    String LV = temp;
    
    memset(temp, 0x00, sizeof(temp));
    //sprintf(temp, "%3.1f", rightspeed);
    dtostrf(rightspeed, 3, 1, temp);  // 相當於 %3.1f
    String RV = temp;

    memset(temp, 0x00, sizeof(temp));
    //sprintf(temp, "%2.2f", Ay);
    dtostrf(Ay, 2, 2, temp);  // 相當於 %2.2f
    String AC = temp;
     
    memset(temp, 0x00, sizeof(temp));
    //sprintf(temp, "%4.2f", Gx);
    dtostrf(Gx, 4, 2, temp);  // 相當於 %4.2f
    String GY = temp;
    
    CSB = "0.00";
    VT = "0.00";
    //AC =
    returntemp = "$LV" + LV + ",RV" + RV + ",AC" + AC + ",GY" + GY + ",CSB" + CSB + ",VT" + VT + "#";
    Serial.print(returntemp);
  }
  
  if (g_uptimes == 0)
      g_uptimes = 5000;
}


// ===    initial     ===
void setup() {
  //TB6612FNGN
  pinMode(IN1M, OUTPUT);                  
  pinMode(IN2M, OUTPUT);
  pinMode(IN3M, OUTPUT);
  pinMode(IN4M, OUTPUT);
  pinMode(PWMA, OUTPUT);//left
  pinMode(PWMB, OUTPUT);//right
  pinMode(STBY, OUTPUT);


  //initial 
  digitalWrite(IN1M, 0);
  digitalWrite(IN2M, 1);
  digitalWrite(IN3M, 1);
  digitalWrite(IN4M, 0);
  digitalWrite(STBY, 1);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);

  pinMode(PinA_left, INPUT);
  pinMode(PinA_right, INPUT);


  //I2C
  Wire.begin();
  Serial.begin(9600);
  delay(1500);
  mpu.initialize();
  delay(2);
  balancecar.pwm1 = 0;
  balancecar.pwm2 = 0;

  MsTimer2::set(5, inter);
  MsTimer2::start();

}

////////////////////////////////////////turn//////////////////////////////////

void ResetPID()
{
  kp = PID_Original[0];
  ki =  PID_Original[1];
  kd =  PID_Original[2];
  kp_speed =  PID_Original[3];
  ki_speed =  PID_Original[4];
  kd_speed =  PID_Original[5];
}
void ResetCarState()
{
  turnl = 0; 
  turnr = 0;  
  front = 0; 
  back = 0; 
  spinl = 0; 
  spinr = 0; 
  turnoutput = 0;
}
// ===       main loop      ===
void loop() {

 String returnstr = "$0,0,0,0,0,0,0,0,0,0,0,0cm,8.2V#";

    attachInterrupt(0, Code_left, CHANGE);
    attachPinChangeInterrupt(PinA_right, Code_right, CHANGE);
    
    if (newLineReceived)
    {
      switch (inputString[1])
      {
        case run_car:   g_carstate = enRUN;   break;
        case back_car:  g_carstate = enBACK;  break;
        case left_car:  g_carstate = enLEFT;  break;
        case right_car: g_carstate = enRIGHT; break;
        case stop_car:  g_carstate = enSTOP;  break;
        default: g_carstate = enSTOP; break;
      }

      if (inputString[3] == '1' && inputString.length() == 21) //left spin
      {
        g_carstate = enTLEFT;
        //Serial.print(returnstr);
      }
      else if (inputString[3] == '2' && inputString.length() == 21) //right spin
      {
        g_carstate = enTRIGHT;
       // Serial.print(returnstr);
      }
  
      if (inputString[5] == '1') //inquire PID
      {
        char charkp[7], charkd[7], charkpspeed[7], charkispeed[7];
  
        dtostrf(kp, 3, 2, charkp);  // 相當於 %3.2f
        dtostrf(kd, 3, 2, charkd);  // 相當於 %3.2f
        dtostrf(kp_speed, 3, 2, charkpspeed);  // 相當於 %3.2f
        dtostrf(ki_speed, 3, 2, charkispeed);  // 相當於 %3.2f
  
        String strkp = charkp; String strkd = charkd; String strkpspeed = charkpspeed; String strkispeed = charkispeed;
  
        returntemp = "$0,0,0,0,0,0,AP" + strkp + ",AD" + strkd + ",VP" + strkpspeed + ",VI" + strkispeed + "#";
  
        Serial.print(returntemp);
      }
      else if (inputString[5] == '2')//keep initial
      {
        ResetPID();
        Serial.print("$OK#");
      }
  
      if (inputString[7] == '1')
      {
        g_autoup = true;
        Serial.print("$OK#");
      }
      else if (inputString[7] == '2')
      {
        g_autoup = false;
        Serial.print("$OK#");
      }
  
      if (inputString[9] == '1') //update $0,0,0,0,1,1,AP23.54,AD85.45,VP10.78,VI0.26#
      {
        int i = inputString.indexOf("AP");
        int ii = inputString.indexOf(",", i);
        if(ii > i)
        {
          String m_skp = inputString.substring(i + 2, ii);
          m_skp.replace(".", "");
          int m_kp = m_skp.toInt();
          kp = (double)( (double)m_kp / 100.0f);
        }
       
  
        i = inputString.indexOf("AD");
        ii = inputString.indexOf(",", i);
        if(ii > i)
        {
          String m_skd = inputString.substring(i + 2, ii);
          m_skd.replace(".", "");
          int m_kd = m_skd.toInt();
          kd = (double)( (double)m_kd / 100.0f);
        }
        Serial.print("$OK#");
      }
  
      if (inputString[11] == '1') //update
      {
        int i = inputString.indexOf("VP");
        int ii = inputString.indexOf(",", i);
        if(ii > i)
        {
          String m_svp = inputString.substring(i + 2, ii);
          m_svp.replace(".", "");
          int m_vp = m_svp.toInt();
          kp_speed = (double)( (double)m_vp / 100.0f);
        }

        i = inputString.indexOf("VI");
        ii = inputString.indexOf("#", i);
        if(ii > i)
        {
          String m_svi = inputString.substring(i + 2, ii);
          m_svi.replace(".", "");
          int m_vi = m_svi.toInt();
          ki_speed = (double)( (double)m_vi / 100.0f);
          Serial.print("$OK#");
        }
       
      }

      inputString = "";   // clear the string
      newLineReceived = false;
  
    }
    switch (g_carstate)
    {
      case enSTOP: turnl = 0; turnr = 0;  front = 0; back = 0; spinl = 0; spinr = 0; turnoutput = 0; break;
      case enRUN: ResetCarState();front = 350; break;
      case enLEFT: turnl = 1; break;
      case enRIGHT: turnr = 1; break;
      case enBACK: ResetCarState();back = -350; break;
      case enTLEFT: spinl = 1; break;
      case enTRIGHT: spinr = 1; break;
      default: front = 0; back = 0; turnl = 0; turnr = 0; spinl = 0; spinr = 0; turnoutput = 0; break;
    }

  SendAutoUp();


}

////////////////////////////////////////pwm///////////////////////////////////



//////////////////////////countpluse/////////////////////////////////////

void Code_left() {

  count_left ++;

}



void Code_right() {

  count_right ++;

}

//////////////////////////countpluse/////////////////////////////////////

int num1 = 0;
void serialEvent()
{
  
  while (Serial.available())
  {
    incomingByte = Serial.read();
    if (incomingByte == '$')
    {
      num1 = 0;
      startBit = true;
    }
    if (startBit == true)
    {
      num1++;
      inputString += (char) incomingByte;
    }
    if (startBit == true && incomingByte == '#')
    {
      newLineReceived = true;
      startBit = false;
    }
    
    if(num1 >= 80)
    {
      num1 = 0;
      startBit = false;
      newLineReceived = false;
      inputString = "";
    }	
  }
}