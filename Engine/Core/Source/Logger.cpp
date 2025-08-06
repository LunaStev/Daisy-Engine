#include "Core/Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Daisy {

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

void Logger::Initialize(const std::string& logFile) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_logFile = std::make_unique<std::ofstream>(logFile, std::ios::out | std::ios::app);
    if (!m_logFile->is_open()) {
        std::cerr << "Failed to open log file: " << logFile << std::endl;
        m_logFile.reset();
    }
    
    Info("Logger initialized - Log file: {}", logFile);
}

void Logger::SetLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logLevel = level;
}

void Logger::Debug(const std::string& message) {
    if (m_logLevel <= LogLevel::Debug) {
        Log(LogLevel::Debug, message);
    }
}

void Logger::Info(const std::string& message) {
    if (m_logLevel <= LogLevel::Info) {
        Log(LogLevel::Info, message);
    }
}

void Logger::Warning(const std::string& message) {
    if (m_logLevel <= LogLevel::Warning) {
        Log(LogLevel::Warning, message);
    }
}

void Logger::Error(const std::string& message) {
    if (m_logLevel <= LogLevel::Error) {
        Log(LogLevel::Error, message);
    }
}

void Logger::Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string timestamp = GetTimestamp();
    std::string levelStr = GetLogLevelString(level);
    std::string fullMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    std::cout << fullMessage << std::endl;
    
    if (m_logFile && m_logFile->is_open()) {
        *m_logFile << fullMessage << std::endl;
        m_logFile->flush();
    }
}

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string Logger::GetLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO ";
        case LogLevel::Warning: return "WARN ";
        case LogLevel::Error:   return "ERROR";
        default:                return "UNKNW";
    }
}

}