## Project 1 - QR Decoder, Client / Server
### Contributors: 
Jack Gerulskis and Winny Cheng

### Overview

This project uses a TCP client / server model to let the user parse QR codes on the server side, and then get the QR
codes URL as a response.

#### How to use

- run the makefile via 'make'
- run the server executable with the following options in any order
    - --PORT : the server port number (<i>2000 <= PORT <= 3000</i>)
      > default is 2012
    - --RATE_MSG : the max amount of requests per RATE_TIME [<b>Note 1</b>]
      > default is 3 requests
    - --RATE_TIME : the timespan of RATE_MSG [<b>Note 2</b>]
      > default is 60 seconds
    - --MAX_USERS : the maximum amount of concurrent users 
      > default is 3 users
    - --TIME_OUT : the time until a connection times out
      > default is 60 seconds
                                                         
- Run as many client executables as necessary within server bounds using following options in any order
    - --PORT : the server port number (<i>2000 <= PORT <= 3000</i>)
      > default is 2012
    - --ADDRESS : the server ip address
      > default is 127.0.0.1
    - --FILE : (<b>NOT OPTIONAL</b>) the qr file to decode

[<b>Note 1</b>] The max amount of request for all users.
[<b>Note 2</b>] The time span of 'max amount of requests.

#### Our Client Implementation

Our client is required to be given a image file to start as specified. In order to support timeout we allow client users 
the option to make multiple requests within one session. The client, after fulfilling the first request, will ask the 
user to make an additional request of enter 'q' to quit. When a client receives a timeout from the server, they will not 
see the message until the client makes an additional request.  
