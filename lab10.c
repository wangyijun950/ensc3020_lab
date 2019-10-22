#include "eyebot.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include <math.h>

#define NO_HUE 255
#define HUE_MIN_THRESHOLD               25
#define HUE_MAX_THRESHOLD               55
#define SATURATION_THRESHOLD    35
#define INTENSITY_THRESHOLD             35
#define RED_HISTOGRAM_THRESHOLD 35
#define SERVO_MIDDLE 128

#define STRAIGHT_SPEED   30
#define CURVE_SPEED   10
#define WALL_THRESHOLD  20 //5cm

BYTE cameraIsRed[QVGA_PIXELS];
int cameraRowRedCounts[QVGA_Y];
int cameraColRedCounts[QVGA_X];
BYTE processedImage[QVGA_SIZE]; 
BYTE HSI[QVGA_SIZE];
int LCD_CENTER_X, LCD_CENTER_Y, IMG_X, IMG_Y;
int LCD_WIDTH, LCD_HEIGHT;
const int CAMERA_CENTER_X = QVGA_X / 2;
const int CAMERA_CENTER_Y = QVGA_Y / 2;
// ------------------------------------------

int end_position; 

int max(int a, int b) {
    return (a>b?a:b);
}

int min(int a, int b) {
    return (a<b?a:b);
}


BYTE RGBtoHue(BYTE r, BYTE g, BYTE b) /* return hue value for RGB color */ 
 { int hue, delta, ma, mi; 
   ma   = max(r, max(g,b));  
   mi   = min(r, min(g,b));   
   delta = ma - mi;   
   hue =0; /* init hue*/ 
   if (2*delta <= ma) hue = NO_HUE; /* gray, no color */   
   else {     
           if (r==ma) hue = 42 + 42*(g-b)/delta;      /* 1*42 */     
           else if (g==ma) hue = 126 +42*(b-r)/delta; /* 3*42 */     
           else if (b==ma) hue = 210 +42*(r-g)/delta; /* 5*42 */   
   }   
   return hue; /* now: hue is in range [0..252] */ 
}


BYTE RGBtoIntensity(BYTE r, BYTE g, BYTE b) {
        int intensity = (1/3)*(r+g+b);
        return intensity;
}

BYTE RGBtoSaturation(BYTE r, BYTE g, BYTE b) {
        int sat = 1 - 3/(r+g+b)*min(min(r,g),b);
        return sat;
}

int isRed(int pixel) {
    BYTE hue = HSI[pixel*3];
    BYTE saturation = HSI[pixel*3+1];
    BYTE intensity = HSI[pixel*3+2];

    return hue >= HUE_MIN_THRESHOLD && hue <= HUE_MAX_THRESHOLD && saturation > SATURATION_THRESHOLD && intensity > INTENSITY_THRES$
}

void calcCameraIsRed() {
    // Clear the row and col red counts
    for (int x = 0; x < QVGA_X; ++x) {
        cameraColRedCounts[x] = 0;
    }
    for (int y = 0; y < QVGA_Y; ++y) {
        cameraRowRedCounts[y] = 0;
    }

    // Calculate whether each pixel is red
    for (int x = 0; x < QVGA_X; ++x) {
        for (int y = 0; y < QVGA_Y; ++y) {
            int pixel = x + y * QVGA_X;
            int pixelIsRed = isRed(pixel);

            cameraIsRed[pixel] = pixelIsRed;
            if (pixelIsRed) {
                cameraColRedCounts[x] += 1;
                cameraRowRedCounts[y] += 1;
            }
        }
    }
}


void findRedObject(int * x, int * y) {
    int maxX = -1;
    int maxXCount = 0;
    int maxY = -1;
    int maxYCount = 0;

    // Search for most red column
    for (int x = 0; x < QVGA_X; ++x) {
        int count = cameraColRedCounts[x];
    if (count > maxXCount && count > RED_HISTOGRAM_THRESHOLD) {
            maxX = x;
            maxXCount = count;
        }
    }

    // Search for most red row
    for (int y = 0; y < QVGA_Y; ++y) {
        int count = cameraRowRedCounts[y];
        if (count > maxYCount && count > RED_HISTOGRAM_THRESHOLD) {
            maxY = y;
            maxYCount = count;
        }
    }

    *x = maxX;
   *y = maxY;
}

void drawImage(BYTE * image) {
    LCDImageStart(IMG_X, IMG_Y, QVGA_X, QVGA_Y);
    LCDImage(image);
}

BYTE colimage[QVGA_SIZE];

void calcCameraHSI() {
    BYTE red, green, blue;
    BYTE hue, saturation, intensity;
    for (int pixel = 0; pixel < QVGA_PIXELS; ++pixel) {
        red = colimage[pixel*3];
        green = colimage[pixel*3+1];
       blue = colimage[pixel*3+2];

        // Convert from RGB to HSI
        IPPRGB2HSI(red, green, blue, &hue, &saturation, &intensity);

        HSI[pixel*3] = hue;
        HSI[pixel*3+1] = saturation;
        HSI[pixel*3+2] = intensity;
    }
}

