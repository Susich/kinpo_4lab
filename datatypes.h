#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <exception>
#include <string>

/**
 * @file datatypes.h
 * @author Сапунков А.Р (ПрИн-266)
 * @brief Заголовочный файл с определениями базовых типов данных, структур и класса ошибок.
 */

/**
 * @enum ErrorType
 * @brief Перечисление возможных типов ошибок в программе.
 */
enum class ErrorType {
    FileOpenError,    ///< Ошибка при открытии файла (чтение/запись)
    ParseError,       ///< Ошибка синтаксического анализа (некорректный DOT или параметры)
    GraphError,       ///< Ошибки, связанные с логикой графа (например, несуществующие узлы)
    InvalidArgument   ///< Неверно заданные аргументы
};

/**
 * @enum RouteStatus
 * @brief Перечисление статусов поиска маршрута (для логики Дейкстры).
 */
enum class RouteStatus {
    Success,          ///< Маршрут успешно найден с учетом габаритов и массы
    RouteImpossible,  ///< Маршрут существует, но грузовик не проходит по габаритам/массе
    NoRouteExists     ///< Маршрут между пунктами в принципе не существует
};

/**
 * @struct Edge
 * @brief Структура, описывающая ребро графа (дорогу между пунктами).
 */
struct Edge {
    QString targetNode;  ///< Название целевого пункта
    double distance;     ///< Длина пути (вес ребра)
    double maxWeight;    ///< Максимально допустимая масса грузовика (в тоннах)
    double maxHeight;    ///< Максимально допустимая высота грузовика (в метрах)
};

/**
 * @struct Truck
 * @brief Структура, описывающая параметры грузовика и точки его маршрута.
 */
struct Truck {
    QString startNode;   ///< Начальный пункт отправления
    QString endNode;     ///< Конечный пункт назначения
    double weight;       ///< Фактическая масса грузовика
    double height;       ///< Фактическая высота грузовика
};

/**
 * @class Error
 * @brief Класс для обработки исключений. Наследуется от std::exception.
 * Содержит детальную информацию об ошибке, включая позиционирование в файле.
 */
class Error : public std::exception {
private:
    ErrorType type;       ///< Тип возникшей ошибки
    QString details;      ///< Детальное описание причины
    int line;             ///< Номер строки, где произошла ошибка (-1 если неизвестно)
    int column;           ///< Номер столбца, где произошла ошибка (-1 если неизвестно)
    std::string message;  ///< Сформированное сообщение для метода what()

public:
    /**
     * @brief Конструктор класса Error
     * @param errorType Тип ошибки
     * @param errorDetails Подробное текстовое описание
     * @param errorLine Строка (по умолчанию -1)
     * @param errorColumn Столбец (по умолчанию -1)
     */
    Error(ErrorType errorType, const QString& errorDetails, int errorLine = -1, int errorColumn = -1)
        : type(errorType), details(errorDetails), line(errorLine), column(errorColumn) {

        QString typeStr;
        switch (type) {
            case ErrorType::FileOpenError: typeStr = "Ошибка открытия файла"; break;
            case ErrorType::ParseError: typeStr = "Ошибка синтаксического анализа"; break;
            case ErrorType::GraphError: typeStr = "Ошибка графа"; break;
            case ErrorType::InvalidArgument: typeStr = "Неверный аргумент"; break;
            default: typeStr = "Неизвестная ошибка"; break;
        }

        QString fullMessage = QString("[%1] %2").arg(typeStr, details);

        // Формирование строки с позицией, если она указана
        if (line != -1) {
            fullMessage += QString(" (Строка: %1").arg(line);
            if (column != -1) {
                fullMessage += QString(", Столбец: %1").arg(column);
            }
            fullMessage += ")";
        }

        // Сохраняем как std::string, чтобы безопасно возвращать C-строку в what()
        message = fullMessage.toStdString();
    }

    /**
     * @brief Переопределенный метод стандартного исключения
     * @return C-строка с полным описанием ошибки
     */
    const char* what() const noexcept override {
        return message.c_str();
    }

    /**
     * @brief Получить тип ошибки
     * @return ErrorType
     */
    ErrorType getType() const { return type; }

    /**
     * @brief Получить детали ошибки
     * @return QString
     */
    QString getDetails() const { return details; }

    /**
     * @brief Получить номер строки ошибки
     * @return int
     */
    int getLine() const { return line; }

    /**
     * @brief Получить номер столбца ошибки
     * @return int
     */
    int getColumn() const { return column; }
};

#endif // DATATYPES_H
