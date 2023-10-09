#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <conio.h>
#include <windows.h>


#define PI 3.1415926535

float* resetScreen(int Xsize, int Ysize);
float pixel(float x, float y , int time);
void renderArray1(float* a, int Ysize, int Xsize);
void renderArray2(float* a, int Ysize, int Xsize);
int clamp(int x,int a, int b);
float mod(float x, float a);
float ball(float x, float y , int time);
float degrade(float x, float y , int time);
float game(float x, float y , int time, int map[], int Msize, float pos[], float angle);

void main(){
    int time = 0;
    int freq = 5000;
    int Ysize = 40;
    int Xsize = 140;
    int renderType = 1;

    int Msize = 14;
    int map[] = {
                1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                1,0,0,0,0,0,0,1,0,0,0,0,0,1,
                1,0,0,0,0,0,0,1,0,0,0,0,0,1,
                1,0,0,0,0,0,1,1,1,0,0,0,0,1,
                1,0,1,1,0,0,0,0,0,0,0,0,0,1,
                1,0,0,1,0,0,0,0,0,0,0,0,0,1,
                1,0,0,1,0,0,1,0,0,0,0,0,0,1,
                1,0,0,0,0,0,1,0,0,0,0,0,0,1,
                1,0,0,0,0,0,1,0,0,0,0,0,0,1,
                1,0,0,1,0,0,1,1,1,0,0,1,1,1,
                1,0,0,1,0,0,1,0,0,0,0,0,0,1,
                1,0,0,0,0,0,1,0,0,0,0,0,0,1,
                1,0,0,0,0,0,1,0,0,0,0,0,0,1,
                1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                };

    float pos[] = {2.5,2.5};
    float angle = 0;
    float delta = 0.2;
    float speed = 0.15*delta;
    float rotSpeed = 0.15*delta;

    float* arrayImage = resetScreen(Xsize, Ysize);

    
    while (1) {

        if(GetAsyncKeyState(65)){
            Xsize += 1;
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize);
        }

        if(GetAsyncKeyState(81)){
            Xsize -= 1;
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize);
        }

        if(GetAsyncKeyState(90)){
            Ysize += 1;
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize);
        }

        if(GetAsyncKeyState(83)){
            Ysize -= 1;
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize);
        }
        
        if(GetAsyncKeyState(VK_LEFT)){
            angle -= rotSpeed;
        }
        if(GetAsyncKeyState(VK_RIGHT)){
            angle += rotSpeed;
        }
        if(GetAsyncKeyState(VK_UP)){
            pos[0] += cos(angle)*speed;
            pos[1] += sin(angle)*speed;
        }
        if(GetAsyncKeyState(VK_DOWN)){
            pos[0] -= cos(angle)*speed;
            pos[1] -= sin(angle)*speed;
        }

        for (int i = 0; i < Ysize; i++){
            for (int j = 0; j < Xsize; j++){
                arrayImage[i*Xsize + j] = game(((float)j) / Xsize, ((float)i) / Ysize, time, map, Msize, pos, angle);
                //arrayImage[i*Xsize + j] = pixel(((float)j) / Xsize, ((float)i) / Ysize, time);
            }            
        }
        if(renderType == 1){
            renderArray1(arrayImage, Ysize, Xsize);
        }
        if(renderType == 2){
            renderArray2(arrayImage, Ysize, Xsize);
        }
        time++;
        usleep(freq);
    }

    free(arrayImage);
    return;
}

float* resetScreen(int Xsize, int Ysize){
    printf("\x1b[2J");//for the renderer
    float* arr = (float*)malloc(Ysize * Xsize * sizeof(float));
    for (int i = 0; i < Ysize*Xsize; i++) {
        arr[i] = 0.;        
    }
    return arr;
}

float pixel(float x, float y , int time){
    return degrade(x,y,time);
}

