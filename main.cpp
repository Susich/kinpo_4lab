#include <QCoreApplication>
#include <QTextStream>
#include "datatypes.h"
#include "utils.h"
#include "graph.h"

// Подключаем API Windows для настройки кодировки консоли
#ifdef Q_OS_WIN
#include <windows.h>
#endif

/**
 * @file main.cpp
 * @author Сапунков А.Р.
 * @brief Главный файл программы согласно Внутренней спецификации.
 */

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Принудительно ставим консоли Windows кодировку UTF-8 (65001)
#ifdef Q_OS_WIN
    SetConsoleOutputCP(65001);
#endif

    // Настраиваем потоки вывода Qt
    QTextStream qout(stdout);
    QTextStream qerr(stderr);

    if (argc < 4) {
        qerr << "Ошибка: Недостаточно аргументов. Использование: <program> <map.dot> <truck.txt> <result.dot>\n";
        qerr.flush();
        return 1;
    }

    QString mapFile = QString::fromLocal8Bit(argv[1]);
    QString truckFile = QString::fromLocal8Bit(argv[2]);
    QString outputFile = QString::fromLocal8Bit(argv[3]);

    // 1. Чтение и парсинг параметров грузовика
    QString truckContent;
    Error err = readFile(truckFile, truckContent);
    if (err.type != ErrorType::NoError) {
        qerr << err.generateErrorMessage() << "\n";
        qerr.flush();
        return 1;
    }

    Truck truck;
    int startNode, endNode;
    err = parseTruckData(truckContent, truck, startNode, endNode);
    if (err.type != ErrorType::NoError) {
        qerr << err.generateErrorMessage() << "\n";
        qerr.flush();
        return 1;
    }

    // 2. Чтение DOT файла
    QString mapContent;
    err = readFile(mapFile, mapContent);
    if (err.type != ErrorType::NoError) {
        qerr << err.generateErrorMessage() << "\n";
        qerr.flush();
        return 1;
    }

    // 3. Создание графа
    QSet<Error> errors;
    Graph graph(mapContent, errors);

    if (!errors.isEmpty()) {
        for (const Error& e : errors) {
            qerr << e.generateErrorMessage() << "\n";
        }
        qerr.flush();
        return 1;
    }

    // 4. Поиск пути
    QList<int> path;
    double totalLen, requiredMass, requiredHeight;
    RouteStatus status = graph.findShortestPath(startNode, endNode, truck, path, totalLen, requiredMass, requiredHeight);

    // 5. Обработка результатов
    if (status == RouteStatus::NoRouteExists) {
        qerr << "Маршрут не существует: пункты " << startNode << " и " << endNode << " никак не соединены дорогами.\n";
        qerr.flush();
        return 1;
    } else if (status == RouteStatus::RouteImpossible) {
        qerr << "Маршрут невозможен: требуется для проезда масса <= " << requiredMass
             << ", высота <= " << requiredHeight << ". Ваш автомобиль: масса = "
             << truck.mass << ", высота = " << truck.height << ".\n";
        qerr.flush();
        return 1;
    }

    // Если путь найден - генерируем DOT
    QString resultDot = generateDotRoute(graph, path, totalLen);
    err = writeToFile(outputFile, resultDot);

    if (err.type != ErrorType::NoError) {
        qerr << err.generateErrorMessage() << "\n";
        qerr.flush();
        return 1;
    }

    qout << "Успех! Маршрут найден и сохранен в файл: " << outputFile << "\n";
    qout.flush();
    return 0;
}
