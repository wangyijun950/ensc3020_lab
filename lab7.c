#include  <stdio.h>
#include <stdlib.h>
#include "eyebot.h"


void initialize(){
  int v_act=0;
  int enc_old=0;
  int enc_new=0;
  int r_mot=0;
  int v_des=5030;//10060/2
  static int counter=0;
  FILE *fp;
}
void onoff_controller(){
  MOTORInit();
  enc_new=ENCODERRead(1);
  v_act=(enc_new-enc_old)*100;
  if(v_act<v_des){
    r_mot=100；
    print("%d ON\n",v_act);
    fprintf(fp,"%d\n",v_act);
    counter++
  }
  else{
    r_mot=0;
    print("%d ON\n",v_act);
    fprintf(fp,"%d\n",v_act);
    counter++
  }
  MOTORDrive(1,r_mot);
  enc_old=enc_new;
}
int main(){
  initialize();
  fp = fopen("/home/pi/usr/exp1.csv", "w");
  while(counter<1000){
   OSAttachTimer(50,onoff_controller);//1000/50=20hz
  }
  fclose(fp);
  MOTORDrive(1,0)
  return 0;
 }
