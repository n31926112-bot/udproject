/**
 * SocketServer.cpp - Implementation of cross-platform line-buffered TCP server.
 */
#include "SocketServer.h"
#include "Parser.h"
#include <iostream>
#include <vector>
#include <cstring>

SocketServer::SocketServer(int port)
    : m_port(port),
      m_serverSock(INVALID_SOCKET),
      m_clientSock(INVALID_SOCKET),
      m_running(false) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

SocketServer::~SocketServer() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool SocketServer::start() {
    m_serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSock == INVALID_SOCKET) {
        std::cerr << "[ERROR] Failed to create socket\n";
        return false;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(m_serverSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(m_serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(m_port);

    if (bind(m_serverSock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[ERROR] Failed to bind socket to port " << m_port << "\n";
        CLOSE_SOCKET(m_serverSock);
        m_serverSock = INVALID_SOCKET;
        return false;
    }

    if (listen(m_serverSock, 1) == SOCKET_ERROR) {
        std::cerr << "[ERROR] Failed to listen on socket\n";
        CLOSE_SOCKET(m_serverSock);
        m_serverSock = INVALID_SOCKET;
        return false;
    }

    m_running = true;
    std::cout << "[SERVER] PortSync TCP Server listening on 127.0.0.1:" << m_port << "\n";
    return true;
}

void SocketServer::stop() {
    m_running = false;
    if (m_clientSock != INVALID_SOCKET) {
        CLOSE_SOCKET(m_clientSock);
        m_clientSock = INVALID_SOCKET;
    }
    if (m_serverSock != INVALID_SOCKET) {
        CLOSE_SOCKET(m_serverSock);
        m_serverSock = INVALID_SOCKET;
    }
}

void SocketServer::runLoop(PortfolioManager& manager, const std::function<void(int seq)>& onBatchEnd) {
    if (!m_running) return;

    sockaddr_in clientAddr{};
#ifdef _WIN32
    int clientLen = sizeof(clientAddr);
#else
    socklen_t clientLen = sizeof(clientAddr);
#endif

    m_clientSock = accept(m_serverSock, (struct sockaddr*)&clientAddr, &clientLen);
    if (m_clientSock == INVALID_SOCKET) {
        if (m_running) {
            std::cerr << "[ERROR] Failed to accept client connection\n";
        }
        return;
    }

    std::cout << "[SERVER] Client connected successfully.\n";

    std::string lineBuffer;
    char recvBuf[1024];
    int lineNumber = 1;

    while (m_running) {
        int bytesRead = recv(m_clientSock, recvBuf, sizeof(recvBuf) - 1, 0);
        if (bytesRead <= 0) {
            std::cout << "[SERVER] Client disconnected or stream closed.\n";
            break;
        }

        recvBuf[bytesRead] = '\0';
        lineBuffer.append(recvBuf, bytesRead);

        size_t pos;
        while ((pos = lineBuffer.find('\n')) != std::string::npos) {
            std::string rawLine = lineBuffer.substr(0, pos + 1);
            lineBuffer.erase(0, pos + 1);

            ParseResult result = Parser::parseLine(rawLine, lineNumber++);
            switch (result.type) {
                case ParseResultType::HELLO:
                    std::cout << "[SERVER] Handshake: " << result.broker << " (v" << result.version << ")\n";
                    break;
                case ParseResultType::TRADE:
                    manager.applyTrade(Trade(result.broker, result.ticker, result.side, result.qty, result.price, result.dateOrTime));
                    break;
                case ParseResultType::PRICE:
                    manager.applyPriceUpdate(result.ticker, result.price, result.dateOrTime);
                    break;
                case ParseResultType::HIST:
                    manager.addHistClose(result.ticker, result.dateOrTime, result.price);
                    break;
                case ParseResultType::NEWS:
                    manager.addNews(NewsItem(result.ticker, result.dateOrTime, result.headline));
                    break;
                case ParseResultType::BATCH_END:
                    if (onBatchEnd) {
                        onBatchEnd(result.seq);
                    }
                    break;
                case ParseResultType::BYE:
                    std::cout << "[SERVER] Received BYE signal. Closing connection.\n";
                    m_running = false;
                    break;
                case ParseResultType::MALFORMED:
                    std::cerr << "[SERVER WARN] Malformed record on line " << (lineNumber - 1)
                              << ": " << result.errorMsg << "\n";
                    break;
                case ParseResultType::UNKNOWN:
                    std::cerr << "[SERVER WARN] Unknown record on line " << (lineNumber - 1)
                              << ": " << result.errorMsg << "\n";
                    break;
            }

            if (!m_running) break;
        }
    }

    if (m_clientSock != INVALID_SOCKET) {
        CLOSE_SOCKET(m_clientSock);
        m_clientSock = INVALID_SOCKET;
    }
}
