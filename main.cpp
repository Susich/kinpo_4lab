#include <QCoreApplication>
#include <QTextStream>
#include <iostream>
#include <clocale>
#include "datatypes.h"
#include "utils.h"
#include "graph.h"

/**
 * @file main.cpp
 * @author Сапунков А.Р.
 * @brief Главный файл программы согласно Внутренней спецификации.
 */

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    std::setlocale(LC_ALL, "Russian");

    if (argc < 4) {
        std::cerr << "Ошибка: Недостаточно аргументов. Использование: <program> <map.dot> <truck.txt> <result.dot>" << std::endl;
        return 1;
    }

    QString mapFile = QString::fromLocal8Bit(argv[1]);
    QString truckFile = QString::fromLocal8Bit(argv[2]);
    QString outputFile = QString::fromLocal8Bit(argv[3]);

    // 1. Чтение и парсинг параметров грузовика
    QString truckContent;
    Error err = readFile(truckFile, truckContent);
    if (err.type != ErrorType::NoError) {
        std::cerr << err.generateErrorMessage().toStdString() << std::endl;
        return 1;
    }

    Truck truck;
    int startNode, endNode;
    err = parseTruckData(truckContent, truck, startNode, endNode);
    if (err.type != ErrorType::NoError) {
        std::cerr << err.generateErrorMessage().toStdString() << std::endl;
        return 1;
    }

    // 2. Чтение DOT файла
    QString mapContent;
    err = readFile(mapFile, mapContent);
    if (err.type != ErrorType::NoError) {
        std::cerr << err.generateErrorMessage().toStdString() << std::endl;
        return 1;
    }

    // 3. Создание графа
    QSet<Error> errors;
    Graph graph(mapContent, errors);

    if (!errors.isEmpty()) {
        for (const Error& e : errors) {
            std::cerr << e.generateErrorMessage().toStdString() << std::endl;
        }
        return 1;
    }

    // 4. Поиск пути
    QList<int> path;
    double totalLen, requiredMass, requiredHeight;
    RouteStatus status = graph.findShortestPath(startNode, endNode, truck, path, totalLen, requiredMass, requiredHeight);

    // 5. Обработка результатов
    if (status == RouteStatus::NoRouteExists) {
        std::cerr << "Маршрут не существует: пункты " << startNode << " и " << endNode << " никак не соединены дорогами." << std::endl;
        return 1;
    } else if (status == RouteStatus::RouteImpossible) {
        std::cerr << "Маршрут невозможен: требуется для проезда масса <= " << requiredMass
                  << ", высота <= " << requiredHeight << ". Ваш автомобиль: масса = "
                  << truck.mass << ", высота = " << truck.height << "." << std::endl;
        return 1;
    }

    // Если путь найден - генерируем DOT
    QString resultDot = generateDotRoute(graph, path, totalLen);
    err = writeToFile(outputFile, resultDot);

    if (err.type != ErrorType::NoError) {
        std::cerr << err.generateErrorMessage().toStdString() << std::endl;
        return 1;
    }

    std::cout << "Успех! Маршрут найден и сохранен в файл: " << outputFile.toStdString() << std::endl;
    return 0;
}
