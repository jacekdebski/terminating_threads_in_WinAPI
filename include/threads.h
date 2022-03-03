#ifndef TERMINATING_THREADS_IN_WINAPI_H
#define TERMINATING_THREADS_IN_WINAPI_H

#include <windows.h>
#include <tchar.h>
#include <iostream>

#define ERROR_ALLOCATION_MEMORY -1
#define ERROR_INPUT_ARGS -2
#define SUCCESS 0

enum order_of_finishing_threads {
    inc,
    dec
};

typedef struct SharedData {
    DWORD *dwThreadIdArray;
    int number_of_threads;
    enum order_of_finishing_threads order_of_finishing_threads;
} SHAREDDATA, *PSHAREDDATA;

typedef struct Data {
    PSHAREDDATA pSharedData;
    int dwThreadIDArray_index;
} DATA, *PDATA;

int AllocateMemory(HANDLE **hThreadArray, PDATA *pData_array, PSHAREDDATA *pSharedData, TCHAR *argv[]);

void DeallocateMemory(HANDLE **hThreadArray, PDATA *pData_array, PSHAREDDATA *pSharedData);

DWORD WINAPI MyThreadFunction(LPVOID lpParam);

void lock(PDATA pThreadData);

void unlock(PSHAREDDATA pSharedData, int dwThreadID_index);

void CreateThreads(HANDLE *hThreadArray, PDATA pData_array, PSHAREDDATA pSharedData);

void FinishThreads(HANDLE *hThreadArray, PSHAREDDATA pSharedData);

void display_help();

#endif //TERMINATING_THREADS_IN_WINAPI_H
