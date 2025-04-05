FROM ubuntu:latest

# Обновление пакетов
RUN apt-get update && apt-get upgrade -y

# Установка необходимых зависимостей
RUN apt-get install -y \
    build-essential \
    cmake \
    libboost-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-json-dev \
    libboost-program-options-dev \
    --no-install-recommends

# Создание рабочей директории
WORKDIR /app

# Копирование исходного кода
COPY . /app

# Создание директории сборки
RUN mkdir build && cd build

# Сборка проекта с CMake
RUN cmake ..
RUN make -j $(nproc)

# Создание директории для артефактов (опционально)
RUN mkdir -p /app/output

# Копирование собранного исполняемого файла в директорию артефактов
COPY build/cmdb_service /app/output/

# Определение команды для запуска приложения
CMD ["/app/output/cmdb_service"]

# Определение порта, который слушает приложение (для документации)
EXPOSE 8080
