#include "include/threads.h"

int _tmain(int argc, TCHAR *argv[]) {
    HANDLE *hThreadArray = NULL;
    PDATA pData_array = NULL;
    PSHAREDDATA pSharedData = NULL;

    if (argc == 2 && ((strcmp(*(argv + 1), "--help") == 0) || (strcmp(*(argv + 1), "-h") == 0))) {
        display_help();
        return ERROR_INPUT_ARGS;
    } else if (argc == 1 || argc > 3) {
        std::cout << "Incorrect number of arguments, see '--help or -h'" << std::endl;
        return ERROR_INPUT_ARGS;
    } else if (atoi(*(argv + 1)) < 3 || atoi(*(argv + 1)) > 100 ||
               (strcmp(*(argv + 2), "inc") != 0 && strcmp(*(argv + 2), "dec") != 0)) {
        std::cout << "Incorrect arguments, see '--help or -h'" << std::endl;
        return ERROR_INPUT_ARGS;
    } else {
        if (AllocateMemory(&hThreadArray, &pData_array, &pSharedData, argv) == ERROR_ALLOCATION_MEMORY) {
            return ERROR_ALLOCATION_MEMORY;
        }
        if ((strcmp(*(argv + 2), "inc") == 0)) {
            pSharedData->order_of_finishing_threads = inc;
        } else if ((strcmp(*(argv + 2), "dec") == 0)) {
            pSharedData->order_of_finishing_threads = dec;
        }
        pSharedData->number_of_threads = atoi(*(argv + 1));
    }

    CreateThreads(hThreadArray, pData_array, pSharedData);
    FinishThreads(hThreadArray, pSharedData);
    DeallocateMemory(&hThreadArray, &pData_array, &pSharedData);
    return SUCCESS;
}