#include "Log.h"
#include <ctime>
#include <fstream>
#include <mutex>

// Mutexes to ensure class is threadsafe
std::mutex ioMutex; 
std::mutex timeMutex; 

// PUBLIC

Log::Log() {}
Log::~Log() {}

void Log::serverStarted() {
    Log::write(Log::getTimeStamp() + " | Server started");
}

void Log::successfulConnection(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | User connected");
}

void Log::validQRRequest(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | Received valid qr request");
}

void Log::invalidQRRequest(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | Received invalid qr request");
}

void Log::userDisconnected(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | User disconnected");
}

void Log::userExceededRate(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | User exceeded request rate");
}

/**
 * IP is of the user with the attempted connection that resulted in exceeding the max user threshold 
 */
void Log::maxUsersExceeded(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | Max concurrent users exceeded");
}

// PRIVATE

/**
 * Converts system time into YYYY-MM-DD HH:mm:ss
 * HH (military): 0 - 23
 * 
 * Needs to be thread safe to support concurrent users
 */
string Log::getTimeStamp() {
    timeMutex.lock();

    time_t rawtime;
    struct tm* timeinfo;
    char buffer[19]; // 19 chars satisfy all time stamps

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    std::strftime(buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);

    timeMutex.unlock();

    return buffer;
}

/**
 * Needs to be thread safe to support concurrent users
 */
void Log::write(string logEntry) {
    ioMutex.lock();

    std::ofstream logFile;
    logFile.open("log.txt");
    logFile << logEntry + "\n";
    logFile.close();

    ioMutex.unlock();
}
