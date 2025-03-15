#include "Server/Server.h"

int main() {
    Server server(8080, std::thread::hardware_concurrency() * 2);
    server.Run();
    return 0;
}
