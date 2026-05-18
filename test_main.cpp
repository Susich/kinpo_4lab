#include <QtTest/QtTest>
#include "datatypes.h"
#include "graph.h"
#include "utils.h"

/**
 * @file test_main.cpp
 * @author Сапунков А.Р.
 * @brief Полный набор модульных тестов согласно Программе и методике испытаний (Таблицы 1-4).
 */

class TestGraphPathfinding : public QObject {
    Q_OBJECT

private slots:

    // =========================================================================
    // ТАБЛИЦА 1: Тесты функции parseTruckData
    // =========================================================================

    void testParseTruckData_1_Success() {
        QString content = "12.5 3.2 1 4";
        Truck truck; int start, end;
        Error err = parseTruckData(content, truck, start, end);
        QCOMPARE(err.type, ErrorType::NoError);
        QCOMPARE(truck.mass, 12.5);
        QCOMPARE(truck.height, 3.2);
        QCOMPARE(start, 1);
        QCOMPARE(end, 4);
    }

    void testParseTruckData_2_Letters() {
        QString content = "12.5 3m 1 4";
        Truck truck; int start, end;
        Error err = parseTruckData(content, truck, start, end);
        QCOMPARE(err.type, ErrorType::NotANumber);
    }

    void testParseTruckData_3_Missing() {
        QString content = "12.5 3.2 1";
        Truck truck; int start, end;
        Error err = parseTruckData(content, truck, start, end);
        QCOMPARE(err.type, ErrorType::NotANumber);
    }

    void testParseTruckData_4_Empty() {
        QString content = "";
        Truck truck; int start, end;
        Error err = parseTruckData(content, truck, start, end);
        QCOMPARE(err.type, ErrorType::NotANumber);
    }

    // =========================================================================
    // ТАБЛИЦА 2: Тесты функции findShortestPath
    // =========================================================================

