#include<stdio.h>
 
int main()
{
    int i,j;
    for(i=0;i<8;i++)
    {
        for(j=0;j<8;j++)
            if((i+j)%2==0)
                printf("%c%c",219,219);
            else printf("  ");
        printf("\n");
    }
    return 0;
}

#include "graphics.h"
int main()
{
    int x0,y0,y1,x1,driver,mode,i;
    driver=VGA;mode=VGAHI;
    initgraph(&driver,&mode,"");
    setbkcolor(YELLOW);
    x0=263;y0=263;y1=275;x1=275;
    for(i=0;i<=18;i++)
    {
        setcolor(1);
        rectangle(x0,y0,x1,y1);
        x0=x0-5;
        y0=y0-5;
        x1=x1+5;
        y1=y1+5;
    }
    settextstyle(DEFAULT_FONT,HORIZ_DIR,2);
    outtextxy(150,40,"How beautiful it is!");
    line(130,60,480,60);
    setcolor(2);
    circle(269,269,137);
}

#include <stdio.h>
#include <math.h>
 
float calculateSD(float data[]);
 
int main()
{
    int i;
    float data[10];
 
    printf("输入10个元素: ");
    for(i=0; i < 10; ++i)
        scanf("%f", &data[i]);
 
    printf("\n标准偏差 = %.6f", calculateSD(data));
 
    return 0;
}
 
float calculateSD(float data[])
{
    float sum = 0.0, mean, standardDeviation = 0.0;
 
    int i;
 
    for(i=0; i<10; ++i)
    {
        sum += data[i];
    }
 
    mean = sum/10;
 
    for(i=0; i<10; ++i)
        standardDeviation += pow(data[i] - mean, 2);
 
    return sqrt(standardDeviation/10);
}