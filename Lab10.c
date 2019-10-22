#include "eyebot.h"
#include <math.h>
#include "types.h"
#define NO_HUE 255

BYTE img[QVGA_SIZE];                                    //create buffer for image,QVGA_SIZE=320*240*3
BYTE binary[QVGA_PIXELS];                               //create space for bin value
BYTE hist_col[320];
BYTE hist_row[240];

int button = 0;
int col_max = 0;
int row_max = 0;
int max_x;
int max_y;

int MAX(int a,int b){
    if(a > b) return a;
    else return b;
}
int MIN(int a,int b){
    if(a < b) return a;
    else return b;
}

BYTE RGBtoHue(BYTE r, BYTE g, BYTE b){                  // return hue value for RGB color
    int hue, delta, max, min;
    max = MAX(r, MAX(g, b));
    min = MIN(r, MIN(g, b));
    delta = max - min;
    hue = 0;                                            // initialize hue
    if (2*delta <= max) hue = NO_HUE;                   // gray, no color
    else {
        if (r == max) hue = 42 + 42*(g-b)/delta;        // 1*42
        else if (g == max) hue = 126 + 42*(b-r)/delta;  // 3*42
        else if (b == max) hue = 210 + 42*(r-g)/delta;  // 5*42
    }
    return hue;                                         // now: hue is in range [0..252]
}

void Init(){                                            //Camera and LCD initialization
    LCDMenu("FIND","DRIVE","NEXT","EXIT");                //set for button pressed
    CAMInit(QVGA);
    button = 0;
    col_max = 0;
    row_max = 0;
    max_x=0;
    max_y=0;
}

void ChangeToBinaryImage(){
    int hue_value;
    for(int i=0;i<240;i++){
        for(int j=0;j<320;j++){
            BYTE r = img[3*(320*i +j)];
            BYTE g = img[3*(320*i +j)+1];
            BYTE b = img[3*(320*i +j)+2];
            hue_value = RGBtoHue(r,g,b);
            if(hue_value < 50){                         // 50 is the hue threshold for red
                binary[i*320+j] = 1;
            }
            else{
                binary[i*320+j] = 0;
            }
        }
    }
    LCDImage(img);
}

void Histogram(){
    int x, y, j, k;
    int col_sum = 0;
    int row_sum = 0;
    for(x = 0; x < 320; x++){
        for(y = 0; y < 240; y++){
            col_sum = col_sum + binary[320*y+x];
        }
        hist_col[x] = col_sum;
        col_sum = 0;
    }
    for(j = 0; j < 240; j++){
        for(k = 0; k < 320; k++){
            row_sum = row_sum + binary[320*j+k];
        }
        hist_row[j] = row_sum;
        row_sum = 0;
    }
}

void FindMax(){
    for(int x = 0; x < 320; x++){
        if(hist_col[x] > col_max){
            col_max = hist_col[x];
            max_x = x ;                                  //find the max value in column
        }
    }
    for(int y = 0; y < 240; y++){
        if(hist_row[y] > row_max){
            row_max = hist_row[y];
            max_y = y;                                  //find the max value in row
        }
    }
    row_max = 0;
    col_max = 0;
    LCDSetPrintf(18,0,"x = %d y = %d", max_x, max_y);
    LCDLine(max_x-20, max_y, max_x+20, max_y, WHITE);    //hozi  x1,y1,x2,y2,cross hairs
    LCDLine(max_x, max_y+20, max_x, max_y-20, WHITE);    //vert
}

void Rotate() {
    if(max_x < 160){
        VWTurn(6, 60);
        VWWait();
    }
    else{
        VWTurn(-6, 60);
        VWWait();
    }
}

void drive()
{
    int x,y,phi;
    int dist;
    float angle;
    while(PSDGet(1)>150) {
        VWStraight(100,200);
        VWWait();
        CAMGet(img);
        ChangeToBinaryImage();
        Histogram();
        FindMax();
        Rotate();
    }
    VWWait();
    LCDSetPrintf(19,0,"GOT TARGET,BACKING");
    //turn on magnet
    VWGetPosition(&x,&y,&phi);
    dist=sqrt(pow(x,2)+pow(y,2));
    angle = atan2(y,x)*180/M_PI;
    VWTurn(180+angle-phi,30);
    VWWait();
    VWStraight(dist,200);
    VWWait();
    LCDSetPrintf(19,0,"READY FOR NEXT TARGET");
}


int main(){
    Init();
    VWSetPosition(0,0,0);
    while(button != KEY4){
        CAMGet(img);                                    //store one frame of RGB data into img buffer         ");
        while(button == KEY1){
            ChangeToBinaryImage();
            Histogram();
            FindMax();
            while(max_x < 150 || max_x > 170) {
                Rotate();
                CAMGet(img);
                ChangeToBinaryImage();
                Histogram();
                FindMax();
            }
            LCDSetPrintf(19,0,"FOUND TARGET");
            button = KEYRead();
        }
        while(button == KEY2){
            drive();
            button = KEYRead();
        }
        while(button == KEY3) {
            LCDClear();
            Init();
            VWSetPosition(0,0,0);
            button = KEYRead();
        }
        button = KEYRead();
    }
    CAMRelease();
    return 0;
}