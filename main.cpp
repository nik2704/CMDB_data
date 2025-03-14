#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <unordered_map>
#include <vector>
#include <queue>   // Подключаем очередь
#include <condition_variable>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace json = boost::json;
using tcp = net::ip::tcp;

std::unordered_map<int, std::unordered_map<std::string, std::string>> data_store;

// Класс для пула потоков
class ThreadPool {
public:
    ThreadPool(size_t threads) : stop(false) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] { this->worker(); });
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop = true;
        }
        cv.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }

    // Добавление задачи в пул
    void enqueue(std::function<void()> func) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            tasks.push(std::move(func));
        }
        cv.notify_one();
    }

private:
    void worker() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                cv.wait(lock, [this] { return stop || !tasks.empty(); });

                if (stop && tasks.empty())
                    return;

                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;  // Очередь задач
    std::mutex queue_mutex;
    std::condition_variable cv;
    bool stop;
};

ThreadPool pool(4); 

// Функция для обработки PUT запроса с параметром id
void handle_put_request(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        // Извлечение id из URL
        std::string target(req.target().data(), req.target().size());
        size_t pos = target.find_last_of('/');
        int id = std::stoi(target.substr(pos + 1));

        // Обработка данных JSON
        auto json_data = json::parse(req.body());
        auto& record = data_store[id];

        // Обновление данных
        for (auto& [key, value] : json_data.as_object()) {
            record[key] = value.as_string();
        }

        res.set(http::field::server, "Boost.Beast HTTP Server");
        res.set(http::field::content_type, "application/json");
        res.body() = R"({"status": "success", "message": "Record updated", "id": )" + std::to_string(id) + R"(})";

        res.prepare_payload();
    } catch (const std::exception& e) {
        res.result(http::status::bad_request);
        res.body() = R"({"error": "Invalid JSON data or ID"})";
        res.prepare_payload();
    }
}

// Функция для обработки POST запроса
void handle_post_request(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        // Обработка данных JSON для создания новой записи
        auto json_data = json::parse(req.body());
        int new_id = data_store.size() + 1;  // Новый ID для записи

        std::unordered_map<std::string, std::string> new_record;
        for (auto& [key, value] : json_data.as_object()) {
            new_record[key] = value.as_string();
        }

        data_store[new_id] = new_record;

        res.set(http::field::server, "Boost.Beast HTTP Server");
        res.set(http::field::content_type, "application/json");
        res.body() = R"({"status": "success", "message": "Record created", "id": )" + std::to_string(new_id) + R"(})";
        res.prepare_payload();
    } catch (const std::exception& e) {
        res.result(http::status::bad_request);
        res.body() = R"({"error": "Invalid JSON data"})";
        res.prepare_payload();
    }
}

// Функция для обработки PATCH запроса
void handle_patch_request(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        // Извлечение фильтра из строки запроса
        std::string target(req.target().data(), req.target().size());
        size_t filter_pos = target.find("filter=");
        std::string filter = (filter_pos != std::string::npos) ? target.substr(filter_pos + 7) : "";

        // Обработка данных JSON
        auto json_data = json::parse(req.body());
        auto update = json_data.as_object().at("update").as_object();

        bool updated = false;
        for (auto& [id, record] : data_store) {
            bool match = true;
            if (!filter.empty() && record.find("filter") != record.end() && record["filter"] != filter) {
                match = false;
            }

            if (match) {
                for (auto& [key, value] : update) {
                    record[key] = value.as_string();
                }
                updated = true;
            }
        }

        if (updated) {
            res.set(http::field::server, "Boost.Beast HTTP Server");
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status": "success", "message": "Records updated"})";
        } else {
            res.result(http::status::not_found);
            res.body() = R"({"status": "error", "message": "No records matching filter found"})";
        }
        res.prepare_payload();
    } catch (const std::exception& e) {
        res.result(http::status::bad_request);
        res.body() = R"({"error": "Invalid JSON data"})";
        res.prepare_payload();
    }
}

// Функция для обработки GET запроса с фильтром
void handle_get_request(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    try {
        // Извлечение параметра фильтра из строки запроса
        std::string target(req.target().data(), req.target().size());
        std::string filter;
        size_t filter_pos = target.find("filter=");
        if (filter_pos != std::string::npos) {
            filter = target.substr(filter_pos + 7);  // 7 - длина "filter="
        }

        json::object result;

        // Проходим по всем данным в data_store
        for (const auto& [id, record] : data_store) {
            // Если filter пустой или filter совпадает с данными записи
            if (filter.empty() || (record.find("filter") != record.end() && record.at("filter") == filter)) {
                json::object record_json;
                for (const auto& [key, value] : record) {
                    record_json[key] = value;
                }
                result[std::to_string(id)] = record_json;
            }
        }

        // Формируем успешный ответ
        res.set(http::field::server, "Boost.Beast HTTP Server");
        res.set(http::field::content_type, "application/json");
        res.body() = json::serialize(result);  // Сериализуем JSON объект
        res.prepare_payload();
    } catch (const std::exception& e) {
        res.result(http::status::bad_request);
        res.body() = R"({"error": "Invalid query parameters"})";
        res.prepare_payload();
    }
}

// Функция для обработки всех запросов
void handle_request(tcp::socket& socket) {
    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    http::read(socket, buffer, req);

    http::response<http::string_body> res{http::status::ok, req.version()};

    if (req.target().starts_with("/api/v1/data")) {
        if (req.method() == http::verb::get) {
            handle_get_request(req, res);
        } else if (req.method() == http::verb::post) {
            handle_post_request(req, res);
        } else if (req.method() == http::verb::put) {
            handle_put_request(req, res);
        } else if (req.method() == http::verb::patch) {
            handle_patch_request(req, res);
        } else {
            res.result(http::status::method_not_allowed);
            res.body() = R"({"error": "Method not allowed"})";
            res.prepare_payload();
        }
    } else {
        // В случае других запросов (не на /api/v1/data)
        res.body() = "Hello";
        res.prepare_payload();
    }

    http::write(socket, res);
}

void session(std::shared_ptr<tcp::socket> socket) {
    try {
        handle_request(*socket);  // Dereference the shared_ptr when passing it to handle_request
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }
}



int main() {
    try {
        net::io_context ioc;

        tcp::acceptor acceptor{ioc, {tcp::v4(), 8080}};  // Слушаем на порту 8080 для HTTP
        std::cout << "HTTP сервер запущен на порту 8080\n";

        // Пример инициализации данных
        data_store[1] = {{"field1", "old_value1"}, {"field2", "old_value2"}};
        data_store[2] = {{"field1", "old_value3"}, {"field2", "old_value4"}};

        while (true) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            // Pass the socket by moving it into the session function
            // Modify the enqueue call to pass a unique_ptr
            // Modify the enqueue call to use a shared_ptr
            // Modify the enqueue call to use a shared_ptr
            pool.enqueue([socket = std::make_shared<tcp::socket>(std::move(socket))]() mutable {
                session(socket);  // Pass the shared_ptr directly to session
            });




        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}