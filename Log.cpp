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

string Log::serverStarted() {
	string output = Log::getTimeStamp() + "\t | Server started PORT=" + std::to_string(Log::port) + ", RATE_MSGS=" +
			std::to_string(Log::rateMSGS) + ", RATE_TIME=" + std::to_string(Log::rateTime) + ", MAX_USERS=" +
			std::to_string(Log::maxUsers) + ", TIMEOUT=" +std::to_string(Log::timeout) + "\n";
    Log::write(output);
    return output;
}

string Log::successfulConnection(string ip, int clientPort) {
	string output = Log::getTimeStamp() + " " + ip + ":" + std::to_string(clientPort) + "\t | User connected\n";
    Log::write(output);
    return output;
}

string Log::validQRRequest(string ip, int clientPort) {
	string output = Log::getTimeStamp() + " " + ip + ":" + std::to_string(clientPort) + "\t | Received valid qr request\n";
	Log::write(output);
	return output;
}

string Log::invalidQRRequest(string ip, int clientPort) {
	string output = Log::getTimeStamp() + " " + ip + ":" + std::to_string(clientPort) + "\t | Received invalid qr request\n";
	Log::write(output);
	return output;
}

string Log::userDisconnected(string ip, int clientPort) {
	string output = Log::getTimeStamp() + " " + ip + ":" + std::to_string(clientPort) + "\t | User disconnected\n";
	Log::write(output);
	return output;
}
string Log::userTimeout(string ip, int clientPort) {
    string output = Log::getTimeStamp() + " " + ip + ":" + std::to_string(clientPort) + "\t | User timed-out\n";
    Log::write(output);
    return output;
}

string Log::userExceededRate(string ip, int clientPort) {
	string output = Log::getTimeStamp() + " " + ip + ":" + std::to_string(clientPort) + "\t | User exceeded request rate\n";
	Log::write(output);
	return output;
}

/**
 * IP is of the user with the attempted connection that resulted in exceeding the max user threshold 
 */
string Log::maxUsersExceeded(string ip, int clientPort) {
    string output = Log::getTimeStamp() + " " + ip + ":" + std::to_string(clientPort) + "\t | Max concurrent users exceeded\n";
	Log::write(output);
	return output;
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
