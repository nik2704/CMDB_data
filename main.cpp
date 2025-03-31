#include "Server/Server.h"

int main() {
/*
- путь к файлам БД
- порт
- число потоков
*/

    Server server(8080, std::thread::hardware_concurrency() * 2);
    server.Run();
    return 0;
}
