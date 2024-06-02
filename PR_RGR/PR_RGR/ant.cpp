#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

using namespace std;

class Graph {
public:
    vector<vector<int>> adjList;
    vector<vector<double>> pheromones;
    vector<vector<double>> distances;

    Graph(int numVertices) {
        adjList.resize(numVertices);
        pheromones.resize(numVertices, vector<double>(numVertices, 1.0));
        distances.resize(numVertices, vector<double>(numVertices, 1.0));
    }

    void addEdge(int u, int v, double distance) {
        adjList[u].push_back(v);
        adjList[v].push_back(u);
        distances[u][v] = distance;
        distances[v][u] = distance;
    }

    void evaporatePheromones(double evaporationRate) {
        for (int i = 0; i < pheromones.size(); ++i) {
            for (int j = 0; j < pheromones[i].size(); ++j) {
                pheromones[i][j] *= (1.0 - evaporationRate);
            }
        }
    }

    void updatePheromones(vector<vector<int>>& visitedVerticesLists, double Q) {
        for (auto& visitedVertices : visitedVerticesLists) {
            double contribution = Q / visitedVertices.size();
            for (int i = 0; i < visitedVertices.size() - 1; ++i) {
                int u = visitedVertices[i];
                int v = visitedVertices[i + 1];
                pheromones[u][v] += contribution;
                pheromones[v][u] += contribution;
            }
        }
    }

    double calculatePathLength(const vector<int>& path) {
        double length = 0.0;
        for (int i = 0; i < path.size() - 1; ++i) {
            length += distances[path[i]][path[i + 1]];
        }
        return length;
    }
};

class Ant {
public:
    int currentVertex;
    vector<int> visitedVertices;
    vector<bool> visited;
    double alpha, beta;

    Ant(int startVertex, int numVertices, double alpha, double beta) : alpha(alpha), beta(beta) {
        currentVertex = startVertex;
        visitedVertices.push_back(startVertex);
        visited.resize(numVertices, false);
        visited[startVertex] = true;
    }

    void move(Graph& graph) {
        vector<int> possibleVertices;
        vector<double> probabilities;
        double sum = 0.0;

        for (int nextVertex : graph.adjList[currentVertex]) {
            if (!visited[nextVertex]) {
                double pheromone = graph.pheromones[currentVertex][nextVertex];
                double heuristic = 1.0 / graph.distances[currentVertex][nextVertex];
                double probability = pow(pheromone, alpha) * pow(heuristic, beta);
                possibleVertices.push_back(nextVertex);
                probabilities.push_back(probability);
                sum += probability;
            }
        }

        if (possibleVertices.empty()) {
            return;
        }

        for (double& probability : probabilities) {
            probability /= sum;
        }

        random_device rd;
        mt19937 gen(rd());
        discrete_distribution<> d(probabilities.begin(), probabilities.end());
        int selectedIndex = d(gen);

        currentVertex = possibleVertices[selectedIndex];
        visitedVertices.push_back(currentVertex);
        visited[currentVertex] = true;
    }
};

pair<vector<int>, double> antColonyOptimization(Graph& graph, int numVertices, int numAnts, int numIterations, double alpha, double beta, double evaporationRate, double Q) {
    double bestPathLength = numeric_limits<double>::infinity();
    vector<int> bestPath;

    for (int i = 0; i < numIterations; i++) {
        vector<Ant> ants;
        for (int j = 0; j < numAnts; j++) {
            int startVertex = rand() % numVertices;
            ants.emplace_back(startVertex, numVertices, alpha, beta);
        }

        for (int j = 0; j < numVertices - 1; j++) {
            for (Ant& ant : ants) {
                ant.move(graph);
            }
        }

        vector<vector<int>> visitedVerticesLists(numAnts);
        for (int j = 0; j < numAnts; j++) {
            visitedVerticesLists[j] = ants[j].visitedVertices;
        }

        graph.evaporatePheromones(evaporationRate);
        graph.updatePheromones(visitedVerticesLists, Q);

        for (int j = 0; j < numAnts; j++) {
            double pathLength = graph.calculatePathLength(visitedVerticesLists[j]);
            if (pathLength < bestPathLength) {
                bestPathLength = pathLength;
                bestPath = visitedVerticesLists[j];
            }
        }

        setlocale(LC_ALL, "RU");
        cout << "Итерация " << i + 1 << ": ";
        for (int vertex : bestPath) {
            cout << vertex << " ";
        }
        cout << " | Длина: " << bestPathLength << endl;
    }

    return make_pair(bestPath, bestPathLength);
}

int main() {
    setlocale(LC_ALL, "RU");
    int numVertices = 16;
    Graph graph(numVertices);

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(1.0, 10.0);

    vector<vector<double>> distances(numVertices, vector<double>(numVertices, 0.0));

    // Заполнение таблицы расстояний и вывод таблицы связи между городами и длины рёбер графа
    cout << "Таблица связи между городами и длина рёбер графа:\n";
    cout << "   ";
    for (int i = 0; i < numVertices; ++i) {
        cout << i << " ";
    }
    cout << endl;
    for (int i = 0; i < numVertices; ++i) {
        cout << i << ": ";
        for (int j = 0; j < numVertices; ++j) {
            double distance = dis(gen);
            distances[i][j] = distance;
            distances[j][i] = distance;
            cout << distance << " ";
        }
        cout << endl;
    }
    cout << endl;

    // Добавление рёбер в граф на основе расстояний
    for (int i = 0; i < numVertices; ++i) {
        for (int j = i + 1; j < numVertices; ++j) {
            graph.addEdge(i, j, distances[i][j]);
        }
    }

    for (int i = 0; i < numVertices; ++i) {
        for (int j = i + 1; j < numVertices; ++j) {
            double distance = dis(gen);
            distances[i][j] = distance;
            distances[j][i] = distance;
        }
    }

    for (int i = 0; i < numVertices; ++i) {
        for (int j = i + 1; j < numVertices; ++j) {
            graph.addEdge(i, j, distances[i][j]);
        }
    }

    int numAnts = 10;
    int numIterations = 100;
    double alpha = 1.0;
    double beta = 2.0;
    double evaporationRate = 0.1;
    double Q = 10.0;

    auto result = antColonyOptimization(graph, numVertices, numAnts, numIterations, alpha, beta, evaporationRate, Q);

    vector<int> bestPath = result.first;
    double bestPathLength = result.second;
    system("chcp 1251");
    cout << "\nОптимальный путь:\n";
    for (int vertex : bestPath) {
        cout << vertex << " ";
    }
    cout << "\nДлина оптимального пути: " << bestPathLength << endl;

    return 0;
}
