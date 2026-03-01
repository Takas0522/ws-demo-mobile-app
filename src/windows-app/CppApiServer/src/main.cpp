#include "Server.h"
#include "Utils/Constants.h"

#include <iostream>
#include <string>
#include <csignal>

static ws::Server* g_server = nullptr;

void SignalHandler(int /*signal*/)
{
	if (g_server)
	{
		std::cout << "\nShutting down server..." << std::endl;
		g_server->Stop();
	}
}

int main(int argc, char* argv[])
{
	int port = ws::utils::kDefaultPort;
	std::string dbPath = ws::utils::kDefaultDbPath;

	// Parse command-line arguments
	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];

		if ((arg == "--port" || arg == "-p") && i + 1 < argc)
		{
			try
			{
				port = std::stoi(argv[++i]);
			}
			catch (...)
			{
				std::cerr << "[ERROR] Invalid port number: " << argv[i] << std::endl;
				return 1;
			}
		}
		else if ((arg == "--db" || arg == "-d") && i + 1 < argc)
		{
			dbPath = argv[++i];
		}
		else if (arg == "--help" || arg == "-h")
		{
			std::cout << "Usage: CppApiServer [options]" << std::endl;
			std::cout << "  --port, -p <port>  Server port (default: "
				<< ws::utils::kDefaultPort << ")" << std::endl;
			std::cout << "  --db, -d <path>    Database path (default: "
				<< ws::utils::kDefaultDbPath << ")" << std::endl;
			std::cout << "  --help, -h         Show this help message" << std::endl;
			return 0;
		}
		else
		{
			std::cerr << "[WARN] Unknown argument: " << arg << std::endl;
		}
	}

	// Set up signal handling for graceful shutdown
	std::signal(SIGINT, SignalHandler);
	std::signal(SIGTERM, SignalHandler);

	ws::Server server;
	g_server = &server;

	if (!server.Initialize(dbPath))
	{
		std::cerr << "[ERROR] Failed to initialize server" << std::endl;
		return 1;
	}

	server.Run(port);

	std::cout << "Server stopped." << std::endl;
	return 0;
}
