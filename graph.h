#ifndef GRAPH_H
#define GRAPH_H

#include <QMap>
#include <QList>
#include <QString>
#include <QSet>
#include "datatypes.h"

/**
 * @file graph.h
 * @author Сапунков А.Р.
 * @brief Интерфейс класса Graph для управления дорожной сетью.
 */

/**
 * @brief Класс математической модели графа дорожной сети
 * @details Хранит топологию карты в виде двумерного списка смежности
 * и реализует алгоритмы валидации и поиска кратчайших маршрутов.
 */
class Graph {
private:
    /**
     * @brief Двумерный ассоциативный словарь смежности графа
     * @details Ключ первого уровня — исходный узел, ключ второго уровня — смежный узел.
     * Значение — структура Edge с физическими ограничениями дороги.
     */
    QMap<int, QMap<int, Edge>> adjacencyMap;

    /**
     * @brief Внутренний метод добавления ребра в структуру графа
     * @details Автоматически дублирует связь в обе стороны, обеспечивая неориентированность графа.
     * @param[in] from Идентификатор начального пункта
     * @param[in] to Идентификатор конечного пункта
     * @param[in] len Длина дороги
     * @param[in] mMass Лимит по массе
     * @param[in] mHeight Лимит по высоте
     */
    void addEdge(int from, int to, double len, double mMass, double mHeight);

    /**
     * @brief Низкоуровневая реализация волнового алгоритма Дейкстры
     * @details Выполняет геометрический поиск пути. Может работать как с фильтрацией
     * по габаритам автомобиля, так и в режиме чистого поиска (если truck == nullptr).
     * @param[in] startNode Номер пункта отправления
     * @param[in] endNode Номер пункта назначения
     * @param[out] path Контейнер для записи последовательности узлов маршрута
     * @param[out] totalLen Переменная для записи суммарной протяженности пути
     * @param[out] pathMass Переменная для фиксации минимального лимита массы на пути
     * @param[out] pathHeight Переменная для фиксации минимального лимита высоты на пути
     * @param[in] truck Указатель на параметры автомобиля (опционально)
     * @return true Если путь между точками физически существует
     * @return false Если точки не связаны
     */
    bool runDijkstra(int startNode, int endNode, QList<int>& path, double& totalLen,
                     double& pathMass, double& pathHeight, const Truck* truck = nullptr);

public:
    /**
     * @brief Конструктор графа, совмещенный с синтаксическим парсером DOT
     * @details Читает текст DOT-файла, проверяет его на валидность и заполняет граф.
     * @param[in] dotContent Строка с сырым текстовым содержимым DOT-файла
     * @param[out] errors Множество для аккумулирования найденных синтаксических ошибок
     */
    Graph(const QString& dotContent, QSet<Error>& errors);

    /**
     * @brief Главный метод высокоуровневого расчета оптимального маршрута
     * @details Реализует двухпроходную стратегию: сначала ищет безопасный путь для грузовика.
     * Если проезд заблокирован, ищет любой физический путь, чтобы выявить критические ограничения.
     * @param[in] startNode Номер пункта отправления
     * @param[in] endNode Номер пункта назначения
     * @param[in] truck Габариты транспортного средства
     * @param[out] path Сформированный итоговый список пунктов маршрута
     * @param[out] totalLen Итоговая длина рассчитанного маршрута
     * @param[out] requiredMass Требуемая масса для проезда (заполняется при RouteImpossible)
     * @param[out] requiredHeight Требуемая высота для проезда (заполняется при RouteImpossible)
     * @return RouteStatus Статус результата расчета (PathFound, NoRouteExists или RouteImpossible)
     */
    RouteStatus findShortestPath(int startNode, int endNode, const Truck& truck,
                                 QList<int>& path, double& totalLen,
                                 double& requiredMass, double& requiredHeight);

    /**
     * @brief Геттер для получения копии списка смежности графа
     * @return QMap<int, QMap<int, Edge>> Двумерный словарь структуры дорог
     */
    QMap<int, QMap<int, Edge>> getAdjacencyMap() const { return adjacencyMap; }
};

#endif // GRAPH_H
