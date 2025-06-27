/*
 * Owllab License Agreement
 *
 * This software is provided by Owllab and may not be used, copied, modified,
 * merged, published, distributed, sublicensed, or sold without a valid and
 * explicit agreement with Owllab.
 *
 * Copyright (c) 2025 Owllab. All rights reserved.
 */

#pragma once

#include "ravennakit/core/platform.hpp"
#include "ravennakit/core/log.hpp"

#if RAV_WINDOWS

    #include <windows.h>
    #include <avrt.h>
    #pragma comment(lib, "Avrt.lib")

namespace rav {

struct WindowsThreadCharacteristics {
    DWORD task_index = 0;
    HANDLE h_task = nullptr;

    WindowsThreadCharacteristics(const LPCSTR task_name = TEXT("Pro Audio")) {
        h_task = AvSetMmThreadCharacteristics(task_name, &task_index);
        if (h_task == nullptr) {
            RAV_ERROR("Failed to set thread priority");
        }
    }

    ~WindowsThreadCharacteristics() {
        if (h_task != nullptr) {
            AvRevertMmThreadCharacteristics(h_task);
        }
    }
};

}

#endif
