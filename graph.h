#ifndef GRAPH_H
#define GRAPH_H

#include <QMap>
#include <QList>
#include <QStringList>
#include <QPair>
#include "datatypes.h"

/**
 * @file graph.h
 * @author Сапунков А.Р.
 * @brief Класс Graph для представления топологии дорог и поиска маршрутов.
 */
class Graph {
private:
    // Список смежности: узел -> список исходящих ребер
    QMap<QString, QList<Edge>> adjacencyList;

public:
    /**
     * @brief Конструктор, создающий граф на основе DOT-разметки.
     * @param dotData Строка с содержимым DOT-файла.
     * @throw Error При ошибках синтаксиса или форматов данных.
     */
    explicit Graph(const QString& dotData);

    /**
     * @brief Находит оптимальный путь с учетом ограничений грузовика.
     * @param truck Параметры грузовика (масса, высота, точки).
     * @return Пара: статус маршрута и список узлов (путь).
     */
    QPair<RouteStatus, QStringList> findShortestPath(const Truck& truck);

private:
    /**
     * @brief Ядро алгоритма: поиск пути методом Дейкстры.
     * @param start Начальный узел.
     * @param end Конечный узел.
     * @param weightLimit Порог по массе (0 если не учитывается).
     * @param heightLimit Порог по высоте (0 если не учитывается).
     * @return Пара: общая дистанция и список узлов. Если пути нет, дистанция = -1.
     */
    QPair<double, QStringList> runDijkstra(const QString& start, const QString& end,
                                           double weightLimit, double heightLimit);
};

#endif // GRAPH_H