float game(float x, float y , int time, int map[], int Msize, float pos[], float angle){
    float x0 = pos[0];
    float y0 = pos[1];
    float FOV = 1.5;
    float ang = angle +(x-0.5)*FOV;
    float xDir = cos(ang);
    float yDir = sin(ang);
    float xUnitStepSize = sqrt(1 + (yDir/xDir)*(yDir/xDir) );
    float yUnitStepSize = sqrt(1 + (xDir/yDir)*(xDir/yDir) );
    int xMapCheck = (int)x0;
    int yMapCheck = (int)y0;
    float xlen = 0;
    float ylen = 0;

    float xStep;
    float yStep;

    if(xDir < 0){
        xStep = -1;
        xlen = (x0 - (float)xMapCheck )*xUnitStepSize;
    }else{
        xStep = 1;
        xlen = ((float)(xMapCheck + 1) - x0 )*xUnitStepSize;
    }

    if(yDir < 0){
        yStep = -1;
        ylen = (y0 - (float)(yMapCheck) )*yUnitStepSize;
    }else{
        yStep = 1;
        ylen = ((float)(yMapCheck + 1) - y0 )*yUnitStepSize;
    }

    int found = 0;//bool
    float fdist = 0;
    float fdistMax = 100.;
    while ((found==0) && fdist < fdistMax){
        if (xlen < ylen){
            xMapCheck += xStep;
            fdist = xlen;
            xlen += xUnitStepSize;
        }else{
            yMapCheck += yStep;
            fdist = ylen;
            ylen += yUnitStepSize;
        }

        if(xMapCheck >= 0 && xMapCheck < Msize && yMapCheck >= 0 && yMapCheck < Msize){
            if(map[yMapCheck*Msize + xMapCheck] == 1){
                found = 1;
            }
        }
        
    }
    
    float xIntersect;
    float yIntersect;
    if(found){
        xIntersect = x0 + xDir*fdist;
        yIntersect = y0 + yDir*fdist;
    }

    float col = 0;
    if(xIntersect - floor(xIntersect) < 0.0001 || xIntersect - ceil(xIntersect) > -0.0001){
        if(xDir>0){
            col = 0.7;
        }else{
            col = 0.7;
        }
    }else{
        if(yDir>0){
            col = 0.9;
        }else{
            col = 0.9;
        }
    }

    float h = 0.5;

    if(fabs(tan(y-0.5))*(fdist*cos((x-0.5)*FOV)) < h){
        return col;
        //fdist/3.
    }else{
        return 0.;
    }

}

float ball(float x, float y , int time){
    float xpos = 0;
    float ypos = 0;
    float xspeed = 0.05;
    float yspeed = 0.035;
    xpos = mod(time*xspeed, 2.0);
    xpos = xpos > 1 ? 2 -xpos : xpos;
    ypos = mod(time*yspeed, 2.0);
    ypos = ypos > 1 ? 2 -ypos : ypos;

    if(sqrtf((x/0.65-xpos/0.65)*(x/0.65-xpos/0.65) + (y-ypos)*(y-ypos)) < 0.1){
        return 1.;
    }else{
        return 0.1;
    }
}

float degrade(float x, float y , int time){
    return mod(x/2.+(float)(time%100)/100. + y/2., 1.0);
}


void renderArray2(float* a, int Ysize, int Xsize){

    char *arraychar = (char*)malloc((Ysize * Xsize +1) * sizeof(char));
    for (int i = 0; i < Ysize*Xsize; i++) {
        if(i%Xsize==0){
            arraychar[i] = 10;
        }else{
            arraychar[i] = " .-:,~=;!vw#$@"[clamp((int)(a[i]*14), 0, 13)];        
        }
    }
    arraychar[Ysize * Xsize] = '\0';
    
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    printf(arraychar);
    return;
}

void renderArray1(float* a,int Ysize,int Xsize){
    printf("\x1b[H");
    char *arraychar = (char*)malloc((Ysize * Xsize +1) * sizeof(char));
    for (int i = 0; i < Ysize*Xsize; i++) {
        if(i%Xsize==0){
            arraychar[i] = 10;
        }else{
            arraychar[i] = " .-:,~=;!vw#$@"[clamp((int)(a[i]*14), 0, 13)];        
        }
    }
    arraychar[Ysize * Xsize] = '\0';
    printf(arraychar);
    return;
}

float mod(float x, float a){
    if(x > 0){
        while (x>a){
            x-=a;
        }
        return x;
    }
    if(x < 0){
        while (x<0){
            x+=a;
        }
        return x;
    }
    return 0;
}

int clamp(int x,int a, int b){
    return x>a ? (x<b ? x : b) : a;
}