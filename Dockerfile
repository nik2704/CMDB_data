FROM ubuntu:latest

# Обновление пакетов
RUN apt-get update && apt-get upgrade -y

# Установка зависимостей, которые нужны для работы приложения (но не для сборки!)
RUN apt-get install -y \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-json-dev \
    libboost-program-options-dev \
    wget \
    dpkg \
    --no-install-recommends

# Создание рабочей директории
WORKDIR /app

# Скачать готовый релиз
RUN wget -O cmdb_service.deb https://github.com/nik2704/CMDB_data/releases/download/v6/cmdb_service-0.0.1-Linux.deb

# Установить DEB-пакет
RUN dpkg -i cmdb_service.deb || apt-get install -fy

# (если нужно, создать директорию для запуска)
WORKDIR /app/output

# Определение команды для запуска приложения
CMD ["/usr/bin/cmdb_service"]

# Определение порта
EXPOSE 8080
