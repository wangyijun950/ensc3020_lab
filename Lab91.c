#include "eyebot.h"
#include "types.h"
#define NO_HUE 255

int button = 0;
int hue_sum, hue_average, hue_current;

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

int main(){
    BYTE img[QVGA_SIZE];                                //create buffer for image, QVGA_SIZE=320*240*3
    LCDMenu("HUE"," "," ","EXIT");                    //set for button pressed
    CAMInit(QVGA);
    while(button != KEY4){
        CAMGet(img);                                    //store one frame of RGB data into img buffer
        LCDImage(img);                                  //display image
        LCDLine(140, 120, 180, 120, RED);
        LCDLine(160, 100, 160, 140, RED);               //x1,y1,x2,y2,cross hairs
        usleep(100);
        while (button == KEY1){
            hue_sum = 0;
            for (int i = 119; i < 122; i++){
                for (int j = 159; j < 162; j++){
                    BYTE r = img[(i * 320 + j) * 3];
                    BYTE g = img[(i * 320 + j) * 3 + 1];
                    BYTE b = img[(i * 320 + j) * 3 + 2];
                    hue_current = RGBtoHue(r, g, b);
                    hue_sum = hue_sum + hue_current;
                }
            }
            hue_average = hue_sum / 9;
            LCDSetPrintf(18, 0, "Average Hue = %d", hue_average);
            button = KEYRead();
        }
        button = KEYRead();
    }
}

