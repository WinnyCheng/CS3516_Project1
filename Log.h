#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <time.h>

using std::string;

class Log {
public:
    Log(int port, int rateMSGS, int rateTime, int maxUsers, int timeout);
    ~Log();
    void serverStarted();
    void successfulConnection(string ip, int clientPort);
    void validQRRequest(string ip, int clientPort);
    void invalidQRRequest(string ip, int clientPort);
    void userDisconnected(string ip, int clientPort);
    void userExceededRate(string ip, int clientPort);
    void maxUsersExceeded(string ip, int clientPort);
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