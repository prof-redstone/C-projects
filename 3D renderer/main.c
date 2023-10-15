#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <windows.h>


#define PI 3.1415926535
#define seed 185478

void collision(float* px, float* py, float* lpx, float* lpy, int* map, int Msize);
void genMase(int* mase, int size);
void genMase2(int* mase, int size);
float* resetScreen(int Xsize, int Ysize, int renderType);
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
    int renderType = 2;

    int Msize = 11;
    int map[Msize*Msize];
    genMase(map, Msize);
    map[0] = 0;

    // int map[] = {
    //             1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    //             1,0,0,0,0,0,0,1,0,0,0,0,0,1,
    //             1,0,0,0,0,0,0,1,0,0,0,0,0,1,
    //             1,0,0,0,0,0,1,1,1,0,0,0,0,1,
    //             1,0,1,1,0,0,0,0,0,0,0,0,0,1,
    //             1,0,0,1,0,0,0,0,0,0,0,0,0,1,
    //             1,0,0,1,0,0,1,0,0,0,0,0,0,1,
    //             1,0,0,0,0,0,1,0,0,0,0,0,0,1,
    //             1,0,0,0,0,0,1,0,0,0,0,0,0,1,
    //             1,0,0,1,0,0,1,1,1,0,0,1,1,1,
    //             1,0,0,1,0,0,1,0,0,0,0,0,0,1,
    //             1,0,0,0,0,0,1,0,0,0,0,0,0,1,
    //             1,0,0,0,0,0,1,0,0,0,0,0,0,1,
    //             1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    //             };

    float pos[] = {0.5,0.5};
    float lastpos[] = {0.5,0.5};
    float angle = 0;
    float delta = 0.2;
    float speed = 0.15*delta;
    float rotSpeed = 0.15*delta;

    float* arrayImage = resetScreen(Xsize, Ysize, renderType);

    
    while (1) {

        if(GetAsyncKeyState(65)){
            Xsize += 1;
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize, renderType);
        }

        if(GetAsyncKeyState(81)){
            Xsize -= 1;
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize, renderType);
        }

        if(GetAsyncKeyState(90)){
            Ysize += 1;
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize, renderType);
        }

        if(GetAsyncKeyState(83)){
            Ysize -= 1;
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize, renderType);
        }

        if(GetAsyncKeyState(80)){//p     
            renderType = 2;      
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize, renderType);
        }
        if(GetAsyncKeyState(79)){//o
            renderType = 1;          
            free(arrayImage);
            arrayImage = resetScreen(Xsize,Ysize, renderType);
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

        //collision
        collision(&pos[0],&pos[1],&lastpos[0],&lastpos[1],map, Msize);

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
        if(renderType == 3){
            printf("\n\n\n\n");
            for (int i = 0; i < Msize; i++)
            {
                printf("#");
                for (int j = 0; j < Msize; j++)
                {
                    if(map[i*Msize + j] == 0){
                        printf(" ");
                    }else{
                        printf("%d",map[i*Msize + j]);
                    }
                }printf("#");
                printf("\n");
            }
            
        }
        if(renderType == 4){}

        time++;
        usleep(freq);
    }

    free(arrayImage);
    return;
}

void collision(float* px, float* py, float* lpx, float* lpy, int* map, int Msize){
    if(0 <= (int)floor(*px) && (int)floor(*px) < Msize && 0 <= (int)floor(*py) && (int)floor(*py) < Msize){
        if(map[((int)floor(*py))*Msize + ((int)floor(*px))] != 1){
            *lpx = *px;
            *lpy = *py;
            return;
        }
    }
    *px = *lpx;
    *py = *lpy;
    return;
}


