#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAXSTACK 100

int stack[MAXSTACK][2];
int size = 0;

// Semaphores for synchronization
HANDLE dataAvailable;  // 数据存在信号量
HANDLE spaceAvailable; // 缓冲区空间信号量
HANDLE mutex;          // 单次操作不能打断

// Error handling function
void handle_error(const char *message)
{
    fprintf(stderr, "%s Error: %d\n", message, GetLastError());
    exit(EXIT_FAILURE);
}

// Reader thread 1
DWORD WINAPI RData1(LPVOID lpParameter)
{
    FILE *fp = fopen("file1.dat", "r");
    if (fp == NULL)
        handle_error("Error opening file1.dat");

    int data1, data2;
    while (fscanf(fp, "%d %d", &data1, &data2) == 2)
    {
        WaitForSingleObject(spaceAvailable, INFINITE);

        WaitForSingleObject(mutex, INFINITE);

        stack[size][0] = data1;
        stack[size][1] = data2;
        size++;

        ReleaseMutex(mutex);

        ReleaseSemaphore(dataAvailable, 1, NULL);
    }

    fclose(fp);
    return 0;
}

// Reader thread 2
DWORD WINAPI RData2(LPVOID lpParameter)
{
    FILE *fp = fopen("file2.dat", "r");
    if (fp == NULL)
        handle_error("Error opening file2.dat");

    int data1, data2;
    while (fscanf(fp, "%d %d", &data1, &data2) == 2)
    {
        WaitForSingleObject(spaceAvailable, INFINITE);

        WaitForSingleObject(mutex, INFINITE);

        stack[size][0] = data1;
        stack[size][1] = data2;
        size++;

        ReleaseMutex(mutex);

        ReleaseSemaphore(dataAvailable, 1, NULL);
    }

    fclose(fp);
    return 0;
}

// Consumer thread 1 (Addition)
DWORD WINAPI CData1(LPVOID lpParameter)
{
    while (1)
    {
        WaitForSingleObject(dataAvailable, INFINITE);

        WaitForSingleObject(mutex, INFINITE);

        if (size <= 0)
        {
            ReleaseMutex(mutex);
            ReleaseSemaphore(dataAvailable, 1, NULL);
            Sleep(10);
            continue;
        }

        int val1 = stack[size - 1][0];
        int val2 = stack[size - 1][1];
        size--;

        ReleaseMutex(mutex);

        ReleaseSemaphore(spaceAvailable, 1, NULL);

        printf("Plus: %d + %d = %d\n", val1, val2, val1 + val2);
        Sleep(1000);
    }
    return 0;
}

// Consumer thread 2 (Multiplication)
DWORD WINAPI CData2(LPVOID lpParameter)
{
    while (1)
    {
        WaitForSingleObject(dataAvailable, INFINITE);

        WaitForSingleObject(mutex, INFINITE);

        if (size <= 0)
        {
            ReleaseMutex(mutex);
            ReleaseSemaphore(dataAvailable, 1, NULL);
            Sleep(10);
            continue;
        }

        int val1 = stack[size - 1][0];
        int val2 = stack[size - 1][1];
        size--;

        ReleaseMutex(mutex);

        ReleaseSemaphore(spaceAvailable, 1, NULL);

        printf("Multiply: %d * %d = %d\n", val1, val2, val1 * val2);
        Sleep(1000);
    }
    return 0;
}

int main()
{
    HANDLE tid1, tid2, tid3, tid4;
    DWORD threadID1, threadID2, threadID3, threadID4;

    // Initialize semaphores
    dataAvailable = CreateSemaphore(NULL, 0, MAXSTACK, NULL);         // Initially empty
    spaceAvailable = CreateSemaphore(NULL, MAXSTACK, MAXSTACK, NULL); // Initially full
    mutex = CreateMutex(NULL, FALSE, NULL);                           // Initially unlocked

    if (dataAvailable == NULL || spaceAvailable == NULL || mutex == NULL)
    {
        handle_error("Failed to create semaphore/mutex");
    }

    // Create threads
    tid1 = CreateThread(NULL, 0, RData1, NULL, 0, &threadID1);
    tid2 = CreateThread(NULL, 0, RData2, NULL, 0, &threadID2);
    tid3 = CreateThread(NULL, 0, CData1, NULL, 0, &threadID3);
    tid4 = CreateThread(NULL, 0, CData2, NULL, 0, &threadID4);

    if (tid1 == NULL || tid2 == NULL || tid3 == NULL || tid4 == NULL)
    {
        handle_error("Failed to create thread");
    }

    // Let the threads run for a bit (example: 5 seconds)
    Sleep(5000);

    // Clean up and exit (important to terminate the consumer threads, as they are infinite loops)
    TerminateThread(tid1, 0);
    TerminateThread(tid2, 0);
    TerminateThread(tid3, 0);
    TerminateThread(tid4, 0);

    CloseHandle(tid1);
    CloseHandle(tid2);
    CloseHandle(tid3);
    CloseHandle(tid4);
    CloseHandle(dataAvailable);
    CloseHandle(spaceAvailable);
    CloseHandle(mutex);

    printf("Program finished.\n");
    system("pause");
    return 0;
}