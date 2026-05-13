#include <QtTest/QtTest>
#include "datatypes.h"
#include "graph.h"
#include "utils.h"

/**
 * @file test_main.cpp
 * @author Сапунков А.Р.
 * @brief Автоматические юнит-тесты для проверки логики графа и алгоритма поиска пути.
 */

class TestGraphPathfinding : public QObject {
    Q_OBJECT

private slots:
    // Тест 1: Идеальные условия, путь существует и грузовик проходит
    void testSuccessPath() {
        // Подготавливаем валидный граф
        QString dotData = "A -> B [label=\"10.0,20.0,5.0\"]\n"
                          "B -> C [label=\"15.0,20.0,5.0\"]\n"
                          "A -> C [label=\"50.0,20.0,5.0\"]"; // Длинный, но прямой путь
        Graph graph(dotData);

        // Масса 15, высота 4 - везде проходит
        Truck truck = {"A", "C", 15.0, 4.0};

        QPair<RouteStatus, QStringList> result = graph.findShortestPath(truck);

        QCOMPARE(result.first, RouteStatus::Success);

        // Ожидаем короткий путь через B (10+15 = 25), а не напрямую (50)
        QStringList expectedPath = {"A", "B", "C"};
        QCOMPARE(result.second, expectedPath);
    }

    // Тест 2: Путь физически есть, но грузовик слишком тяжелый для одного из мостов
    void testRouteImpossible() {
        // Дорога из A в B выдерживает только 10 тонн
        QString dotData = "A -> B [label=\"10.0,10.0,5.0\"]";
        Graph graph(dotData);

        // Наш грузовик весит 15 тонн
        Truck truck = {"A", "B", 15.0, 4.0};

        QPair<RouteStatus, QStringList> result = graph.findShortestPath(truck);

        // Алгоритм должен понять, что дорога есть, но ехать нельзя
        QCOMPARE(result.first, RouteStatus::RouteImpossible);
    }

    // Тест 3: Между пунктами вообще нет связи (разные компоненты связности)
    void testNoRouteExists() {
        QString dotData = "A -> B [label=\"10.0,20.0,5.0\"]\n"
                          "C -> D [label=\"15.0,20.0,5.0\"]";
        Graph graph(dotData);

        // Пытаемся проехать из A в D
        Truck truck = {"A", "D", 10.0, 4.0};

        QPair<RouteStatus, QStringList> result = graph.findShortestPath(truck);

        QCOMPARE(result.first, RouteStatus::NoRouteExists);
    }

    // Тест 4: Проверка надежности при некорректных входных данных графа
    void testParseErrorException() {
        // Намеренно портим DOT файл (не хватает параметра высоты)
        QString badDotData = "A -> B [label=\"10.0,20.0\"]";

        // Макрос QVERIFY_EXCEPTION_THROWN проверяет, что конструктор выбросит нашу ошибку
        QVERIFY_EXCEPTION_THROWN(Graph graph(badDotData), Error);
    }
};

// Макрос генерации функции main() специально для запуска тестов без GUI
QTEST_APPLESS_MAIN(TestGraphPathfinding)

// Включение moc-файла (необходимо для работы системы мета-объектов Qt в одном cpp файле)
#include "test_main.moc"
