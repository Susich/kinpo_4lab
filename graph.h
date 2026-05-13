#ifndef GRAPH_H
#define GRAPH_H

#include <QMap>
#include <QList>
#include <QSet>
#include <QString>
#include "datatypes.h"

/**
 * @file graph.h
 * @author Сапунков А.Р.
 * @brief Класс Graph согласно Внутренней спецификации.
 */
class Graph {
private:
    QMap<int, QMap<int, Edge>> adjacencyMap; // Двумерный словарь (список смежности)

    void addEdge(int from, int to, double len, double mMass, double mHeight);
    bool runDijkstra(int startNode, int endNode, QList<int>& path, double& totalLen, double& pathMass, double& pathHeight, const Truck* truck = nullptr);

public:
    Graph(const QString& dotContent, QSet<Error>& errors);
    RouteStatus findShortestPath(int startNode, int endNode, const Truck& truck, QList<int>& path, double& totalLen, double& requiredMass, double& requiredHeight);

    QMap<int, QMap<int, Edge>> getAdjacencyMap() const { return adjacencyMap; }
};

#endif // GRAPH_H
