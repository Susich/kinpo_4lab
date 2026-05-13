#include "utils.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

QString readFile(const QString& filePath) {
    QFile file(filePath);

    // Проверка на возможность открытия файла (надежность программы)
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw Error(ErrorType::FileOpenError, "Не удалось открыть файл для чтения: " + filePath);
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

void writeToFile(const QString& filePath, const QString& data) {
    QFile file(filePath);

    // Проверка на возможность записи в файл
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw Error(ErrorType::FileOpenError, "Не удалось открыть файл для записи: " + filePath);
    }

    QTextStream out(&file);
    out << data;
    file.close();
}

Truck parseTruckData(const QString& truckDataString) {
    // Регулярное выражение для строгого контроля формата ввода
    // Ожидаем: <Старт> <Конец> <Масса> <Высота>. Допускаются пробелы по краям.
    QRegularExpression regex("^\\s*(\\w+)\\s+(\\w+)\\s+([0-9]*\\.?[0-9]+)\\s+([0-9]*\\.?[0-9]+)\\s*$");
    QRegularExpressionMatch match = regex.match(truckDataString);

    if (!match.hasMatch()) {
        throw Error(ErrorType::ParseError,
                    "Неверный формат данных грузовика. Ожидается: <Старт> <Конец> <Масса> <Высота>.");
    }

    Truck truck;
    truck.startNode = match.captured(1);
    truck.endNode = match.captured(2);

    // Локальные переменные для проверки корректности перевода строки в число
    bool weightOk = false;
    bool heightOk = false;

    truck.weight = match.captured(3).toDouble(&weightOk);
    truck.height = match.captured(4).toDouble(&heightOk);

    if (!weightOk || !heightOk) {
        throw Error(ErrorType::ParseError, "Критическая ошибка преобразования числовых значений массы или высоты.");
    }

    return truck;
}
