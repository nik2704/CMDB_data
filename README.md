# Работа с данными в формате JSON
Этот проект предоставляет инструменты для работы с данными в формате JSON в C++. Он включает в себя механизмы для парсинга, построения и преобразования JSON-документов, а также утилиту для командной строки.

## Краткое описание
Проект состоит из нескольких частей, которые позволяют:

- Парсить и валидировать JSON-документы.
- Строить сложные JSON-структуры с помощью Builder.
- Выполнять различные действия с JSON через командную строку.

##Пример команд
Командный интерфейс утилиты simple_json:

1. Получение справки
```bah
simple_json -h

Usage:
  --output-file (string, optional, default: ): Path to the output file
  --input-file (string, mandatory, default: ): Path to the input file
  --output (string, mandatory, default: console): Output type: console | file
  --action (string, mandatory, default: check-json): Action to perform: check-json | string-to-json | csv-to-json

```

2. Проверка формата даннх
```bah
simple_json --action=check-json --input-file=data/data.txt
```

3. Конвертация даннх
```bah
simple_json --action=string-to-json --input-file=data/data.txt --output-file=data/data.json
```

4. Преобразование CSV в JSON
```bah
simple_json --action=csv-to-json --input-file=data/data.csv --output-file=data/data.json --output=file
```

## Состав проекта
- ArgumentParser
- JSON Parser
- JSON Builder

### Требования
- Компилятор с поддержкой C++17 и выше

--------------------------------
## ArgumentParser

### Описание

Vеханизм для хранения параметров программы и их описания, а также для парсинга аргументов командной строки с валидацией и установкой значений по умолчанию.

### Структура кода

Проект состоит из двух основных классов:

1. `ProgramParameters` — отвечает за хранение параметров программы, их описание, обязательность и значения по умолчанию.
2. `ArgumentParser` — обрабатывает аргументы командной строки, валидирует их и применяет значения по умолчанию.

--------------------------------

## JSON Parser

### Описание
Класс для работы с JSON в C++. Он включает классы для хранения, обработки и парсинга JSON-документов.

### Возможности
- Представление JSON-данных с использованием класса `Node`
- Поддержка различных типов данных: `null`, `bool`, `int`, `double`, `string`, `array`, `object`
- Загрузка JSON из потока
- Вывод JSON в поток
- Операции сравнения JSON-узлов и документов

### Структура кода
- **`Node`** — универсальный контейнер для JSON-значений.
- **`Document`** — класс для представления JSON-документа.
- **`Load(std::istream&)`** — загрузка JSON из потока.
- **`Print(const Document&, std::ostream&)`** — вывод JSON в поток.

### Использование
```cpp
#include "json.h"
#include <iostream>
#include <sstream>

int main() {
    std::istringstream json_input("{\"key\": 42}");
    json::Document doc = json::Load(json_input);
    std::cout << "Value: " << doc.GetRoot().AsMap().at("key").AsInt() << std::endl;
    return 0;
}
```

--------------------------------

## JSON Builder

### Описание
`json::Builder` — это класс для пошагового построения JSON-объектов. Он позволяет строить сложные структуры JSON с помощью цепочечных вызовов методов.

### Возможности
- Построение JSON-объекта без ошибок синтаксиса.
- Поддержка вложенных словарей и массивов.
- Безопасное API с классами-контекстами, предотвращающее некорректное использование.

### Использование
Пример создания JSON-объекта:
```cpp
#include "json_builder.h"
#include "json.h"

int main() {
    json::Builder builder;
    json::Node result = builder
        .StartDict()
            .Key("name").Value("Alice")
            .Key("age").Value(25)
            .Key("hobbies").StartArray()
                .Value("reading")
                .Value("coding")
            .EndArray()
        .EndDict()
        .Build();
}
```

Выходной JSON:
```json
{
  "name": "Alice",
  "age": 25,
  "hobbies": ["reading", "coding"]
}
```

### Методы
#### Основные методы `Builder`
| Метод | Описание |
|--------|-----------|
| `StartDict()` | Начинает новый JSON-объект `{}` |
| `StartArray()` | Начинает новый JSON-массив `[]` |
| `Key(std::string key)` | Добавляет ключ в объект |
| `Value(Node::Value value)` | Добавляет значение в текущий контейнер |
| `EndDict()` | Завершает объект `{}` |
| `EndArray()` | Завершает массив `[]` |
| `Build()` | Завершает построение JSON и возвращает `Node` |

#### Классы-контексты
| Класс | Описание |
|--------|-----------|
| `BaseContext` | Базовый класс, содержащий методы построения |
| `DictValueContext` | Контекст внутри словаря, когда ожидается значение |
| `DictItemContext` | Контекст внутри словаря, когда ожидается ключ |
| `ArrayItemContext` | Контекст внутри массива, когда ожидается значение |

## Лицензия
Этот проект распространяется под MIT License.

