#include "graph.h"
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QQueue>
#include <QSet>

Graph::Graph(const QString& dotContent, QSet<Error>& errors) {
    if (!dotContent.trimmed().startsWith("graph")) {
        errors.insert(Error(ErrorType::DotSyntaxError, "Ошибка: файл должен начинаться со слова «graph»."));
    }

    // Ищем строки вида: 1 -- 2 [length=10.5, max_mass=20, max_height=5];
    QRegularExpression edgeRegex("(\\d+)\\s*--\\s*(\\d+)\\s*\\[([^\\]]+)\\]");
    QRegularExpressionMatchIterator it = edgeRegex.globalMatch(dotContent);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        int u = match.captured(1).toInt();
        int v = match.captured(2).toInt();
        QString attrsStr = match.captured(3);

        // Парсинг атрибутов
        double length = -1, maxMass = -1, maxHeight = -1;
        QStringList attrs = attrsStr.split(',');

        bool hasLen = false, hasMass = false, hasHeight = false;

        for (int i = 0; i < attrs.size(); ++i) {
            QString attr = attrs[i].trimmed();
            if (attr.startsWith("length=")) {
                length = attr.mid(7).toDouble(&hasLen);
            } else if (attr.startsWith("max_mass=")) {
                maxMass = attr.mid(9).toDouble(&hasMass);
            } else if (attr.startsWith("max_height=")) {
                maxHeight = attr.mid(11).toDouble(&hasHeight);
            }
        }

        if (!hasLen) errors.insert(Error(ErrorType::DotSyntaxError, QString("Ошибка на участке %1--%2: пропущен обязательный атрибут «length».").arg(u).arg(v)));
        if (!hasMass) errors.insert(Error(ErrorType::DotSyntaxError, QString("Ошибка на участке %1--%2: пропущен обязательный атрибут «max_mass».").arg(u).arg(v)));
        if (!hasHeight) errors.insert(Error(ErrorType::DotSyntaxError, QString("Ошибка на участке %1--%2: пропущен обязательный атрибут «max_height».").arg(u).arg(v)));

        if (hasLen && (length < 1 || length > 100)) errors.insert(Error(ErrorType::OutOfRange, QString("Ошибка на участке %1--%2: длина дороги выходит за рамки лимита (1 - 100).").arg(u).arg(v)));

        // Если город не совпадает сам с собой (не петля) и атрибуты валидны
        if (u != v && hasLen && hasMass && hasHeight && length >= 1 && length <= 100) {
            addEdge(u, v, length, maxMass, maxHeight);
        }
    }
}

void Graph::addEdge(int from, int to, double len, double mMass, double mHeight) {
    adjacencyMap[from][to] = Edge(len, mMass, mHeight);
    adjacencyMap[to][from] = Edge(len, mMass, mHeight); // Неориентированный граф
}

RouteStatus Graph::findShortestPath(int startNode, int endNode, const Truck& truck, QList<int>& path, double& totalLen, double& requiredMass, double& requiredHeight) {
    if (!adjacencyMap.contains(startNode)) return RouteStatus::NoRouteExists;
    if (!adjacencyMap.contains(endNode)) return RouteStatus::NoRouteExists;

    double pMass = 0, pHeight = 0;

    // Ищем путь с учетом грузовика
    if (runDijkstra(startNode, endNode, path, totalLen, pMass, pHeight, &truck)) {
        return RouteStatus::PathFound;
    }

    // Если не нашли, ищем любой путь
    if (runDijkstra(startNode, endNode, path, totalLen, requiredMass, requiredHeight, nullptr)) {
        return RouteStatus::RouteImpossible;
    }

    return RouteStatus::NoRouteExists;
}

bool Graph::runDijkstra(int startNode, int endNode, QList<int>& path, double& totalLen, double& pathMass, double& pathHeight, const Truck* truck) {
    QMap<int, double> distances;
    QMap<int, int> previous;
    QSet<int> visited;

    QList<int> nodes = adjacencyMap.keys();
    for (int i = 0; i < nodes.size(); ++i) {
        distances[nodes[i]] = -1; // -1 означает бесконечность
    }
    distances[startNode] = 0;

    bool targetReached = false;

    while (!targetReached) {
        int u = -1;
        double minDistance = -1;

        // Поиск узла с минимальным расстоянием
        for (int i = 0; i < nodes.size(); ++i) {
            int node = nodes[i];
            if (!visited.contains(node) && distances[node] != -1) {
                if (minDistance == -1 || distances[node] < minDistance) {
                    minDistance = distances[node];
                    u = node;
                }
            }
        }

        if (u == -1 || u == endNode) {
            targetReached = true;
        }

        if (!targetReached) {
            visited.insert(u);
            QMap<int, Edge> neighbors = adjacencyMap.value(u);
            QList<int> adjacentNodes = neighbors.keys();

            for (int i = 0; i < adjacentNodes.size(); ++i) {
                int v = adjacentNodes[i];
                Edge e = neighbors.value(v);

                bool pass = true;
                if (truck != nullptr) {
                    if (truck->mass > e.maxMass || truck->height > e.maxHeight) {
                        pass = false;
                    }
                }

                if (pass) {
                    double newDist = distances[u] + e.length;
                    if (distances[v] == -1 || newDist < distances[v]) {
                        distances[v] = newDist;
                        previous[v] = u;
                    }
                }
            }
        }
    }

    if (!previous.contains(endNode) && startNode != endNode) {
        return false;
    }

    // Восстановление пути
    path.clear();
    int curr = endNode;
    bool pathFinished = false;

    pathMass = -1;
    pathHeight = -1;

    while (!pathFinished) {
        path.prepend(curr);
        if (curr == startNode) {
            pathFinished = true;
        } else {
            int prev = previous[curr];
            Edge e = adjacencyMap[prev][curr];

            if (pathMass == -1 || e.maxMass < pathMass) pathMass = e.maxMass;
            if (pathHeight == -1 || e.maxHeight < pathHeight) pathHeight = e.maxHeight;

            curr = prev;
        }
    }

    totalLen = distances[endNode];
    return true;
}
