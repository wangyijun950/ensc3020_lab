//
// 
//
#include "eyebot.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//declare functions
int speedCheck(int);
void bangController();
void SaveCSV(int);
void pController();
void piController();
void pidController();
int LiveArray(int, int);
void disp_LCD();
int i = 0;

//global vars
FILE *fp;
int dSpeed = 10000; //encoder clicks per sec
//int arr[100];
float Kp = 0.009;
float Ki = 0.005;
float Kd = 0.002;
int per = 50;
int  arr[320] = {0};
int LCD_scale =100;


int main() {
    LCDClear();
    ENCODERReset(2);
    static int count = 0;
    TIMER t1 = OSAttachTimer(per, &pidController); //set to p pi or pid func
    //TIMER t2 = OSAttachTimer(LCD_scale, &disp_LCD);

    while(1);
    OSDetachTimer(t1);
    //OSDetachTimer(t2);
    return 0;
}

//P controller
void pController(){
    int encNow, actSpeed, rMotor;
    static int encOld;
    static int count = 0;
    encNow = ENCODERRead(1);
    actSpeed = (encNow - encOld)*(1000/per);
    rMotor = Kp*(dSpeed - actSpeed);
    MOTORDrive(1, rMotor);
    encOld = encNow;

    if (count < 1000){
        SaveCSV(actSpeed);
        ++count;
    }
}
void SaveCSV( int speed) {
    fp = fopen("/home/pi/usr/speedLog.csv", "a");
    fprintf(fp, "%d\n", speed);
    fclose(fp);
}



void piController(){
  int encNow, actSpeed, rMotor, errSpeed;
  static int encOld, oldErr, rOld;
  static int count = 0;
  encNow = ENCODERRead(1);
  actSpeed = (encNow - encOld)*(1000/per);
  errSpeed = dSpeed - actSpeed;
  rMotor = rOld + Kp*(errSpeed - oldErr) + Ki*(errSpeed + oldErr)/2;
  rMotor = fmin(rMotor, +100);
  rMotor = fmax(rMotor, -100);
  MOTORDrive(1, rMotor);
  rOld = rMotor;
  oldErr = errSpeed;
  encOld = encNow;

  if (count < 1000){
    SaveCSV(actSpeed);
    ++count;
  }
}



void pidController(){
    int encNow, rMotor, errSpeed;
    static int encOld, oldErr, old_Err_der, rOld;
    static int count = 0, actSpeed;
  
    encNow = ENCODERRead(2);
  
    actSpeed = (encNow - encOld)*(1000/per);
    errSpeed = dSpeed - actSpeed;
  
    rMotor = rOld + Kp*(errSpeed - oldErr) + Ki*(errSpeed + oldErr)/2 + Kd*(errSpeed - 2*oldErr + old_Err_der);
    rMotor = fmin(rMotor, +100);
    rMotor = fmax(rMotor, -100);
  
    MOTORDrive(2, rMotor);
    rOld = rMotor;
    oldErr = errSpeed;
  old_Err_der = oldErr;
    encOld = encNow;

    //LCDPrintf("%i\n", v_act);
    LCDPixel(i%640, actSpeed/100+100, WHITE);
    i++;

    if (count < 1000){
        SaveCSV(actSpeed);
        ++count;
    }

}
/*
int LiveArray(int* arr){
    srand(time(NULL));

    for(int i = 0, i < 320; i++){
        arr[i] = rand()%100 +1;
    }
    return(0);
}

int LiveArray( int* arr, int speed) {
    LCDClear();
    //usleep(10); //Wait for LCD to clear
    //Shift each element in array to add new value
    for(int i=0; i<319; i++) {
        arr[i] = arr[i+1];
    }
    arr[319] = speed/80;
    //Print array to LCD
    for(int i=0; i<320; i++) {
        LCDPixel(i, 50+arr[i], WHITE);
    }
    //usleep(50);
    return 0;
}
