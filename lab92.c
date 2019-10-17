#include "eyebot.h"
#include <math.h>
#include "types.h"
#define NO_HUE 255

BYTE img[QVGA_SIZE];                                    //create buffer for image,QVGA_SIZE=320*240*3
BYTE binary[QVGA_PIXELS];                               //create space for bin value
BYTE hist_col[320];
BYTE hist_row[240];
BYTE *gray[QVGA_PIXELS];
BYTE *red[QVGA_PIXELS];

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
    LCDMenu("GET","RED "," ","EXIT");                     //set for button pressed
    CAMInit(QVGA);
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
    //LCDImageBinary(binary);
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
    LCDSetPrintf(18,0,"x = %d y = %d",max_x,max_y);
    LCDLine(max_x-20, max_y, max_x+20, max_y, WHITE);    //hozi  x1,y1,x2,y2,cross hairs
    LCDLine(max_x, max_y+20, max_x, max_y-20, WHITE);    //vert
}

int main(){
    Init();
    while(button != KEY4){
        CAMGet(img);                                    //store one frame of RGB data into img buffer         ");
        ChangeToBinaryImage();
        LCDImageBinary(binary);
        Histogram();
        FindMax();
        //IPOverlayGray(binary, binary, RED, gray);
        while(button == KEY2) {
            while(button != KEY1){
                CAMGet(img);
                ChangeToBinaryImage();
                Histogram();
                FindMax();
                IPCol2Gray(img, gray);
                IPOverlayGray(gray, binary, RED, red);
            LCDImage(red);
            button = KEYRead();
            if(button == KEY4) return 0;}
        }
        button = KEYRead();
    }
    CAMRelease();
    return 0;
}
