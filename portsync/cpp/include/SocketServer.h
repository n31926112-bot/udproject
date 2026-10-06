/**
 * SocketServer.h - Cross-platform TCP server with line buffering.
 * Demonstrates: Cross-platform socket abstraction (#ifdef _WIN32 vs POSIX),
 * line-buffered TCP stream parser handling coalesced & partial network frames, RAII.
 */
#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include "PortfolioManager.h"
#include <string>
#include <functional>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef SOCKET socket_t;
  #define CLOSE_SOCKET(s) closesocket(s)
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  typedef int socket_t;
  #define INVALID_SOCKET (-1)
  #define SOCKET_ERROR (-1)
  #define CLOSE_SOCKET(s) ::close(s)
#endif

class SocketServer {
private:
    int m_port;
    socket_t m_serverSock;
    socket_t m_clientSock;
    bool m_running;

public:
    explicit SocketServer(int port = 5050);
    ~SocketServer();

    // Disable copies
    SocketServer(const SocketServer&) = delete;
    SocketServer& operator=(const SocketServer&) = delete;

    bool start();
    void stop();

    // Listen and process client stream; calls onBatchEnd callback on receiving BATCH_END
    void runLoop(PortfolioManager& manager, const std::function<void(int seq)>& onBatchEnd);
};

#endif // SOCKET_SERVER_H
