#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QSet>
#include <QHash>

/**
 * @file datatypes.h
 * @author Сапунков А.Р.
 * @brief Описание структур данных согласно Внутренней спецификации.
 */

// Перечисление системных ошибок
enum class ErrorType {
    NoError,            // ошибок нет
    InputFileNotFound,  // указанный входной файл не существует
    OutputCreateFail,   // невозможно создать выходной файл
    NotANumber,         // параметр не является числом
    OutOfRange,         // число вне диапазона реальных физических ограничений
    DotSyntaxError      // ошибка структуры DOT-файла
};

// Перечисление статусов поиска пути
enum class RouteStatus {
    PathFound,          // кратчайший путь успешно найден
    NoRouteExists,      // физического пути между пунктами нет вообще
    RouteImpossible     // путь есть, но грузовик не проходит по габаритам
};

// Класс для хранения и генерации ошибок
class Error {
public:
    ErrorType type;
    QString errorDetail;

    Error() : type(ErrorType::NoError), errorDetail("") {}
    Error(ErrorType t, const QString& detail) : type(t), errorDetail(detail) {}

    // Метод возврата готового сообщения
    QString generateErrorMessage() const {
        if (type == ErrorType::NoError) return "";
        return errorDetail;
    }

    // Оператор сравнения (нужен для QSet)
    bool operator==(const Error& other) const {
        return type == other.type && errorDetail == other.errorDetail;
    }
};

// Хэш-функция (нужна для работы QSet<Error>)
inline uint qHash(const Error& key, uint seed = 0) {
    return qHash(static_cast<int>(key.type), seed) ^ qHash(key.errorDetail, seed);
}

// Описание дороги
struct Edge {
    double length;
    double maxMass;
    double maxHeight;

    Edge() : length(0.0), maxMass(0.0), maxHeight(0.0) {}
    Edge(double l, double m, double h) : length(l), maxMass(m), maxHeight(h) {}
};

// Описание габаритов грузовика
struct Truck {
    double mass;
    double height;
};

// Предварительное объявление класса Graph для функции generateDotRoute
class Graph;

#endif // DATATYPES_H
