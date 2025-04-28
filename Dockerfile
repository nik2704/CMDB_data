FROM ubuntu:latest

# Обновление пакетов и установка необходимых утилит
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    build-essential \
    cmake \
    libboost-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-json-dev \
    libboost-program-options-dev \
    wget \
    ca-certificates \
    --no-install-recommends

# Создание рабочей директории
WORKDIR /app

# Скачать готовый релиз
RUN wget -O cmdb_service.deb https://github.com/nik2704/CMDB_data/releases/download/v7/cmdb_service-0.0.7-Linux.deb

# Установить DEB-пакет
RUN apt-get install -y ./cmdb_service.deb

# Очистить ненужное
RUN rm cmdb_service.deb

# Определение команды для запуска приложения
CMD ["/usr/bin/cmdb_service"]

# Проброс порта
EXPOSE 8080
