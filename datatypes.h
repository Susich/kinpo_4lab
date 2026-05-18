#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QSet>
#include <QHash>

/**
 * @file datatypes.h
 * @author Сапунков А.Р.
 * @brief Описание базовых структур данных и перечислений системы.
 * @details Содержит перечисления типов ошибок, статусов маршрутов,
 * а также структуры для моделирования дорожной сети и транспортного средства.
 */

/**
 * @brief Перечисление типов системных и синтаксических ошибок
 */
enum class ErrorType {
    NoError,            ///< Ошибок не обнаружено
    InputFileNotFound,  ///< Указанный входной файл не найден на диске
    OutputCreateFail,   ///< Не удалось создать выходной файл (отказ в доступе)
    NotANumber,         ///< Текстовый параметр не может быть преобразован в число
    OutOfRange,         ///< Значение числа выходит за рамки физических лимитов
    DotSyntaxError      ///< Нарушен синтаксис или структура входного DOT-файла
};

/**
 * @brief Перечисление возможных статусов поиска маршрута
 */
enum class RouteStatus {
    PathFound,          ///< Кратчайший путь успешно найден, габариты подходят
    NoRouteExists,      ///< Физический путь между пунктами отсутствует (граф не связен)
    RouteImpossible     ///< Путь существует, но проезд заблокирован ограничениями габаритов
};

/**
 * @brief Класс для обработки, хранения и генерации сообщений об ошибках
 */
class Error {
public:
    ErrorType type;        ///< Категория (тип) возникшей ошибки
    QString errorDetail;   ///< Конкретизированный текст ошибки с деталями

    /**
     * @brief Конструктор по умолчанию (создает объект без ошибки)
     */
    Error() : type(ErrorType::NoError), errorDetail("") {}

    /**
     * @brief Инициализирующий конструктор ошибки
     * @param[in] t Тип регистрируемой ошибки
     * @param[in] detail Подробное текстовое описание причины сбоя
     */
    Error(ErrorType t, const QString& detail) : type(t), errorDetail(detail) {}

    /**
     * @brief Формирует и возвращает итоговую строку сообщения об ошибке
     * @return QString Текст ошибки для вывода в поток std::cerr
     */
    QString generateErrorMessage() const {
        if (type == ErrorType::NoError) return "";
        return errorDetail;
    }

    /**
     * @brief Оператор проверки равенства двух объектов ошибок
     * @details Необходим для корректного поиска дубликатов внутри контейнера QSet.
     * @param[in] other Сравниваемый объект ошибки
     * @return true Если типы и детали ошибок полностью совпадают
     * @return false В противном случае
     */
    bool operator==(const Error& other) const {
        return type == other.type && errorDetail == other.errorDetail;
    }
};

/**
 * @brief Внешняя функция вычисления хэш-кода для класса Error
 * @details Позволяет использовать объекты Error в качестве элементов хэш-таблиц (QSet).
 * @param[in] key Объект ошибки для хэширования
 * @param[in] seed Начальное значение (соль) хэша
 * @return uint Уникальное числовое представление объекта
 */
inline uint qHash(const Error& key, uint seed = 0) {
    return qHash(static_cast<int>(key.type), seed) ^ qHash(key.errorDetail, seed);
}

/**
 * @brief Структура, моделирующая участок дороги (ребро графа)
 */
struct Edge {
    double length;    ///< Геометрическая длина участка дороги (вес ребра графа)
    double maxMass;   ///< Предельно допустимая масса транспортного средства на участке (кг)
    double maxHeight; ///< Предельно допустимая высота транспортного средства на участке (см)

    /**
     * @brief Конструктор по умолчанию (инициализирует поля нулями)
     */
    Edge() : length(0.0), maxMass(0.0), maxHeight(0.0) {}

    /**
     * @brief Инициализирующий конструктор ребра графа
     * @param[in] l Длина участка
     * @param[in] m Максимально допустимая масса
     * @param[in] h Максимально допустимая высота
     */
    Edge(double l, double m, double h) : length(l), maxMass(m), maxHeight(h) {}
};

/**
 * @brief Структура, описывающая габаритные характеристики грузового автомобиля
 */
struct Truck {
    double mass;      ///< Фактическая полная масса грузовика (кг)
    double height;    ///< Фактическая максимальная высота грузовика (см)
};

class Graph;

#endif // DATATYPES_H
