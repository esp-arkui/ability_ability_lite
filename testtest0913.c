部 
文件类型 
文件变更 
评论 
0/1 个文件已阅
 +29 test0913.c 
int main()
{
    int d1, d2, c1, c2, i, j;
    c1 = c2 = 0;  /*初始化*/
    srand((unsigned)time(NULL));
    rand();  /*初始化随机数产生器*/
    for(i=1; i<=100; i++)
    {  /*模拟游戏过程*/
        d1 = d2 = 0;
        for(j=1; j<=6; j++)
        {  /*两个人轮流掷骰子*/
            d1 = d1+rand()+1;
            d2 = d2+rand()+1;
        }
        if(d1>d2)
            c1++;  /*累加获胜盘数*/
        else if(d1<d2)
            c2++;
    }
    if(c1>c2)  /*输出最终获胜者信息*/
        printf("\nThe first win.\n");
    else
        if (c1<c2)
            printf("\nThe second win.\n");
        else
            printf("They tie.\n");
   
    return 0;
}