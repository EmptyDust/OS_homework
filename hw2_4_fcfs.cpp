#include <stdio.h>
#include <stdlib.h>

#define getpch(type) (type *)malloc(sizeof(type))
#define NOTHING 0

typedef struct pcb
{                  // è¿ç¨ç®¡çå
    char name[10]; // è¿ç¨åå­
    char state;    // è¿ç¨ç¶æ
    int ntime;     // è¿ç¨éè¦è¿è¡çæ¶é´
    int rtime;     // è¿ç¨å·²ç»è¿è¡çæ¶é´
    struct pcb *link;
} PCB;

PCB *ready = NOTHING, *pfend = NOTHING, *p = NOTHING; // å°±ç»ªéåï¼è¿ç¨æå¥ä½ç½®çåé

int geti() // ä½¿ç¨æ·ä»è½è¾å¥æ´æ°
{
    char ch;
    int i = 0;
    fflush(stdin);
    ch = getchar();

    while (ch == '\n')
    {
        // printf("\tfè¾å¥ä¸è½ä¸ºç©º..è¯·éæ°è¾å¥\n");
        fflush(stdin);
        ch = getchar();
    }

    while (ch != '\n')
    {
        if (ch > '9' || ch < '0')
        {
            printf("\tè¾å¥æè¯¯!!è¾å¥åªè½ä¸ºæ­£æ´æ°ï¼è¯·éæ°è¾å¥...\n");
            fflush(stdin);
            i = 0;
            ch = getchar();
        }
        else
        {
            i = i * 10 + (ch - '0');
            ch = getchar();
        }
    }
    return i;
}

void fcfs() // æå¥è¿ç¨
{
    if (!ready)
    {
        ready = pfend = p;
        // å¾è¡¥å¨
    }
    else
    {
        pfend = pfend->link = p;
        // å¾è¡¥å¨
    }
}

void input() /*å»ºç«è¿ç¨æ§å¶åå½æ°*/
{
    int i, num;
    printf("\nè¯·è¾å¥è¿ç¨çä¸ªæ°?\n");
    num = geti();
    for (i = 0; i < num; i++)
    {
        printf("\nè¿ç¨å·No.%d:\n", i + 1);
        p = getpch(PCB);
        printf("\nè¾å¥è¿ç¨å:");
        scanf("%s", p->name);
        printf("\nè¾å¥è¿ç¨è¿è¡æ¶é´:");
        p->ntime = geti();
        printf("\n");
        p->rtime = 0;
        p->state = 'w';
        p->link = NOTHING;
        fcfs();
    }
}

void disp(PCB *pr) /*å»ºç«è¿ç¨ç°å®å½æ°ï¼ç¨äºæ¾ç¤ºå½åè¿ç¨*/
{
    printf("\nname\t state\t ntime\t rtime\t \n");
    printf("|%s\t", pr->name);
    printf(" |%c\t", pr->state);
    printf(" |%d\t", pr->ntime);
    printf(" |%d\t", pr->rtime);
    printf("\n");
}

void check() /*å»ºç«è¿ç¨æ¥çå½æ°*/
{
    PCB *pr;
    printf("\n ****å½åæ­£å¨è¿è¡çè¿ç¨æ¯:%s", ready->name); /*æ¾ç¤ºå½åè¿è¡çè¿ç¨*/
    disp(ready);
    pr = ready->link;
    printf("\n****å½åå°±ç»ªéåç¶æä¸º:\n"); /*æ¾ç¤ºå°±ç»ªéåç¶æ*/
    while (pr != NOTHING)
    {
        disp(pr);
        pr = pr->link;
    }
}

void destroy() /*å»ºç«è¿ç¨æ¤éå½æ°(è¿ç¨è¿è¡ç»æï¼æ¤éè¿ç¨)*/
{
    printf("\nè¿ç¨[%s]å·²å®æ.\n", ready->name);
    p = ready;
    ready = ready->link;
    free(p);
}

void running() /*å»ºç«è¿ç¨å°±ç»ªå½æ°(è¿ç¨è¿è¡æ¶é´å°ï¼ç½®å°±ç»ªç¶æ)*/
{
    (ready->rtime)++;
    check();
    if (ready->rtime == ready->ntime)
    {
        destroy();
        return;
    }
}

void main()
{
    char ch;
    input();
    while (ready != NOTHING)
    {
        printf("\nThe execute name:%s\n", ready->name);
        ready->state = 'R';
        // check();
        running();
        printf("\næié®æ·»å æ°è¿ç¨....æå¶ä»ä»»æé®ç»§ç»­è¿è¡...");
        fflush(stdin);
        ch = getchar();
        if (ch == 'i' || ch == 'I')
            input();
    }
    printf("\n\n è¿ç¨å·²ç»å®æ\n");
    getchar();
}