void drawCrosshairs(int x, int y) {
    int size = 10;
    LCDLine(x - size, y, x + size, y, ORANGE);
    LCDLine(x, y - size, x, y + size, ORANGE);
   x -= 1;
    y -= 1;
    LCDLine(x - size, y, x + size, y, ORANGE);
    LCDLine(x, y - size, x, y + size, ORANGE);
}

void part1() {

    int x;
    int y;
    LCDGetSize(&x, &y);

    drawImage(colimage);
    drawCrosshairs(x/2, y/2);  // draw cross in centre

        //int row;
       //int col;
        //int r, g, b;
        /*for(row=0; row < 120; ++row) {
                for(col=0; col<160; ++col) {
                        int pixel = row * 160 + col;
                        pixel = pixel*3;
                        r = colimage[pixel*3];
                        ++pixel;
                        g = colimage[pixel];
                        ++pixel;
                        b = colimage[pixel];
                        int hue = IPPRGB2Hue(r, g, b);
                        HSI[(row*160+col)*3] = hue; // get hue values
                }

        }*/
 calcCameraHSI();
        /*int sum = 0;
        for(row=59; row<=61; ++row) {
                for(col=79; col<=81; ++col) {
                        int pixel = row*160+col;
                        sum+=HSI[pixel*3];
                }
        }
        double average = sum/9;*/
        int hueSum = 0;
        int hueCount = 0;
        for (int dx = -1; dx <= 1; ++dx) {         //interate through centre 3 pixels
            for (int dy = -1; dy <= 1; ++dy) {
                int x = CAMERA_CENTER_X + dx;
                int y = CAMERA_CENTER_Y + dy;
                int pixel = x + y * QVGA_X;       // pixel = col + row*width
                BYTE hue = HSI[pixel*3];    // getting hue value from above

                hueSum += hue;
                hueCount += 1;
            }
        }
        int averageHue = hueSum / hueCount;

        int recordedHue = averageHue;



        /*LCDSetColor(WHITE, BLACK);
        LCDArea(0, 0, x/2, 30, BLACK, 1);
        LCDSetPrintf(0,0, "Average Hue: %d\n", recordedHue);*/
   }




BYTE HSI[QVGA_SIZE];

void part2() {

        /**int row;
        int col;
        int r, g, b;
        for(row=0; row < 120; ++row) {
                for(col=0; col<160; ++col) {
                        int pixel = row * 160 + col;
                        pixel = pixel*3;
 r = colimage[pixel*3];
                        ++pixel;
                        g = colimage[pixel];
                        ++pixel;
                        b = colimage[pixel];
                        int hue = RGBtoHue(r, g, b);
                        HSI[(row*160+col)*3] = hue; // get hue values
                        HSI[(row*160+col)*3+1] = RGBtoSaturation(r,g,b);
                        HSI[(row*160+col)*3+2] = RGBtoIntensity(r,g,b);
                }
        }*/
    calcCameraHSI();
        calcCameraIsRed();  // checking which pixels are red

    for (int x = 0; x < QVGA_X; ++x) {
        for (int y = 0; y < QVGA_Y; ++y) {
          int pixel = x + y * QVGA_X;

            if (cameraIsRed[pixel]) {
                processedImage[pixel*3] = 255;   // set red pixels to high and rest to low
                processedImage[pixel*3+1] = 0;
                processedImage[pixel*3+2] = 0;
            } else {
                BYTE intensity = HSI[pixel*3+2];  //grabbing intensity value from HSI array
                processedImage[pixel*3] = intensity;
                processedImage[pixel*3+1] = intensity;  // getting gray scale based on intensity
                processedImage[pixel*3+2] = intensity;
            }
        }
    }
        drawImage(processedImage);
            int x, y;
            findRedObject(&x, &y);

            if (x >= 0 && y >= 0) {
                drawCrosshairs(x+IMG_X, y+IMG_Y);
            }

}



int lastFoundX = 0;
BYTE image[QVGA_SIZE];

int currangle;
void part3() {
    part2();

    int x, y;
    findRedObject(&x, &y);

    int turn_threshold = CAMERA_CENTER_X / 6;
    //int speed = 10;
    //int time = 30;

    if (x < 0 || y < 0) {
        x = lastFoundX;
        turn_threshold = 0;
        //time = 200;
    } else {
        lastFoundX = x;
    }
    int dx = x - CAMERA_CENTER_X;

    if (dx < -turn_threshold) {
        currangle +=5;
        currangle = currangle % 255;
        if(currangle==0) currangle++;
        SERVOSet(1, currangle);
        //MOTORSpeed(2, speed);
    } else if (dx > turn_threshold) {
        currangle -=5;
        if(currangle<=.1) currangle=254;
        SERVOSet(1, currangle);
        //MOTORSpeed(2, -speed);
}
    CAMGet(image);
    //usleep(1000*time);
}

