#include "TwitchSocket.h"

#include <WS2tcpip.h>

#include <iostream>

TwitchSocket::TwitchSocket() : m_socket{INVALID_SOCKET} {
    buffer = new char[buffer_size];
    ZeroMemory(buffer, buffer_size);
}

TwitchSocket::~TwitchSocket() {
    if(m_socket != INVALID_SOCKET) 
        closesocket(m_socket);

    delete[] buffer;
    buffer = nullptr;
}

int TwitchSocket::open_connection(const char* address, const char* port) {
    int err;
    addrinfo *addr_list = nullptr, hints {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

     // resolve server address and port
    err = getaddrinfo(address, port, &hints, &addr_list);    
    if(err != 0) {
        std::cerr << "getaddrinfo failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // attempt connection to any address returned by getaddrinfo
    for(addrinfo *addr = addr_list; addr != nullptr; addr = addr->ai_next) {

        // create a SOCKET for connecting to server
        m_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if(m_socket == INVALID_SOCKET) {
            std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup(); // WSACleanup is used to terminate the use of the WS2_32 DLL.
            return 1;
        }

        // connect the socket to server.
        err = connect(m_socket, addr->ai_addr, static_cast<int>(addr->ai_addrlen));
        if(err == SOCKET_ERROR) {
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(addr_list);

    if(m_socket == INVALID_SOCKET) {
        std::cerr << "unable to connect to server: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    return err;
}

int TwitchSocket::close_socket(int type /*= SD_SEND*/) {
    // shutdown the send half of the connection since no more data will be sent
    int err = shutdown(m_socket, type);
    if(err == SOCKET_ERROR) {
        std::cerr << "shutdown failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        WSACleanup();
        return 1;
    }
    return err;
}

int TwitchSocket::send_msg(const std::string& msg) {
    return send(m_socket, msg.c_str(), static_cast<int>(msg.size()), 0);
}

int TwitchSocket::send_msg_tochannel(const std::string& channel, const std::string& msg) {
    return send_msg("PRIVMSG  #" + channel + " :" + msg + "\r\n");
}

std::function<int(const std::string&)> TwitchSocket::send_tochannel(const std::string& channel) {
    return [=] (const std::string& msg) {
        return send_msg("PRIVMSG  #" + channel + " :" + msg + "\r\n");
    };
}

std::string TwitchSocket::rcv_msg() {

    int bytes_received = recv(m_socket, buffer, buffer_size-1, 0);
    
    if (bytes_received == 0) { // gestire meglio lo stato della connessione, al momento rimane nel loop continuo
        std::cout << "Connection closed\n";
    } else if (bytes_received == SOCKET_ERROR) {
        std::cout << "recv failed: " << WSAGetLastError() << "\n";
    } else {
        if(bytes_received < buffer_size-1) 
            buffer[bytes_received] = '\0';
        else
            buffer[buffer_size-1] = '\0';

        //std::cout << strlen(buffer) << "\n"; 
        //std::cout << bytes_received << "\n";
        //std::cout << "stampa prima lettera msg: " << buffer[0] << "\n";   
        //std::cout << "stampa seconda lettera msg: " << buffer[1] << "\n";
        //std::cout << "stampa vera ultima lettera msg: " << buffer[bytes_received-3] << "\n";   
        //std::cout << "stampa ultima lettera msg \\n: " << buffer[bytes_received-1] << "\n"; 
        //std::cout << "stampa \\0: " << buffer[bytes_received] << "\n";

    }

    return std::string(buffer, buffer + bytes_received);
}