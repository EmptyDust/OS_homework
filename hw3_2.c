#include <stdio.h>
#include <stdlib.h>  // Required for system("pause") and exit()
#include <windows.h> // Windows threading and semaphores
#define MAXSTACK 100

int stack[MAXSTACK][2];
int size = 0;
HANDLE s; // Windows semaphore handle

DWORD WINAPI RData1(LPVOID lpParameter)
{
    FILE *fp = fopen("file1.dat", "r");
    if (fp == NULL)
    {
        perror("Error opening file file1.dat");
        exit(EXIT_FAILURE); // Exit if file cannot be opened
    }
    while (fscanf(fp, "%d %d", &stack[size][0], &stack[size][1]) == 2)
    {
        ReleaseSemaphore(s, 1, NULL);
        ++size;
        if (size >= MAXSTACK)
        {
            fprintf(stderr, "Stack overflow in RData1!\n");
            break; // Prevent overflowing the stack
        }
    }
    fclose(fp);
    return 0;
}

DWORD WINAPI RData2(LPVOID lpParameter)
{
    FILE *fp = fopen("file2.dat", "r");
    if (fp == NULL)
    {
        perror("Error opening file file2.dat");
        exit(EXIT_FAILURE); // Exit if file cannot be opened
    }
    while (fscanf(fp, "%d %d", &stack[size][0], &stack[size][1]) == 2)
    {
        ReleaseSemaphore(s, 1, NULL);
        ++size;
        if (size >= MAXSTACK)
        {
            fprintf(stderr, "Stack overflow in RData2!\n");
            break; // Prevent overflowing the stack
        }
    }
    fclose(fp);
    return 0;
}

DWORD WINAPI CData1(LPVOID lpParameter)
{
    int local_size; // introduce local copy to avoid data race
    while (1)
    {
        WaitForSingleObject(s, INFINITE);
        local_size = size; // local copy
        if (local_size <= 0)
        {
            ReleaseSemaphore(s, 1, NULL); // release semaphore if no data available.
            Sleep(10);                    // avoid busy waiting.
            continue;
        }

        printf("Plus:%d+%d=%d\n", stack[local_size - 1][0], stack[local_size - 1][1], stack[local_size - 1][0] + stack[local_size - 1][1]);
        --size;
        Sleep(1000); // 1 second delay
    }
    return 0;
}

DWORD WINAPI CData2(LPVOID lpParameter)
{
    int local_size;
    while (1)
    {
        WaitForSingleObject(s, INFINITE);
        local_size = size;
        if (local_size <= 0)
        {
            ReleaseSemaphore(s, 1, NULL); // release semaphore if no data available.
            Sleep(10);                    // avoid busy waiting.
            continue;
        }

        printf("Multiply:%d*%d=%d\n", stack[local_size - 1][0], stack[local_size - 1][1], stack[local_size - 1][0] * stack[local_size - 1][1]);
        --size;
        Sleep(1000); // 1 second delay
    }
    return 0;
}

int main()
{
    HANDLE tid1, tid2, tid3, tid4;
    DWORD threadID1, threadID2, threadID3, threadID4;

    s = CreateSemaphore(NULL, 0, MAXSTACK, NULL); // Semaphore initialization
    if (s == NULL)
    {
        perror("CreateSemaphore failed");
        return 1;
    }

    // Create 4 threads
    tid1 = CreateThread(NULL, 0, RData1, NULL, 0, &threadID1);
    tid2 = CreateThread(NULL, 0, RData2, NULL, 0, &threadID2);
    tid3 = CreateThread(NULL, 0, CData1, NULL, 0, &threadID3);
    tid4 = CreateThread(NULL, 0, CData2, NULL, 0, &threadID4);

    if (tid1 == NULL || tid2 == NULL || tid3 == NULL || tid4 == NULL)
    {
        perror("CreateThread failed");
        CloseHandle(s);
        return 1;
    }

    // Wait for threads to finish (this will not happen in the original design, since consumers are infinite loops)
    // You may want to limit the execution time or use a signal to stop the threads
    WaitForSingleObject(tid1, INFINITE);
    WaitForSingleObject(tid2, INFINITE);
    Sleep(5000);              // let consumers execute for 5 seconds
    TerminateThread(tid3, 0); // Terminate consumer threads to allow the program to exit.
    TerminateThread(tid4, 0);

    CloseHandle(tid1);
    CloseHandle(tid2);
    CloseHandle(tid3);
    CloseHandle(tid4);
    CloseHandle(s);

    printf("Program finished.\n");
    system("pause"); // Keep the console open until a key is pressed
    return 0;
}