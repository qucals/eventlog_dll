#include "pch.h"
#include "eventlog.h"

std::string cp1251_to_utf8(const char* str) {
    std::string res;
    int result_u, result_c;
    result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);

    if (!result_u) { return ""; }

    wchar_t* ures = new wchar_t[result_u];
    if (!MultiByteToWideChar(1251, 0, str, -1, ures, result_u)) {
        delete[] ures;
        return 0;
    }

    result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
    if (!result_c) {
        delete[] ures;
        return 0;
    }

    char* cres = new char[result_c];
    if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0)) {
        delete[] cres;
        return 0;
    }

    delete[] ures;
    res.append(cres);
    delete[] cres;

    return res;
}

std::wstring utf8_to_utf16(const std::string a_str)
{
    std::string utf8_str = cp1251_to_utf8(a_str.c_str());
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(utf8_str);
}

WORD install_eventlog_source(
    const char* a_name, const char* a_exe_path)
{
    std::wstring w_name = utf8_to_utf16(std::string(a_name));
    std::wstring w_exe_path = utf8_to_utf16(std::string(a_exe_path));

    const std::wstring key_path(L"SYSTEM\\CurrentControlSet\\Services\\"
        "EventLog\\ГШ Покров\\" + w_name);

    HKEY key;
    DWORD last_error = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
        key_path.c_str(),
        0,
        0,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        0,
        &key,
        0);

    if (ERROR_SUCCESS == last_error) {
        BYTE* exe_path =(BYTE*)w_exe_path.c_str();
        DWORD last_error;
        const DWORD types_supported = EVENTLOG_ERROR_TYPE |
            EVENTLOG_WARNING_TYPE |
            EVENTLOG_INFORMATION_TYPE;

        last_error = RegSetValueEx(key,
            L"EventMessageFile",
            0,
            REG_SZ,
            exe_path,
            sizeof(exe_path));

        if (ERROR_SUCCESS == last_error) {
            last_error = RegSetValueEx(key,
                L"TypesSupported",
                0,
                REG_DWORD,
                (LPBYTE)&types_supported,
                sizeof(types_supported));
        }

        if (ERROR_SUCCESS != last_error) {
            return last_error;
        }

        RegCloseKey(key);
    } else {
        return last_error;
    }

    return last_error;
}

WORD send_eventlog_message(
    const char* a_msg, const WORD a_type, const char* a_name)
{
    std::wstring w_msg = utf8_to_utf16(std::string(a_msg));
    std::wstring w_name = utf8_to_utf16(std::string(a_name));

    DWORD event_id;

    switch (a_type)
    {
    case EVENTLOG_ERROR_TYPE:
        event_id = MSG_ERROR_1;
        break;
    case EVENTLOG_WARNING_TYPE:
        event_id = MSG_WARNING_1;
        break;
    case EVENTLOG_INFORMATION_TYPE:
        event_id = MSG_INFO_1;
        break;
    default:
        event_id = MSG_INFO_1;
        break;
    }

    HANDLE h_event_log = RegisterEventSource(0, w_name.c_str());

    if (0 == h_event_log) {
        return GetLastError();
    } else {
        LPCTSTR message = w_msg.c_str();

        if (FALSE == ReportEvent(h_event_log,
            a_type,
            0,
            event_id,
            0,
            1,
            0,
            &message,
            0)) {
            return GetLastError();
        }

        DeregisterEventSource(h_event_log);
    }

    return 0;
}

WORD uninstall_eventlog_source(
    const char* a_name)
{
    std::wstring w_name = utf8_to_utf16(std::string(a_name));

    const std::wstring key_path(L"SYSTEM\\CurrentControlSet\\Services\\"
        "EventLog\\ГШ Покров\\" + w_name);
    
    DWORD last_error = RegDeleteKey(HKEY_LOCAL_MACHINE, key_path.c_str());
    
    if (ERROR_SUCCESS != last_error) {
        return last_error;
    }

    return 0;
}