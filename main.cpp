
//#include <QCoreApplication>
//#include <QTextStream>
//#include <iostream>
//#include <clocale>
//#include "datatypes.h"
//#include "utils.h"
//#include "graph.h"

///**
// * @file main.cpp
// * @author Сапунков А.Р.
// * @brief Главный файл программы. Точка входа с глобальным обработчиком исключений.
// */

//int main(int argc, char *argv[]) {
//    // Инициализация Qt-приложения
//    QCoreApplication app(argc, argv);

//    // Установка локали для корректного вывода на русском языке в консоли
//    std::setlocale(LC_ALL, "Russian");

//    try {
//        // Проверка количества аргументов (надежность программы)
//        if (argc < 4) {
//            throw Error(ErrorType::InvalidArgument,
//                        "Недостаточно аргументов. Использование: <program> <graph.dot> <truck_params.txt> <output.txt>");
//        }

//        // Считываем пути к файлам из аргументов командной строки
//        QString graphFile = QString::fromLocal8Bit(argv[1]);
//        QString truckFile = QString::fromLocal8Bit(argv[2]);
//        QString outputFile = QString::fromLocal8Bit(argv[3]);

//        // 1. Читаем и парсим DOT файл (может выбросить FileOpenError или ParseError)
//        QString dotData = readFile(graphFile);
//        Graph graph(dotData);

//        // 2. Читаем и парсим параметры грузовика (может выбросить FileOpenError или ParseError)
//        // Ожидается, что в файле одна строка: "Старт Конец Масса Высота"
//        QString truckData = readFile(truckFile);
//        Truck truck = parseTruckData(truckData);

//        // 3. Выполняем поиск оптимального маршрута
//        QPair<RouteStatus, QStringList> result = graph.findShortestPath(truck);

//        // 4. Формируем текстовый результат
//        QString outputText;
//        if (result.first == RouteStatus::Success) {
//            outputText = "Успех! Маршрут найден: " + result.second.join(" -> ");
//        } else if (result.first == RouteStatus::RouteImpossible) {
//            outputText = "Ошибка: Маршрут существует, но грузовик не проходит по габаритам (превышена масса или высота).";
//        } else {
//            outputText = "Ошибка: Маршрут между заданными пунктами не существует.";
//        }

//        // 5. Записываем результат в файл
//        writeToFile(outputFile, outputText);

//        std::cout << "Программа успешно завершила работу. Результат записан в файл: "
//                  << outputFile.toStdString() << std::endl;

//    } catch (const Error& e) {
//        // Глобальный перехват наших пользовательских ошибок
//        std::cerr << "\n[КРИТИЧЕСКАЯ ОШИБКА ПРОГРАММЫ]" << std::endl;
//        std::cerr << e.what() << std::endl;
//        return 1; // Возвращаем ненулевой код (ошибка)

//    } catch (const std::exception& e) {
//        // Перехват стандартных системных ошибок C++ (например, std::bad_alloc при нехватке памяти)
//        std::cerr << "\n[СИСТЕМНАЯ ОШИБКА] " << e.what() << std::endl;
//        return 2;

//    } catch (...) {
//        // Перехват любых других неизвестных исключений
//        std::cerr << "\n[НЕИЗВЕСТНАЯ ОШИБКА] Произошел непредвиденный сбой." << std::endl;
//        return 3;
//    }

//    return 0; // Корректное завершение программы
//}