void setup_camera() {
    LCDGetSize(&LCD_WIDTH, &LCD_HEIGHT);
    LCD_CENTER_X = LCD_WIDTH / 2;
    LCD_CENTER_Y = LCD_HEIGHT / 2;
    IMG_X = (LCD_WIDTH - QVGA_X) / 2;
    IMG_Y = (LCD_HEIGHT - QVGA_Y) / 2;
    part1();
    CAMInit(QVGA);
}

void setup_lcd() {
    LCDMenu("Drive", "", "Reset", "Exit");
}

void wait_for_start() {

    printf("Press 'Start' when ready\n");
    KEYWait(KEY1);

}

int can_detection() {  // returns 0 if can is in centre
    CAMGet(colimage);
    //part3();
    usleep(100);
    part2();

    int x, y;
    findRedObject(&x, &y);
    int turn_threshold = CAMERA_CENTER_X / 6;
    //int speed = 10;
    //int time = 30;

    if (x < 0 || y < 0) {
        x = lastFoundX;
        turn_threshold = 0;
        //time = 200;
    } else {
        lastFoundX = x;
    }

    int dx = x - CAMERA_CENTER_X;

    if (dx < -turn_threshold) {
       VWTurn(1, 50); //turn ~5 degrees right
        VWWait();
        usleep(1000);

    } else if (dx > turn_threshold) {

        VWTurn(-1, 50); // turn ~5 degrees left
        VWWait();
        usleep(1000);
    }
    else {
        printf("Can centered\n");
        return 0;
        VWWait();
        usleep(1000);
    }

    CAMGet(image);
    printf("Can not centered\n");
    return 1; //we havent centered on can yet
}

int drive_to_can() {  // returns 0 if can is found
    int psd_front, psd_left, psd_right, can_centred;
    psd_front = PSDGet(1); //PSDGet gives you distance away in mm, if you use PSDRaw - higher values means youre closer
    psd_left = PSDGet(2);
    psd_right = PSDGet(3);
    if(psd_left < WALL_THRESHOLD)  { //if left psd close to wall
        VWDrive(50, -50, CURVE_SPEED); // curve away from wall
        VWWait();
        can_centred = can_detection();
        while(can_centred ==1){
           can_centred = can_detection();
        }
    }
    else if(psd_right < WALL_THRESHOLD){
        VWDrive(50,50,CURVE_SPEED);
        VWWait();
        can_centred = can_detection();
        while(can_centred ==1) {
            can_centred = can_detection();
        }

    }
    if(psd_front > 90) { //if front psd is more than 9cm away from can
        //drive straight for 5cm
        //VWStraight(50, STRAIGHT_SPEED);
        VWSetSpeed(50,0);
       VWWait();
    }
    else { //if front is closer than 9cm to can
        while(psd_front < 40){
            printf("Approaching can\n");
            //VWStraight(10,5); //distance and speed
            VWSetSpeed(20,0);
            VWWait();
            psd_front = PSDGet(1);
        }
        printf("Found can\n");
        return 0;
        usleep(2000);
    }

    return 1;
}

void return_home(){
    int x,y,can_angle, end_pos;
    VWStraight(-50,50); //reverse 50cm at speed 10mm/sec
    //VWSetSpeed(-50,0);
    VWWait();
    end_pos = VWGetPosition(&x,&y,&can_angle); // get current position
    //VWTurn(-can_angle, 50); // angle, speed in rad/100sec
    //VWWait();
    VWTurn(180, 100); //turn 180 degrees to face start position
    VWWait();


    int dist = sqrt(pow(x,2)+pow(y,2));  // find straight distance to get back to start position
    VWStraight(dist,50);
   VWWait();

    /*if (end_pos > 0){
        VWSetSpeed(50,0);
        VWWait();
        end_pos = VWGetPosition(&x,&y,&can_angle);
    }
    else {
        VWSetSpeed(0,0);
        VWWait();
    }*/

    VWTurn(180,100);
    printf("Program Done\n");

}

int main() {     // here we are calling or functions in order of operation to perform can detection and retun back to start to wait$
    setup_lcd();
    setup_camera();
        int start_pos;
    //comment these 2 lines out
    //SERVOSet(1, SERVO_MIDDLE);
    //currangle = SERVO_MIDDLE;


    while(KEYRead() != KEY4) {
                wait_for_start();
        int centered = 1;
        while(centered!=0) { //center can
            centered = can_detection();
        }
        VWSetPosition(0, 0, 0);   // set start position

        int can_found = 1;
        while(can_found !=0){
            can_found = drive_to_can();

        }
        return_home();


        }

        return 0;
}



