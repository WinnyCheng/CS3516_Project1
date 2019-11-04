#include "Log.h"
#include <ctime>
#include <stdio.h>
#include <mutex>

// Mutexes to ensure class is threadsafe, does this project forks though?
std::mutex ioMutex; 
std::mutex timeMutex; 

// PUBLIC

Log::Log(int port, int rateMSGS, int rateTime, int maxUsers, int timeout) {
	Log::port = port;
	Log::rateMSGS = rateMSGS;
	Log::rateTime = rateTime;
	Log::maxUsers = maxUsers;
	Log::timeout = timeout;
}
Log::~Log() {}

void Log::serverStarted() {
    Log::write(Log::getTimeStamp() + " | Server started on port " + std::to_string(Log::port) + "\n");
}

void Log::successfulConnection(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | User connected\n");
}

void Log::validQRRequest(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | Received valid qr request\n");
}

void Log::invalidQRRequest(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | Received invalid qr request\n");
}

void Log::userDisconnected(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | User disconnected\n");
}

void Log::userExceededRate(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | User exceeded request rate\n");
}

/**
 * IP is of the user with the attempted connection that resulted in exceeding the max user threshold 
 */
void Log::maxUsersExceeded(string ip) {
    Log::write(Log::getTimeStamp() + " " + ip + " | Max concurrent users exceeded\n");
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

    FILE *logFile;
    logFile = fopen("log.txt", "a");
	fprintf(logFile, logEntry.c_str());
    fclose(logFile);

    ioMutex.unlock();
}
