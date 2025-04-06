#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int x = 0;
int y = 0;
int z = 0;

HANDLE S1, S2;

DWORD WINAPI thread_T1(LPVOID arg)
{
    y = 1;
    y = y + 2;
    printf("T1: y = %d\n", y);
    ReleaseSemaphore(S1, 1, NULL);
    z = y + 1;
    printf("T1: z = %d\n", z);
    Sleep(1000);
    WaitForSingleObject(S2, INFINITE);
    y = z + x;
    printf("T1: y (final) = %d\n", y);
    return 0;
}

DWORD WINAPI thread_T2(LPVOID arg)
{
    x = 1;
    x = x + 1;
    printf("T2: x = %d\n", x);
    WaitForSingleObject(S1, INFINITE);
    x = x + y;
    printf("T2: x (after y) = %d\n", x);
    Sleep(1000);
    ReleaseSemaphore(S2, 1, NULL);
    return 0;
}

int main()
{
    HANDLE tid1, tid2;
    DWORD threadID1, threadID2;

    S1 = CreateSemaphore(NULL, 0, 1, NULL);
    S2 = CreateSemaphore(NULL, 0, 1, NULL);

    if (S1 == NULL || S2 == NULL)
    {
        fprintf(stderr, "CreateSemaphore failed with error %d\n", GetLastError());
        return 1;
    }

    tid1 = CreateThread(NULL, 0, thread_T1, NULL, 0, &threadID1);
    tid2 = CreateThread(NULL, 0, thread_T2, NULL, 0, &threadID2);

    if (tid1 == NULL || tid2 == NULL)
    {
        fprintf(stderr, "CreateThread failed with error %d\n", GetLastError());
        CloseHandle(S1);
        CloseHandle(S2);
        return 1;
    }

    WaitForSingleObject(tid1, INFINITE);
    WaitForSingleObject(tid2, INFINITE);

    printf("Final values: x = %d, y = %d, z = %d\n", x, y, z);

    CloseHandle(tid1);
    CloseHandle(tid2);
    CloseHandle(S1);
    CloseHandle(S2);

    printf("Press any key to exit...\n");
    getchar();
    return 0;
}