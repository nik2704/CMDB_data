cmake_minimum_required(VERSION 3.12)
project(cmdb_service)

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Опции для статической линковки (если нужно)
set(Boost_USE_STATIC_LIBS OFF)
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

if(WITH_BOOST_TEST)
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)

    # Добавляем исполняемый файл для тестов
    add_executable(test_ci
        tests/CMDB/test_ci.cpp
        CMDB/CI.cpp
    )

    add_executable(test_relationship
        tests/CMDB/test_relationship.cpp
        CMDB/Relationship.cpp
    )


    # Линкуем Boost с исполняемым файлом
    target_link_libraries(test_ci
        Boost::unit_test_framework
        Boost::json
    )

    target_link_libraries(test_relationship
        Boost::unit_test_framework
        Boost::json
    )

    # Устанавливаем параметры компилятора
    set_target_properties(test_ci PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    set_target_properties(test_relationship PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    target_include_directories(test_ci PRIVATE ${Boost_INCLUDE_DIRS})

    target_include_directories(test_relationship PRIVATE ${Boost_INCLUDE_DIRS})

    # Регистрация теста в CTest
    enable_testing()
    add_test(NAME test_ci COMMAND test_ci)
    add_test(NAME test_relationship COMMAND test_relationship)

endif()


message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
