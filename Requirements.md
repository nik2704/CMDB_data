# Техническое Задание

**Название проекта:** Разработка централизованного хранилища Configuration Management Database (CMDB) с многопоточным HTTP API для управления данными в микросервисной архитектуре.

**Версия:** 1.0

**Дата:** 05.04.2025

## 1. Введение

Данное техническое задание (ТЗ) определяет требования к разработке централизованной системы управления конфигурациями (Configuration Management Database, CMDB) и многопоточного HTTP API.

Система предназначена для обеспечения эффективного управления небольшого объема конфигурационных данных, которые включают конфигурационные элементы (CI) и связи между ними. Решение должно позволять использование в контексте микросервисной архитектуры.

## 2. Цели и задачи проекта

### 2.1. Цели:

* Разработать хранилище конфигурационных данных (CMDB).
* Обеспечить хранение данных CMDB в виде бинарного файла.
* Разработать удобный и эффективный интерфейс для управления данными CMDB посредством RESTful HTTP API.
* Реализовать многопоточность HTTP-сервера для обработки большого количества запросов.
* Обеспечить интеграцию разработанной CMDB с микросервисными приложениями.

### 2.2. Задачи:

* Разработка модели данных CMDB, включающей конфигурационные элементы (CI) и связи между ними.
* Реализация классов C++ для представления CI, связей и основной логики CMDB.
* Разработка многопоточного HTTP-сервера на базе библиотеки Boost.Beast.
* Реализация RESTful API для выполнения операций CRUD (создание, чтение, обновление, удаление) над CI и связями.
* Реализация API для управления уровнями CI.
* Обеспечение хранения данных CMDB в файловой системе.
* Реализация механизма автоматического сохранения данных.
* Реализация возможности поиска CI по различным критериям (ID, тип, уровень, свойства).
* Реализация возможности поиска связанных CI.
* Реализация возможности экспорта данных CMDB в формате JSON.
* Обеспечение обработки параметров командной строки для настройки сервера (порт, количество потоков, путь к файлу БД).

## 3. Требования к системе

### 3.1. Функциональные требования:

* **Управление уровнями CI:**
    * Возможность добавления, удаления, переименования и получения списка уровней CI.
    * Возможность получения информации об уровне по индексу или названию.
* **Управление конфигурационными элементами (CI):**
    * Возможность добавления CI с указанием ID, имени, типа, уровня и набора свойств.
    * Возможность удаления CI по ID.
    * Возможность получения информации о CI по ID.
    * Возможность получения списка всех CI или CI, отфильтрованных по уровню, типу или набору свойств.
    * Возможность обновления свойств, имени и уровня существующего CI.
    * Возможность поиска связанных CI на заданное количество шагов.
    * Возможность получения списка всех уникальных свойств CI.
* **Управление связями (Relationships):**
    * Возможность добавления связей между CI с указанием исходного CI, целевого CI и типа связи.
    * Возможность удаления связей по исходному и целевому CI, а также по типу связи.
    * Возможность удаления всех связей, связанных с определенным CI.
    * Возможность получения списка всех связей или связей, отфильтрованных по исходному и/или целевому CI и типу связи.
    * Возможность получения списка зависимых CI для заданного CI.
* **HTTP API:**
    * Предоставление RESTful API для выполнения всех вышеперечисленных операций.
    * Поддержка методов HTTP: GET, POST, PUT, DELETE.
    * Формат запросов и ответов: JSON.
    * Обработка параметров запроса для фильтрации данных.
* **Хранение данных:**
    * Хранение данных CMDB в бинарном файле.
    * Возможность загрузки данных из файла при запуске сервера.
    * Реализация механизма автоматического периодического сохранения данных.
    * Возможность указания пути к файлу базы данных через параметр командной строки.
* **Конфигурация сервера:**
    * Возможность настройки порта HTTP-сервера через параметр командной строки.
    * Возможность настройки количества рабочих потоков HTTP-сервера через параметр командной строки.
* **Обработка ошибок:**
    * Корректная обработка ошибок и формирование информативных ответов в формате JSON.

### 3.2. Нефункциональные требования:

* **Надежность:**
    * Система должна быть устойчивой к ошибкам и обеспечивать целостность данных.
* **Поддержка платформ:**
    * Разработка должна быть выполнена с поддержкой Linux.
* **Использование технологий:**
    * Язык программирования: C++ (версия 17 или выше).
    * Библиотеки: Boost.Asio, Boost.Beast, Boost.JSON, Boost.Program_options.
* **Многопоточность:**
    * HTTP-сервер должен быть многопоточным для эффективной обработки входящих соединений.

## 4. Требования к интерфейсам

### 4.1. Интерфейс командной строки:

* Поддержка следующих параметров:
    * `-h` или `--help`: вывод справки по доступным опциям.
    * `-p <номер_порта>` или `--port <номер_порта>`: установка порта сервера (по умолчанию 8080).
    * `-t <количество_потоков>` или `--threads <количество_потоков>`: установка количества рабочих потоков (по умолчанию `std::thread::hardware_concurrency() * 2`).
    * `-d <путь_к_файлу_БД>` или `--db <путь_к_файлу_БД>`: установка пути к файлу базы данных (по умолчанию `cmdb.bin`).

### 4.2. Программный интерфейс (API):

* API должен быть интуитивно понятным и удобным для использования микросервисными приложениями.
* Коллекция запросов Postman в формате json в файле `"CMDB_DEV.postman_collection.json"` в данном репозитории.

## 5. Требования к документации

* Комментарии к классам и методам.
* Пример Dockerfile.
* Пример YAML для установки в среду Kubernetes.
* Пример коллекции запросов.

## 6. Критерии приемки

* Соответствие разработанной системы всем функциональным и нефункциональным требованиям, указанным в данном ТЗ.
* Успешное прохождение всех запланированных тестов.
* Наличие комментариев, коллекции запросов и дескрипторов.

## 7. Возможные дальнейшее шаги развитие (потенциальные улучшения)

* Реализация аутентификации и авторизации для доступа к API.
* Добавление поддержки различных форматов хранения данных (например, JSON, другие базы данных).
* Реализация более сложной логики поиска и фильтрации данных.
* Добавление механизма ведения истории изменений.
* Реализация графического интерфейса пользователя (GUI) для администрирования CMDB.
* Интеграция с системами мониторинга и оповещения.
