#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <time.h>

using std::string;

class Log {
public:
    Log(int port, int rateMSGS, int rateTime, int maxUsers, int timeout);
    ~Log();
    string serverStarted();
    string successfulConnection(string ip, int clientPort);
    string validQRRequest(string ip, int clientPort);
    string invalidQRRequest(string ip, int clientPort);
    string userDisconnected(string ip, int clientPort);
    string userExceededRate(string ip, int clientPort);
    string maxUsersExceeded(string ip, int clientPort);
private:
    string getTimeStamp();
    void write(string logEntry);
    int port;
    int rateMSGS;
    int rateTime;
    int maxUsers;
    int timeout;
};

#endif