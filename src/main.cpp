#include "TwitchSocket.h"
#include "utils.h"

//#include <WinSock2.h>
//#include <WS2tcpip.h>
// #pragma comment(lib,"ws2_32.lib") 
// g++ doesn't support the #pragma comment directive
// use arg -lws2_32 during linking  

#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>

int main(/*int argc, char* argv[]*/) 
{  
    WSADATA wsaData;

    const char* loginfile = "../data/config.txt";
    std::ifstream myfile(loginfile);
    std::string nickname, authtoken;
    std::string channel = "channel";

    // 1.initialize windows sockets dll (Ws32_32.dll)
    std::cout << "Initializing WinSock ...\n";

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);    
    if(result != 0) {
        std::cerr << "WinSock Startup failed with error: " << result << std::endl;
        return 1;
    } 
    std::cout << "WinSock Startup successful\n";

    std::cout << "Loading credentials ...\n";
    if (myfile.is_open())
    {
        std::string line;
        while (std::getline(myfile, line))
        {
            std::stringstream(line) >> nickname >> authtoken;
        }

        myfile.close();    
    } else {
        std::cerr << "Failed to open file: " << loginfile << std::endl;
        return 1;
    }
    std::cout << "credentials loaded\n";

    // ricerca informazioni per la connessione
    // refactoring !!!

    const char* address = "irc.chat.twitch.tv";
    const char* port = "6667";

    TwitchSocket connect_socket {};

    if(connect_socket.open_connection(address, port) != 0) {
        std::cerr << "failed to connect: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "connection established\n";

    // effettuare il login
    if(connect_socket.send_msg("CAP REQ :twitch.tv/membership twitch.tv/tags twitch.tv/commands\r\n") == SOCKET_ERROR) {
        //error
    }

    // mandare due msg e ricevere conferma
    if(connect_socket.send_msg("PASS " + authtoken + "\r\n") == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << "\n";
        closesocket(connect_socket.m_socket);
        WSACleanup();
        return 1;
    }

    if(connect_socket.send_msg("NICK " + nickname + "\r\n") == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << "\n";
        closesocket(connect_socket.m_socket);
        WSACleanup();
        return 1;
    }

    std::cout << connect_socket.rcv_msg() << "\n";
   
    // join channel
    if(connect_socket.send_msg("JOIN #"+ channel + "\r\n") == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << "\n";
        closesocket(connect_socket.m_socket);
        WSACleanup();
        return 1;
    }

    std::regex re("^@(.+) :([^!]+)![^@]+@[^\\.]+\\.tmi\\.twitch\\.tv ([A-Z]+) #([^\\s]+) \\:(.*)");
    std::smatch match;

    std::chrono::system_clock clock;
    std::chrono::time_point<std::chrono::system_clock> last;
    last = clock.now() - std::chrono::seconds(10);

    std::string arguments;

    auto send_message = connect_socket.send_tochannel(channel);

    auto bot_command = [](const std::string& command_name, const std::string& command, std::string* args) {
        int split = command_name.length() + 1;
        if(command.substr(0, split) == command_name + " ") {
            *args = command.substr(split);
            return true;
        }
        return false;
    };

    // receive data FROM until peer closes connection
    do {
        std::string reply = connect_socket.rcv_msg();
        std::cout << reply;

        if (bot_command("PING", reply, &arguments))
        {
            std::cout << "Got PING I PONG back!\n";
            connect_socket.send_msg("PONG " + arguments +"\r\n");

            continue;
        }
        
        std::regex_search(reply, match, re);

        std::string complete_message = match[0];
        std::string tags = match[1];
        std::string username = match[2];
        std::string command_server = match[3];
        std::string channel_connected = match[4]; 
        std::string command = match[5];

        std::map<std::string, std::string> attributes = utils::attributes_for(tags);

        
        std::cout << "attributes: " << tags << "\n";
        std::cout << "server command: " << command_server << "\n"; 
        std::cout << "channel: " << channel_connected << "; user: " << username << "; message: " << command << "\n";
        
        if(command == "!test") {
            if(clock.now() -  last >= std::chrono::seconds(10)) {
                std::cout << "This is a successfull test!\n";
                last = clock.now();
            }
        }

        if(command == "!random") {
            //connect_socket.send_msg("PRIVMSG  #" + channel + " :hello too!\r\n");
            //connect_socket.send_msg_tochannel(channel, "Hello!");
            send_message("4");
        }

        if(command == "!hi")
        {
            send_message("Hi!");
        }

        if(bot_command("!welcome", command, &arguments)) {
            send_message("Welcome to the channel " + arguments + "!");
        }
  
        if(bot_command("!tnt", command, &arguments)) {
            send_message("Bye bye! " + arguments);
        }
        
        
        
    } while (true);

    // disconnecting and cleanup
    connect_socket.close_socket(SD_BOTH);

    WSACleanup();

    return 0;
}