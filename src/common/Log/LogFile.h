/***********************************************************************
* @ 高性能异步日志
* @ brief
    1、使用简单，记录调用信息

    2、TODO：还要种记录业务数据的入库表日志，能方便统计、出bug补错等

* @ author zhoumf
* @ date 2016-11-23
************************************************************************/
#pragma once

//#include <string>

class AsyncLog;
class LogFile {
public:
    static LogFile* g_log; //为了让析构函数在逻辑层调用，把单例换成了指针。交给系统回收静态对象，析构执行太晚，AsyncLog的子线程结束便太晚，进程僵死
    enum LogLv {
        OFF = 6,
        ERRO = 5,
        WARING = 4,
        INFO = 3,
        Debug = 2,
        TRACK = 1,
        ALL = 0,
    };
    LogFile(std::string fileName, LogLv lv, bool isPrint);
    ~LogFile();

    void SetLog(LogLv lv, bool isPrint){ _level = lv; _isPrint = isPrint; }
    void Log(const char* curFile, const int curLine, LogLv kLv, const char* fmt, ...);
private:
    FILE*       _fp = NULL;
    AsyncLog*   _async = NULL;
    LogLv       _level = OFF;
    bool        _isPrint = true;

    const char* LevelToString(LogLv kLevel);
};

#define _LOG_MAIN_(obj) LogFile::g_log = &obj /*Notice：要在main函数里，不然程序未结束Log就析构了*/
#define LOG_INFO(...)   LogFile::g_log->Log(__FILE__, __LINE__, LogFile::INFO, __VA_ARGS__)
#define LOG_WARN(...)   LogFile::g_log->Log(__FILE__, __LINE__, LogFile::WARING, __VA_ARGS__)
#define LOG_ERROR(...)  LogFile::g_log->Log(__FILE__, __LINE__, LogFile::ERRO, __VA_ARGS__)
#ifdef _DEBUG
#define LOG_TRACK(...)  { \
    char str[128]; \
    sprintf(str, "%s-%s", __FILE__, __FUNCTION__); \
    LogFile::g_log->Log(str, __LINE__, LogFile::TRACK, __VA_ARGS__); \
}
#define LOG_DEBUG(...)  LogFile::g_log->Log(__FILE__, __LINE__, LogFile::Debug, __VA_ARGS__)
#else
#define LOG_TRACK(...)
#define LOG_DEBUG(...)
#endif