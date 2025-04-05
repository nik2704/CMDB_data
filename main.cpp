#include <boost/program_options.hpp>
#include "Server/Server.h"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    std::string db_path = "cmdb.bin";
    int port = 8080;
    unsigned int num_threads = std::thread::hardware_concurrency() * 2;

    try {
        po::options_description desc("Допустимые опции");
        desc.add_options()
            ("help,h", "help")
            ("port,p", po::value<int>(&port)->default_value(8080), "Номер порта (по умолчанию 8080)")
            ("threads,t", po::value<unsigned int>(&num_threads)->default_value(std::thread::hardware_concurrency() * 2), "Число потоков (по умолчанию число процессоров * 2)")
            ("db,d", po::value<std::string>(&db_path)->default_value("cmdb.bin"), "Путь к файлу БД");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Использование: " << argv[0] << " [options]" << std::endl;
            std::cout << desc << "\n";
            return 0;
        }

        std::cout << "Используемые параметры:" << std::endl;
        std::cout << "  Порт: " << port << std::endl;
        std::cout << "  Число потоков: " << num_threads << std::endl;
        std::cout << "  Файл БД: " << db_path << std::endl;

        Server server(port, num_threads, db_path);
        server.Run();

    } catch (const po::error& e) {
        std::cerr << "Ошибка чтения опций: " << e.what() << std::endl;
        std::cout << "Используйте '" << argv[0] << " --help' для более подробной." << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    // Server server(8080, std::thread::hardware_concurrency() * 2, "cmdb.bin");
    // server.Run();
    return 0;
}
