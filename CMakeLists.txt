cmake_minimum_required(VERSION 3.12)
project(cmdb_service)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Опции для статической линковки (если нужно)
set(Boost_USE_STATIC_LIBS ON)
# set(Boost_USE_STATIC_RUNTIME ON)  # Используйте только если хотите статический рантайм

# Поиск Boost
find_package(Boost 1.67 REQUIRED COMPONENTS system thread json program_options)

# Добавляем исполняемый файл
add_executable(cmdb_service 
    main.cpp
    Server/Server.cpp
    Server/ThreadPool/ThreadPool.cpp
    Server/Model/DataStore.cpp
    Server/View/ResponseFormatter.cpp
    Server/Controller/RequestHandler.cpp
    CMDB/CI.cpp
    CMDB/Relationship.cpp
    CMDB/CMDB.cpp
)

# Подключаем Boost библиотеки
if(Boost_FOUND)
    target_include_directories(cmdb_service PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(cmdb_service PRIVATE ${Boost_LIBRARIES})
endif()

# Устанавливаем определения для Boost
target_compile_definitions(cmdb_service PRIVATE -DBOOST_ALL_NO_LIB)

# Включение всех необходимых заголовочных директорий
# Это уже обрабатывается через target_include_directories для конкретных целей

set_target_properties(cmdb_service PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
