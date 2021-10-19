#pragma once

#if defined(WIN32) || defined(_WIN32)
#define EVENTLOG_DLLEXPORT extern "C" __declspec(dllexport)
#else
#define EVENTLOG_DLLEXPORT extern "C" __attribute__((visibility("default")))
#endif

#include "pch.h"

#define SOURCE_NAME _T("ГШ Покров")

extern "C" EVENTLOG_DLLEXPORT WORD install_eventlog_source(
    const char* a_name, const char* a_exe_path);

extern "C" EVENTLOG_DLLEXPORT WORD send_eventlog_message(
    const char* a_msg, const WORD a_type, const char* a_name);

extern "C" EVENTLOG_DLLEXPORT WORD uninstall_eventlog_source(
    const char* a_name);
