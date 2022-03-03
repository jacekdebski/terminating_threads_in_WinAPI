#include "../include/threads.h"

int AllocateMemory(HANDLE **hThreadArray, PDATA *pData_array, PSHAREDDATA *pSharedData, TCHAR *argv[]) {
    *pSharedData = (PSHAREDDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SHAREDDATA));
    if (*pSharedData == NULL) {
        DeallocateMemory(hThreadArray, pData_array, pSharedData);
        return ERROR_ALLOCATION_MEMORY;
    } else {
        (*pSharedData)->dwThreadIdArray = NULL;
    }
    (*pSharedData)->number_of_threads = atoi(*(argv + 1));
    (*pSharedData)->dwThreadIdArray = (DWORD *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                                          (*pSharedData)->number_of_threads * sizeof(DWORD));
    if ((*pSharedData)->dwThreadIdArray == NULL) {
        DeallocateMemory(hThreadArray, pData_array, pSharedData);
        return ERROR_ALLOCATION_MEMORY;
    }
    *pData_array = (PDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (*pSharedData)->number_of_threads *
                                                                         sizeof(DATA));
    if (*pData_array == NULL) {
        DeallocateMemory(hThreadArray, pData_array, pSharedData);
        return ERROR_ALLOCATION_MEMORY;
    }
    *hThreadArray = (HANDLE *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (*pSharedData)->number_of_threads *
                                                                             sizeof(HANDLE));
    if (*hThreadArray == NULL) {
        DeallocateMemory(hThreadArray, pData_array, pSharedData);
        return ERROR_ALLOCATION_MEMORY;
    }
    return SUCCESS;
}

void DeallocateMemory(HANDLE **hThreadArray, PDATA *pData_array, PSHAREDDATA *pSharedData) {
    if (*hThreadArray != NULL) {
        HeapFree(GetProcessHeap(), 0, *hThreadArray);
        hThreadArray = NULL;
    }
    if (*pData_array != NULL) {
        HeapFree(GetProcessHeap(), 0, *pData_array);
        pData_array = NULL;
    }
    if (*pSharedData != NULL) {
        if ((*pSharedData)->dwThreadIdArray != NULL) {
            HeapFree(GetProcessHeap(), 0, (*pSharedData)->dwThreadIdArray);
            (*pSharedData)->dwThreadIdArray = NULL;
        }
        HeapFree(GetProcessHeap(), 0, *pSharedData);
        pSharedData = NULL;
    }
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
    PDATA pThreadData;
    DWORD dwCurrentThreadID;

    pThreadData = (PDATA) lpParam;
    dwCurrentThreadID = GetCurrentThreadId();

    lock(pThreadData);
    std::cout << "Thread with ThreadID: " << dwCurrentThreadID << " is finishing. ("
              << pThreadData->dwThreadIDArray_index << ")" << std::endl;
    return 0;
}

void lock(PDATA pThreadData) {
    if (pThreadData->pSharedData->order_of_finishing_threads == dec) {
        for (int j = pThreadData->pSharedData->number_of_threads - 1; j > pThreadData->dwThreadIDArray_index; --j) {
            while (*(pThreadData->pSharedData->dwThreadIdArray + j) != 0) {
                Sleep(1000);
            }
        }
    } else if (pThreadData->pSharedData->order_of_finishing_threads == inc) {
        for (int j = 0; j < pThreadData->dwThreadIDArray_index; ++j) {
            while (*(pThreadData->pSharedData->dwThreadIdArray + j) != 0) {
                Sleep(1000);
            }
        }
    }
}

void unlock(PSHAREDDATA pSharedData, int dwThreadID_index) {
    *(pSharedData->dwThreadIdArray + dwThreadID_index) = 0;
}

void CreateThreads(HANDLE *hThreadArray, PDATA pData_array, PSHAREDDATA pSharedData) {
    for (int i = 0; i < pSharedData->number_of_threads; i++) {
        (pData_array + i)->pSharedData = pSharedData;
        (pData_array + i)->dwThreadIDArray_index = i;
        *(hThreadArray + i) = CreateThread(
                NULL,
                0,
                MyThreadFunction,
                pData_array + i,
                CREATE_SUSPENDED,
                pSharedData->dwThreadIdArray + i);
        if (*(hThreadArray + i) == NULL) {
            std::cout << "Thread creation failed" << std::endl;
            DeallocateMemory(&hThreadArray, &pData_array, &pSharedData);
            ExitProcess(3);
        } else {
            std::cout << "Thread has created with ThreadID: "
                      << *(pSharedData->dwThreadIdArray + (pData_array + i)->dwThreadIDArray_index) << " ("
                      << (pData_array + i)->dwThreadIDArray_index << ")" << std::endl;
        }
    }
    for (int i = 0; i < pSharedData->number_of_threads; i++) {
        ResumeThread(*(hThreadArray + i));
    }
}

void FinishThreads(HANDLE *hThreadArray, PSHAREDDATA pSharedData) {
    DWORD temp_dwThreadID;
    if (pSharedData->order_of_finishing_threads == dec) {
        for (int i = pSharedData->number_of_threads - 1; i >= 0; --i) {
            temp_dwThreadID = *(pSharedData->dwThreadIdArray + i);
            WaitForSingleObject(*(hThreadArray + i), INFINITE);
            unlock(pSharedData, i);
            std::cout << "Thread with ThreadID: " << temp_dwThreadID << " has finished execution. (" << i << ")"
                      << std::endl;
            CloseHandle(*(hThreadArray + i));
        }
    } else if (pSharedData->order_of_finishing_threads == inc) {
        for (int i = 0; i < pSharedData->number_of_threads; ++i) {
            temp_dwThreadID = *(pSharedData->dwThreadIdArray + i);
            WaitForSingleObject(*(hThreadArray + i), INFINITE);
            unlock(pSharedData, i);
            std::cout << "Thread with ThreadID: " << temp_dwThreadID << " has finished execution. (" << i << ")"
                      << std::endl;
            CloseHandle(*(hThreadArray + i));
        }
    }
}

void display_help() {
    std::cout << "First argument: number of threads (3-100)" << std::endl;
    std::cout << "Second argument: order of finishing threads (inc, dec)" << std::endl;
}