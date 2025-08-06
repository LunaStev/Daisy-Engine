#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>

namespace Daisy {

enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3
};

class Logger {
public:
    static Logger& GetInstance();
    
    void Initialize(const std::string& logFile = "daisy_engine.log");
    void SetLogLevel(LogLevel level);
    
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    
    template<typename... Args>
    void Debug(const std::string& format, Args&&... args) {
        if (m_logLevel <= LogLevel::Debug) {
            Log(LogLevel::Debug, FormatString(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void Info(const std::string& format, Args&&... args) {
        if (m_logLevel <= LogLevel::Info) {
            Log(LogLevel::Info, FormatString(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void Warning(const std::string& format, Args&&... args) {
        if (m_logLevel <= LogLevel::Warning) {
            Log(LogLevel::Warning, FormatString(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void Error(const std::string& format, Args&&... args) {
        if (m_logLevel <= LogLevel::Error) {
            Log(LogLevel::Error, FormatString(format, std::forward<Args>(args)...));
        }
    }
    
private:
    Logger() = default;
    
    void Log(LogLevel level, const std::string& message);
    std::string GetTimestamp();
    std::string GetLogLevelString(LogLevel level);
    
    template<typename... Args>
    std::string FormatString(const std::string& format, Args&&... args) {
        std::ostringstream oss;
        FormatStringImpl(oss, format, std::forward<Args>(args)...);
        return oss.str();
    }
    
    template<typename T, typename... Args>
    void FormatStringImpl(std::ostringstream& oss, const std::string& format, T&& value, Args&&... args) {
        size_t pos = format.find("{}");
        if (pos != std::string::npos) {
            oss << format.substr(0, pos) << value;
            FormatStringImpl(oss, format.substr(pos + 2), std::forward<Args>(args)...);
        } else {
            oss << format;
        }
    }
    
    void FormatStringImpl(std::ostringstream& oss, const std::string& format) {
        oss << format;
    }
    
    std::unique_ptr<std::ofstream> m_logFile;
    LogLevel m_logLevel = LogLevel::Info;
    std::mutex m_mutex;
};

}

#define DAISY_LOG Daisy::Logger::GetInstance()
#define DAISY_DEBUG(...) DAISY_LOG.Debug(__VA_ARGS__)
#define DAISY_INFO(...) DAISY_LOG.Info(__VA_ARGS__)
#define DAISY_WARNING(...) DAISY_LOG.Warning(__VA_ARGS__)
#define DAISY_ERROR(...) DAISY_LOG.Error(__VA_ARGS__)