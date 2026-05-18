#include <QCoreApplication>
#include <iostream>
#include <cstdlib>
#include "datatypes.h"
#include "utils.h"
#include "graph.h"
/**
 * @mainpage Расчет маршрута грузового автомобиля (Лабораторная работа №4)
 * * @section intro_sec Введение
 * Программа предназначена для расчёта кратчайшего маршрута грузового автомобиля между двумя пунктами
 * с учётом ограничений по высоте и массе. Реализована на базе алгоритма Дейкстры.
 * * @section author_sec Разработчик
 * Выполнил: студент группы ПрИн-266 Сапунков А.Р.\n
 * Проверил: Кулюкин К.С.
 */
/**
 * @file main.cpp
 * @author Сапунков А.Р.
 * @brief Главный файл программы согласно Внутренней спецификации.
 */

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);


#ifdef Q_OS_WIN
    std::system("chcp 65001 > nul");
#endif

    if (argc < 4) {
        std::cerr << "Ошибка: Недостаточно аргументов. Использование: <program> <map.dot> <truck.txt> <result.dot>\n";
        return 1;
    }

    QString mapFile = QString::fromLocal8Bit(argv[1]);
    QString truckFile = QString::fromLocal8Bit(argv[2]);
    QString outputFile = QString::fromLocal8Bit(argv[3]);

    // 1. Чтение и парсинг параметров грузовика
    QString truckContent;
    Error err = readFile(truckFile, truckContent);
    if (err.type != ErrorType::NoError) {
        std::cerr << err.generateErrorMessage().toStdString() << "\n";
        return 1;
    }

    Truck truck;
    int startNode, endNode;
    err = parseTruckData(truckContent, truck, startNode, endNode);
    if (err.type != ErrorType::NoError) {
        std::cerr << err.generateErrorMessage().toStdString() << "\n";
        return 1;
    }

    // 2. Чтение DOT файла
    QString mapContent;
    err = readFile(mapFile, mapContent);
    if (err.type != ErrorType::NoError) {
        std::cerr << err.generateErrorMessage().toStdString() << "\n";
        return 1;
    }

    // 3. Создание графа
    QSet<Error> errors;
    Graph graph(mapContent, errors);

    if (!errors.isEmpty()) {
        for (const Error& e : errors) {
            std::cerr << e.generateErrorMessage().toStdString() << "\n";
        }
        return 1;
    }

    // 4. Поиск пути
        QList<int> path;
        double totalLen, requiredMass, requiredHeight;
        RouteStatus status = graph.findShortestPath(startNode, endNode, truck, path, totalLen, requiredMass, requiredHeight);

        QString errorMsgForDot = "";

        // 5. Обработка результатов и формирование текста ошибки
        if (status == RouteStatus::NoRouteExists) {
            errorMsgForDot = QString("Ошибка: Маршрут не существует.\nПункты %1 и %2 никак не соединены дорогами.").arg(startNode).arg(endNode);
            std::cerr << errorMsgForDot.toStdString() << "\n";
        } else if (status == RouteStatus::RouteImpossible) {
            errorMsgForDot = QString("Ошибка: Маршрут невозможен из-за ограничений.\nТребуется для проезда: масса <= %1, высота <= %2.\nВаш автомобиль: масса = %3, высота = %4.")
                              .arg(requiredMass, 0, 'f', 1).arg(requiredHeight, 0, 'f', 1).arg(truck.mass, 0, 'f', 1).arg(truck.height, 0, 'f', 1);
            std::cerr << errorMsgForDot.toStdString() << "\n";
        }

        // 6. ВСЕГДА генерируем DOT (и при успехе, и при ошибке)
        QString resultDot = generateDotRoute(graph, path, totalLen, status, errorMsgForDot);
        err = writeToFile(outputFile, resultDot);

        if (err.type != ErrorType::NoError) {
            std::cerr << err.generateErrorMessage().toStdString() << "\n";
            return 1;
        }

        if (status == RouteStatus::PathFound) {
            std::cout << "Успех! Маршрут найден и сохранен в файл: " << outputFile.toStdString() << "\n";
            return 0;
        } else {
            std::cout << "Граф с описанием ошибки сохранен в файл: " << outputFile.toStdString() << "\n";
            return 1; // Завершаем с ошибкой, так как физически маршрут не проложен
        }
}
