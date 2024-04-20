//
// Created by Ludw on 4/19/2024.
//

#include "util.h"



char check_pointer(void *ptr) {
    char *char_ptr = (char *) ptr;

    printf("pointer address: %p\n", char_ptr);
    printf("value at address: %d\n", *char_ptr);
    return 1;
}

/*
void load_rdoc() {
    RDOC_API = NULL;

    HMODULE mod = GetModuleHandleA("renderdoc.dll");
    if(mod != NULL)
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI =
                (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&RDOC_API);
        assert(ret == 1);
    }
}
*/