    void testFindPath_1_Direct() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=20, max_height=5]; 2 -- 3 [length=10, max_mass=20, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {8.0, 3.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 3, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(path, QList<int>({1, 2, 3}));
        QCOMPARE(len, 15.0);
    }

    void testFindPath_2_MassBlocked() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=20, max_height=5]; 2 -- 4 [length=5, max_mass=10, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {15.0, 3.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 4, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::RouteImpossible);
        QCOMPARE(rm, 10.0);
    }

    void testFindPath_3_HeightBlocked() {
        QString dot = "graph { 1 -- 2 [length=10, max_mass=20, max_height=6.0]; 2 -- 3 [length=10, max_mass=20, max_height=4.0]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {10.0, 5.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 3, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::RouteImpossible);
        QCOMPARE(rh, 4.0);
    }

    void testFindPath_4_Bypass() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=5, max_height=5]; 2 -- 4 [length=5, max_mass=5, max_height=5]; "
                      "1 -- 3 [length=10, max_mass=20, max_height=5]; 3 -- 4 [length=15, max_mass=20, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {12.0, 4.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 4, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(path, QList<int>({1, 3, 4}));
        QCOMPARE(len, 25.0);
    }

    void testFindPath_5_NoConnection() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=20, max_height=5]; 3 -- 4 [length=5, max_mass=20, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {10.0, 4.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 5, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::NoRouteExists);
    }

    void testFindPath_6_OptimalChoice() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=50, max_height=10]; 2 -- 4 [length=5, max_mass=50, max_height=10]; "
                      "1 -- 3 [length=10, max_mass=50, max_height=10]; 3 -- 4 [length=10, max_mass=50, max_height=10]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {10.0, 4.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 4, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(path, QList<int>({1, 2, 4}));
        QCOMPARE(len, 10.0);
    }

    void testFindPath_7_EqualPaths() {
        QString dot = "graph { 1 -- 2 [length=10, max_mass=50, max_height=10]; 2 -- 4 [length=10, max_mass=50, max_height=10]; "
                      "1 -- 3 [length=10, max_mass=50, max_height=10]; 3 -- 4 [length=10, max_mass=50, max_height=10]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {10.0, 4.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 4, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(len, 20.0); // Любой из путей валиден, проверяем только длину
    }

    // =========================================================================
    // ТАБЛИЦА 3: Тесты конструктора Graph
    // =========================================================================

    void testGraphConstructor_1_Success() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=10, max_height=5]; 2 -- 3 [length=8, max_mass=15, max_height=4]; }";
        QSet<Error> errs; Graph g(dot, errs);
        QVERIFY(errs.isEmpty());
        QVERIFY(g.getAdjacencyMap().contains(1));
        QVERIFY(g.getAdjacencyMap().contains(3));
    }

    void testGraphConstructor_2_MultipleErrors() {
            QString dot = "graph { 1 -- 2 [length=-5, max_mass=10]; }";
            QSet<Error> errs; Graph g(dot, errs);
            QVERIFY(errs.size() >= 2);
            QVERIFY(g.getAdjacencyMap().isEmpty());
        }

    void testGraphConstructor_3_NegativeLimits() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=-10, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        // Не добавляет дорогу, если лимиты некорректны
        QVERIFY(g.getAdjacencyMap().isEmpty());
    }

    void testGraphConstructor_4_IgnoreLoops() {
        QString dot = "graph { 1 -- 1 [length=5, max_mass=10, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        QVERIFY(g.getAdjacencyMap().isEmpty()); // Петля проигнорирована
    }

    void testGraphConstructor_5_MissingParam() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=10]; }";
        QSet<Error> errs; Graph g(dot, errs);
        bool hasSyntaxError = false;
        for(const auto& e : errs) if(e.type == ErrorType::DotSyntaxError) hasSyntaxError = true;
        QVERIFY(hasSyntaxError);
        QVERIFY(g.getAdjacencyMap().isEmpty());
    }

    void testGraphConstructor_6_TextInsteadOfNumber() {
        QString dot = "graph { 1 -- 2 [length=five, max_mass=10, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        bool hasSyntaxError = false;
        for(const auto& e : errs) if(e.type == ErrorType::DotSyntaxError) hasSyntaxError = true;
        QVERIFY(hasSyntaxError);
    }

    // =========================================================================
    // ТАБЛИЦА 4: Тесты логики Дейкстры (через публичный метод)
    // =========================================================================

    void testDijkstra_1_CleanSearchNoLimits() {
        // Симуляция алгоритма без ограничений
        QString dot = "graph { 1 -- 2 [length=5, max_mass=10, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        QList<int> path; double len, rm, rh;
        // Передаем огромную машину, чтобы заставить сработать "чистый" поиск (Проход №2)
        Truck hugeTruck = {1000.0, 1000.0};
        RouteStatus s = g.findShortestPath(1, 2, hugeTruck, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::RouteImpossible); // Путь найден, но машина не прошла
        QCOMPARE(path, QList<int>({1, 2}));
        QCOMPARE(len, 5.0);
    }

    void testDijkstra_3_Trap() {
        QString dot = "graph { 1 -- 2 [length=1, max_mass=20, max_height=5]; 2 -- 3 [length=1, max_mass=2, max_height=5]; "
                      "1 -- 4 [length=10, max_mass=20, max_height=5]; 4 -- 3 [length=10, max_mass=20, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {15.0, 4.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 3, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(path, QList<int>({1, 4, 3}));
        QCOMPARE(len, 20.0);
    }

    void testDijkstra_4_TotalBlock() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=5, max_height=5]; 1 -- 3 [length=5, max_mass=6, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {10.0, 4.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 2, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::RouteImpossible); // Отказ в проезде
        QVERIFY(!path.isEmpty()); // При RouteImpossible путь сохраняется для вывода красной рамки
    }

    void testDijkstra_5_Isolation() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=20, max_height=5]; 3 -- 4 [length=5, max_mass=20, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {10.0, 4.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 4, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::NoRouteExists); // Пути физически нет
    }

    void testDijkstra_6_StartEqualsEnd() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=20, max_height=5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck t = {10.0, 4.0};
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 1, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(path, QList<int>({1}));
        QCOMPARE(len, 0.0);
    }

    void testDijkstra_8_ComplexGraph() {
        QString dot = "graph { 1 -- 3 [length=1, max_mass=30, max_height=5]; 3 -- 6 [length=5, max_mass=12, max_height=3.5]; "
                      "6 -- 7 [length=1, max_mass=40, max_height=6]; 1 -- 2 [length=4, max_mass=20, max_height=4.5]; }";
        QSet<Error> errs; Graph g(dot, errs);
        Truck hugeTruck = {1000.0, 1000.0}; // Вызовет чистый поиск
        QList<int> path; double len, rm, rh;
        RouteStatus s = g.findShortestPath(1, 7, hugeTruck, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::RouteImpossible);
        QCOMPARE(path, QList<int>({1, 3, 6, 7}));
        QCOMPARE(len, 7.0);
        QCOMPARE(rm, 12.0); // Узкое место по весу на 3--6
        QCOMPARE(rh, 3.5);  // Узкое место по высоте на 3--6
    }
};

QTEST_APPLESS_MAIN(TestGraphPathfinding)
#include "test_main.moc"
