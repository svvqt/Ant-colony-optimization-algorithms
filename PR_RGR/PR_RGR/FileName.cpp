#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

// Граф, представленный как список смежности
class Graph {
public:
    vector<vector<int>> adjList;

    // Создание графа с указанным количеством вершин
    Graph(int numVertices) {
        adjList.resize(numVertices);
    }

    // Добавление ребра в граф
    void addEdge(int u, int v) {
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }
};

// Муравей
class Ant {
public:
    int currentVertex;  // Текущая вершина муравья
    vector<int> visitedVertices;  // Список посещенных вершин

    // Конструктор
    Ant(int startVertex) {
        currentVertex = startVertex;
        visitedVertices.push_back(startVertex);
    }

    // Перемещение муравья в следующую вершину
    void move(Graph& graph) {
        // Выбор следующей вершины в зависимости от вероятностей перехода
        vector<double> probabilities(graph.adjList[currentVertex].size());
        double sum = 0;

        for (int i = 0; i < probabilities.size(); i++) {
            int nextVertex = graph.adjList[currentVertex][i];

            // Рассчет вероятности перехода на основе расстояния и феромонов
            double distance = graph.adjList[currentVertex][i];
            double pheromone = 1;  // В данном примере феромоны не используются, поэтому задаем им постоянное значение

            probabilities[i] = (1 / distance) * pheromone;
            sum += probabilities[i];
        }

        // Нормализация вероятностей
        for (int i = 0; i < probabilities.size(); i++) {
            probabilities[i] /= sum;
        }

        // Выбор следующей вершины с учетом вероятностей
        double randNum = (double)rand() / RAND_MAX;
        double cumulativeProbability = 0;

        for (int i = 0; i < probabilities.size(); i++) {
            cumulativeProbability += probabilities[i];

            if (randNum < cumulativeProbability) {
                currentVertex = graph.adjList[currentVertex][i];
                visitedVertices.push_back(currentVertex);
                break;
            }
        }
    }
};

// Алгоритм муравьиной колонии для поиска и ранжирования вершин
vector<int> antColonyOptimization(Graph& graph, int startVertex, int numVertices, int numAnts, int numIterations) {
    // Инициализация списка посещенных вершин
    vector<vector<int>> visitedVerticesLists(numAnts);

    // Запуск муравьев
    for (int i = 0; i < numIterations; i++) {
        // Создание муравьев и их перемещение
        vector<Ant> ants;
        for (int j = 0; j < numAnts; j++) {
            ants.push_back(Ant(startVertex));
        }

        for (int j = 0; j < numVertices - 1; j++) {
            for (Ant& ant : ants) {
                ant.move(graph);
            }
        }

        // Сохранение списка посещенных вершин для каждого муравья
        for (int j = 0; j < numAnts; j++) {
            visitedVerticesLists[j] = ants[j].visitedVertices;
        }
    }

    // Ранжирование вершин на основе количества посещений
    vector<pair<int, int>> vertexRankings;
    for (int i = 0; i < numVertices; i++) {
        int numVisits = 0;

        for (int j = 0; j < numAnts; j++) {
            if (find(visitedVerticesLists[j].begin(), visitedVerticesLists[j].end(), i) != visitedVerticesLists[j].end()) {
                numVisits++;
            }
        }

        vertexRankings.push_back(make_pair(i, numVisits));
    }

    // Сортировка вершин по убыванию числа посещений
    sort(vertexRankings.begin(), vertexRankings.end(), [](const pair<int, int>& a, const pair<int, int>& b) { return a.second > b.second; });

    // Возврат списка ранжированных вершин
    vector<int> rankedVertices;
    for (pair<int, int> vertexRanking : vertexRankings) {
        rankedVertices.push_back(vertexRanking.first);
    }

    return rankedVertices;
}

int main() {
    // Создание графа
    Graph graph(6);
    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(0, 4);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(2, 5);
    graph.addEdge(3, 5);

    // Параметры для поиска и ранжирования
    int startVertex = 0;
    int numVertices = 6;
    int numAnts = 10;
    int numIterations = 100;

    // Выполнение поиска и ранжирования
    vector<int> rankedVertices = antColonyOptimization(graph, startVertex, numVertices, numAnts, numIterations);

    // Печать результатов
    system("chcp 1251");
    cout << "Ранжирование вершин:\n";
    for (int vertex : rankedVertices) {
        cout << vertex << " ";
    }
    cout << endl;

    return 0;
}