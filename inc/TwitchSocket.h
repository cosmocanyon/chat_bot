#ifndef TWITCHSOCKET_H
#define TWITCHSOCKET_H

#include <WinSock2.h>

#include <string>
#include <functional>

constexpr int buffer_size = 513; // buffer 512 bytes (max msg sent by IRC twitch) + 1 for null character '\0' (end of buffer)

class TwitchSocket
{
public:
    TwitchSocket();
    ~TwitchSocket();

    TwitchSocket(const TwitchSocket&) = delete; // copy constructor
    TwitchSocket& operator=(const TwitchSocket&) = delete; // copy assignment operator
    
    SOCKET m_socket;   
    
    int open_connection(const char* address, const char* port);
    int close_socket(int type = SD_SEND);

    int send_msg(const std::string& msg);
    int send_msg_tochannel(const std::string& channel, const std::string& msg);
    std::function<int(const std::string&)> send_tochannel(const std::string& channel);
    
    std::string rcv_msg();

private:
    char* buffer;

};

#endif