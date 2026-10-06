/**
 * main.cpp - Entry point for PortSync C++ TCP Server & Dashboard.
 * Demonstrates: Program entry point, CLI flag parsing, component orchestration.
 */
#include "PortfolioManager.h"
#include "SocketServer.h"
#include "Dashboard.h"
#include <iostream>
#include <string>
#include <cstring>

int main(int argc, char* argv[]) {
    int port = 5050;
    bool useColor = true;
    bool interactive = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (std::strcmp(argv[i], "--no-color") == 0) {
            useColor = false;
        } else if (std::strcmp(argv[i], "--interactive") == 0) {
            interactive = true;
        }
    }

    std::cout << "=== Starting PortSync Cross-Broker Aggregator (C++ Engine) ===\n";

    PortfolioManager manager;
    Dashboard dashboard(useColor);
    SocketServer server(port);

    if (!server.start()) {
        std::cerr << "[FATAL] Failed to start TCP server on port " << port << ". Exiting.\n";
        return 1;
    }

    server.runLoop(manager, [&dashboard, &manager](int seq) {
        dashboard.render(manager, seq);
    });

    if (interactive) {
        dashboard.interactivePrompt(manager);
    }

    std::cout << "[MAIN] PortSync server shutdown cleanly.\n";
    return 0;
}
