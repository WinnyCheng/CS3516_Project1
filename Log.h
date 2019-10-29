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
    void successfulConnection(string ip);
    void validQRRequest(string ip);
    void invalidQRRequest(string ip);
    void userDisconnected(string ip);
    void userExceededRate(string ip);
    void maxUsersExceeded(string ip);
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