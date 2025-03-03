#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#ifndef BEE_PUT
#define BEE_PUT(n) printf("\nBEE_PUT %d!", n)
#endif
#ifndef BEE_WAKE
#define BEE_WAKE(n) printf("\nBEE_WAKE %d!", n)
#endif
#ifndef BEAR_EAT
#define BEAR_EAT() printf("\nBEAR_EAT!")
#endif

int N, H, P, E, C;

int honeyCount = 0;
int jarsCompleted = 0;

// синхронизация
CRITICAL_SECTION cs;
CONDITION_VARIABLE jarFullCV;
CONDITION_VARIABLE jarEmptyCV;

// поток пчелы
DWORD WINAPI BeeThread(LPVOID lpParam) {
    int beeId = *(int*)lpParam;
    free(lpParam);

    while (1) {
        EnterCriticalSection(&cs);

        if (jarsCompleted >= C) {
            LeaveCriticalSection(&cs);
            break;
        }

        while (honeyCount >= H) {
            if (jarsCompleted >= C) {
                LeaveCriticalSection(&cs);
                return 0;
            }
            SleepConditionVariableCS(&jarEmptyCV, &cs, INFINITE);
        }
        if (jarsCompleted >= C) {
            LeaveCriticalSection(&cs);
            break;
        }

        honeyCount++;
        int myCount = honeyCount;
        LeaveCriticalSection(&cs);

        BEE_PUT(beeId);
        if (P > 0)
            Sleep(P);

        if (myCount == H) {
            BEE_WAKE(beeId);
            EnterCriticalSection(&cs);
            WakeConditionVariable(&jarFullCV);
            LeaveCriticalSection(&cs);
        }
    }
    return 0;
}

// поток медведя
DWORD WINAPI BearThread(LPVOID lpParam) {
    (void)lpParam;
    while (1) {
        EnterCriticalSection(&cs);
        while (honeyCount < H) {
            if (jarsCompleted >= C) {
                LeaveCriticalSection(&cs);
                return 0;
            }
            SleepConditionVariableCS(&jarFullCV, &cs, INFINITE);
        }
        int portions = honeyCount;
        honeyCount = 0;
        jarsCompleted++;
        WakeAllConditionVariable(&jarEmptyCV);
        LeaveCriticalSection(&cs);

        for (int i = 0; i < portions; i++) {
            BEAR_EAT();
            if (E > 0)
                Sleep(E);
        }
        if (jarsCompleted >= C)
            break;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s N H P E C\n", argv[0]);
        return 1;
    }
    N = atoi(argv[1]); // количество пчел
    H = atoi(argv[2]); // вместимость горшочка (порции мёда)
    P = atoi(argv[3]); // время пчелы на внесение одной порции (мс)
    E = atoi(argv[4]); // время медведя на поедание одной порции (мс)
    C = atoi(argv[5]); // число итераций (сколько горшков должны быть наполнены)

    InitializeCriticalSection(&cs);
    InitializeConditionVariable(&jarFullCV);
    InitializeConditionVariable(&jarEmptyCV);

    HANDLE *beeThreads = malloc(N * sizeof(HANDLE));
    if (!beeThreads) {
        fprintf(stderr, "Ошибка выделения памяти для пчел\n");
        return 1;
    }
    for (int i = 0; i < N; i++) {
        int *id = malloc(sizeof(int));
        if (!id) {
            fprintf(stderr, "Ошибка выделения памяти для инициализации пчел\n");
            return 1;
        }
        *id = i + 1;
        beeThreads[i] = CreateThread(NULL, 0, BeeThread, id, 0, NULL);
        if (beeThreads[i] == NULL) {
            fprintf(stderr, "Ошибка создания потока пчелы\n");
            return 1;
        }
    }
    HANDLE bearThread = CreateThread(NULL, 0, BearThread, NULL, 0, NULL);
    if (bearThread == NULL) {
        fprintf(stderr, "Ошибка создания потока медведя\n");
        return 1;
    }

    WaitForMultipleObjects(N, beeThreads, TRUE, INFINITE);
    WaitForSingleObject(bearThread, INFINITE);

    for (int i = 0; i < N; i++) {
        CloseHandle(beeThreads[i]);
    }
    free(beeThreads);
    CloseHandle(bearThread);
    DeleteCriticalSection(&cs);

    return 0;
}
