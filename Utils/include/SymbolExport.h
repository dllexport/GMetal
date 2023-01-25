//
// Created by Mario on 2023/1/18.
//
#pragma once

#ifdef _WIN32
    #if GMETAL_BUILD_DLL
        #define DLLEXPORT __declspec(dllexport)
    #else
        #define DLLEXPORT
    #endif
#else
    #define DLLEXPORT
#endif
