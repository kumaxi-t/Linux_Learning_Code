#pragma once
#include <iostream>
#include <pthread.h>
#include "Lock.hpp"
#include <ctime>
#include <string>
#include <fstream>
#include <memory>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#ifdef DEBUG
#undef DEBUG  


#endif
namespace LogModule{
  using namespace MutexModule;
  const std::string dpath = "./log/";
  const std::string dname = "log.txt";

  enum class LogLevel {
    DEBUG,
    WARNING,
    INFO,
    ERROR,
    FATAL
  };

  inline std::string LogLevelToString(LogLevel level){
    switch (level) {
      case LogLevel::DEBUG:   return "DEBUG";
      case LogLevel::INFO:    return "INFO";
      case LogLevel::WARNING: return "WARNING";
      case LogLevel::ERROR:   return "ERROR";
      case LogLevel::FATAL:   return "FATAL";
      default:                return "UNKNOWN";
    }
  }


  inline std::string GetCurrTime(){
    time_t tm = time(nullptr);  
    // time_t 记录的是从 1970 年 1 月 1 日 00:00:00 UTC（被称为 Unix 纪元） 开始，到当前这一秒钟，一共过去了多少秒。
    // time(nullptr) 拿到当前的 Unix 时间戳（比如 1775000000 秒），然后存入变量 tm 中
    struct tm curr;
    localtime_r(&tm, &curr);
    // 将 tm 里的 Unix 时间戳（17 亿多秒），根据当前系统的时区设置，换算成当地的年月日时分秒，并填入到 curr 结构体变量中。
    
    char timebuffer[64];
    snprintf(timebuffer, sizeof(timebuffer), "%4d-%02d-%02d %02d:%02d:%02d",
      curr.tm_year + 1900, // tm_year 存储的是自1900年以来的年数，故需 +1900
      curr.tm_mon + 1,     // tm_mon 范围是 0~11，代表1~12月，故需 +1
      curr.tm_mday,        // 一个月中的第几天 (1~31)
      curr.tm_hour,        // 小时 (0~23)
      curr.tm_min,         // 分钟 (0~59)
      curr.tm_sec);        // 秒数 (0~59)

    return timebuffer;
  }


  class LogStrategy{
  public:
    virtual ~LogStrategy() = default;
    virtual void SyncLog(const std::string &message) = 0;
  };

  class ConsoleLogStrategy : public LogStrategy{
  public:
    void SyncLog(const std::string &message) override {
      LockGuard lockguard(_mutex);
      std::cerr << message << std::endl;
    }
  private:
    Mutex _mutex;
  };

  class FileLogStrategy : public LogStrategy{
  public:
    FileLogStrategy(const std::string &logpath = dpath, const std::string &logfilename = dname)
      : _logpath(logpath),
        _logfilename(logfilename){
      LockGuard lockguard(_mutex);
      if (!std::filesystem::exists(_logpath)) {
        std::filesystem::create_directories(_logpath);
      }
    }
    
    void SyncLog(const std::string &message) override {
      LockGuard lockguard(_mutex);
      std::string logfile = _logpath + _logfilename;

      std::ofstream out(logfile.c_str(), std::ios::app);
      if(!out.is_open()) return ;

      out << message << "\n";
      out.close();
    }
  private:
    std::string _logfilename;
    std::string _logpath;
    Mutex _mutex;
  };


  class Logger;

  class LogMessage{
  public:
    LogMessage(LogLevel level, const std::string &filename, int line, Logger &logger)
    : _logger(logger) {
      _ss << "[" << GetCurrTime() << "]"
          << "[" << LogLevelToString(level) << "]"
          << "[" << getpid() << "]"
          << "[" << filename << "]"
          << "[" << line << "] - ";
    }

    template<typename T>
    LogMessage &operator<<(const T &info){
      _ss << info;
      return *this;
    }

    ~LogMessage();
  private:
    std::stringstream _ss;
    Logger &_logger;
  };


  class Logger{
  public:
    Logger(){
      UseConsoleStrategy();
    }
    // 切换为控制台策略
    void UseConsoleStrategy() {
      _strategy = std::make_unique<ConsoleLogStrategy>();
    }

    // 切换为文件策略
    void UseFileStrategy() {
      _strategy = std::make_unique<FileLogStrategy>();
    }

    // 把拼接好的字符串传递给策略对象刷出去
    void Flush(const std::string &msg) {
      if (_strategy) {
        _strategy->SyncLog(msg);
      }
    }

    // 重载 () 运算符：方便通过 logger(level, file, line) 快速产生一个 LogMessage 临时对象
    LogMessage operator()(LogLevel level, const std::string &filename, int line) {
      return LogMessage(level, filename, line, *this);
    }
    ~Logger(){}

  private:
    std::unique_ptr<LogStrategy> _strategy;
  };

  static Logger logger;
  inline LogMessage::~LogMessage(){
    _logger.Flush(_ss.str());
  }


  #define LOG(level) logger(level, __FILE__, __LINE__)
  #define ENABLE_CONSOLE_LOG() logger.UseConsoleStrategy()
  #define ENABLE_FILE_LOG() logger.UseFileStrategy()
}
// end LogModule