void genMase(int* map, int size){
    srand(seed);
    int* mase = malloc(sizeof(int)*size*size);
    int counter = 1;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            mase[i*size + j] = (i%2==0&&j%2==0) ? counter++ : -1;
            map[i*size + j] = (i%2==0&&j%2==0) ? 0 : 1;
        }
    }

    while (mase[size*size -1] != mase[0]){
        int x = rand()%(size);
        int y;
        if(x%2 == 0){
            y = (rand()%(size/2))*2 +1;
        }else{
            y = (rand()%(size/2 +1 ))*2;
        }
        printf("%d %d\n",x,y);
        int val1;
        int val2;


        if(y%2==1){
            val1 = mase[(y-1)*size + x];
            val2 = mase[(y+1)*size + x];
        }else{
            val1 = mase[y*size + x-1];
            val2 = mase[y*size + x+1];
        }

        if(val1 != val2){
            map[y*size + x] = 0;
            mase[y*size + x] = val1;
            for (int i = 0; i < size; i++){
                for (int j = 0; j < size; j++){
                    if(mase[i*size + j] == val2){
                        mase[i*size + j] = val1;
                    }
                }
            }
        }
    }
    free(mase);
}

void genMase2(int* map, int size){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            if(i%2 == 0 && j%2 ==0){
                map[i*size + j] = 0;
            }else{
                map[i*size + j] = 1;
            }
        }
    }

    int px = 0;
    int py = 0;
    int counter = 1;
    int Nsize = size/2;
    int* visited = malloc(sizeof(int)*Nsize*Nsize); // tableau qui compte la distance au depart
    int* caseCount = malloc(sizeof(int)*Nsize*Nsize); // liste des cases depuis de depart
    for (int i = 0; i < size*size/4; i++){
        visited[i] = -1;
        caseCount[-1] = -1;
    }
    visited[0] = counter;
    caseCount[0] = 0;

    while (counter > 0){
        int vois[] = {-1,-1,-1,-1};
        if(py>0 && visited[(py-1)*Nsize + px]==-1){vois[0]= px+py - Nsize;}
        if(py<Nsize-1 && visited[(py+1)*Nsize + px]==-1){vois[1]= px+py + Nsize;}
        if(px>0 && visited[py*Nsize + (px-1)]==-1){vois[2]= px+py - 1;}
        if(px<Nsize-1 && visited[py*Nsize + (px-1)]==-1){vois[3]= px+py + 1;}
        int ash = (counter*157 + px*139 + py*794 + seed*113)%4;
        int move = -1;
        for (int i = 0; i < 4; i++){
            if(move == -1 && vois[ash] == -1){
                ash = (ash+1)%4;
            }else{
                move = ash;
            }
        }
        printf("%d %d\n", py, px);
        if(move == 0){
            py -= 1;
            caseCount[counter] = py*Nsize + px;
            visited[py*Nsize + px] = counter+1;
            map[(py*2 +1)*Nsize + px*2] = 0;
            counter++;
        }
        if(move == 1){
            py += 1;
            caseCount[counter] = py*Nsize + px;
            visited[py*Nsize + px] = counter+1;
            map[(py*2 -1)*Nsize + px*2] = 0;
            counter++;
        }
        if(move == 2){
            px -= 1;
            caseCount[counter] = py*Nsize + px;
            visited[py*Nsize + px] = counter+1;
            map[(py*2)*Nsize + (px+1)*2] = 0;
            counter++;
        }
        if(move == 3){
            px += 1;
            caseCount[counter] = py*Nsize + px;
            visited[py*Nsize + px] = counter+1;
            map[(py*2)*Nsize + (px-1)*2] = 0;
            counter++;
        }
        if(move == -1){
            px = caseCount[counter-1]%Nsize;
            py = caseCount[counter-1]/Nsize;
            counter--;
        }
    }

    printf("\n\n\n\n");
    for (int i = 0; i < Nsize; i++)
    {
        for (int j = 0; j < Nsize; j++)
        {
            printf("%d  ",map[i*Nsize + j]);
        }
        printf("\n");
    }
    
    free(visited);
    free(caseCount);
    return;    
}

float* resetScreen(int Xsize, int Ysize, int renderType){
    if(renderType == 1 ){
        printf("\x1b[2J");//for the renderer
    }
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
        }else{
            found = 1;
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