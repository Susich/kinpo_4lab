
#include <QtTest/QtTest>
#include "datatypes.h"
#include "graph.h"
#include "utils.h"

/**
 * @file test_main.cpp
 * @author Сапунков А.Р.
 * @brief Модульные тесты согласно Программе и методике испытаний.
 */

class TestGraphPathfinding : public QObject {
    Q_OBJECT

private slots:

    // =========================================================================
    // ТАБЛИЦА 1: Тесты функции parseTruckData
    // =========================================================================

    void testParseTruckData_1_Success() {
        QString content = "12.5 3.2 1 4";
        Truck truck;
        int start, end;
        Error err = parseTruckData(content, truck, start, end);

        QCOMPARE(err.type, ErrorType::NoError);
        QCOMPARE(truck.mass, 12.5);
        QCOMPARE(start, 1);
        QCOMPARE(end, 4);
    }

    void testParseTruckData_2_Letters() {
        QString content = "12.5 3m 1 4";
        Truck truck;
        int start, end;
        Error err = parseTruckData(content, truck, start, end);
        QCOMPARE(err.type, ErrorType::NotANumber);
    }

    void testParseTruckData_3_Missing() {
        QString content = "12.5 3.2 1";
        Truck truck;
        int start, end;
        Error err = parseTruckData(content, truck, start, end);
        QCOMPARE(err.type, ErrorType::NotANumber);
    }

    void testParseTruckData_4_Empty() {
        QString content = "";
        Truck truck;
        int start, end;
        Error err = parseTruckData(content, truck, start, end);
        QCOMPARE(err.type, ErrorType::NotANumber);
    }

    // =========================================================================
    // ТАБЛИЦА 2: Тесты функции findShortestPath
    // =========================================================================

    void testFindPath_1_Direct() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=20, max_height=5]; 2 -- 3 [length=10, max_mass=20, max_height=5]; }";
        QSet<Error> errs;
        Graph g(dot, errs);
        Truck t = {8.0, 3.0};
        QList<int> path;
        double len, rm, rh;

        RouteStatus s = g.findShortestPath(1, 3, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(path, QList<int>({1, 2, 3}));
    }

    void testFindPath_2_MassBlocked() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=20, max_height=5]; 2 -- 4 [length=5, max_mass=10, max_height=5]; }";
        QSet<Error> errs;
        Graph g(dot, errs);
        Truck t = {15.0, 3.0};
        QList<int> path;
        double len, rm, rh;

        RouteStatus s = g.findShortestPath(1, 4, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::RouteImpossible);
        QCOMPARE(rm, 10.0);
    }

    void testFindPath_4_Bypass() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=5, max_height=5]; 2 -- 4 [length=5, max_mass=5, max_height=5]; "
                      "1 -- 3 [length=10, max_mass=20, max_height=5]; 3 -- 4 [length=15, max_mass=20, max_height=5]; }";
        QSet<Error> errs;
        Graph g(dot, errs);
        Truck t = {12.0, 4.0};
        QList<int> path;
        double len, rm, rh;

        RouteStatus s = g.findShortestPath(1, 4, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(path, QList<int>({1, 3, 4}));
    }

    // =========================================================================
    // ТАБЛИЦА 3: Тесты конструктора Graph
    // =========================================================================

    void testGraphConstructor_1_Success() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=10, max_height=5]; }";
        QSet<Error> errs;
        Graph g(dot, errs);
        QVERIFY(errs.isEmpty());
        QVERIFY(g.getAdjacencyMap().contains(1));
    }

    void testGraphConstructor_5_MissingParam() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=10]; }";
        QSet<Error> errs;
        Graph g(dot, errs);
        QVERIFY(!errs.isEmpty());
        // Проверка наличия ошибки синтаксиса [cite: 269]
        bool hasSyntaxError = false;
        for(const auto& e : errs) if(e.type == ErrorType::DotSyntaxError) hasSyntaxError = true;
        QVERIFY(hasSyntaxError);
    }

    // =========================================================================
    // ТАБЛИЦА 4: Тесты логики runDijkstra
    // =========================================================================

    void testDijkstra_3_Trap() {
        QString dot = "graph { 1 -- 2 [length=1, max_mass=20, max_height=5]; 2 -- 3 [length=1, max_mass=2, max_height=5]; "
                      "1 -- 4 [length=10, max_mass=20, max_height=5]; 4 -- 3 [length=10, max_mass=20, max_height=5]; }";
        QSet<Error> errs;
        Graph g(dot, errs);
        Truck t = {15.0, 4.0};
        QList<int> path;
        double len, rm, rh;

        RouteStatus s = g.findShortestPath(1, 3, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(path, QList<int>({1, 4, 3}));
    }

    void testDijkstra_6_StartEqualsEnd() {
        QString dot = "graph { 1 -- 2 [length=5, max_mass=20, max_height=5]; }";
        QSet<Error> errs;
        Graph g(dot, errs);
        Truck t = {10.0, 4.0};
        QList<int> path;
        double len, rm, rh;

        RouteStatus s = g.findShortestPath(1, 1, t, path, len, rm, rh);
        QCOMPARE(s, RouteStatus::PathFound);
        QCOMPARE(path, QList<int>({1}));
        QCOMPARE(len, 0.0);
    }
};

QTEST_APPLESS_MAIN(TestGraphPathfinding)
#include "test_main.moc"
