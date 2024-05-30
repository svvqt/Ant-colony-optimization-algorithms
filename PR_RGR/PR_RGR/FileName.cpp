#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

// ����, �������������� ��� ������ ���������
class Graph {
public:
    vector<vector<int>> adjList;

    // �������� ����� � ��������� ����������� ������
    Graph(int numVertices) {
        adjList.resize(numVertices);
    }

    // ���������� ����� � ����
    void addEdge(int u, int v) {
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }
};

// �������
class Ant {
public:
    int currentVertex;  // ������� ������� �������
    vector<int> visitedVertices;  // ������ ���������� ������

    // �����������
    Ant(int startVertex) {
        currentVertex = startVertex;
        visitedVertices.push_back(startVertex);
    }

    // ����������� ������� � ��������� �������
    void move(Graph& graph) {
        // ����� ��������� ������� � ����������� �� ������������ ��������
        vector<double> probabilities(graph.adjList[currentVertex].size());
        double sum = 0;

        for (int i = 0; i < probabilities.size(); i++) {
            int nextVertex = graph.adjList[currentVertex][i];

            // ������� ����������� �������� �� ������ ���������� � ���������
            double distance = graph.adjList[currentVertex][i];
            double pheromone = 1;  // � ������ ������� �������� �� ������������, ������� ������ �� ���������� ��������

            probabilities[i] = (1 / distance) * pheromone;
            sum += probabilities[i];
        }

        // ������������ ������������
        for (int i = 0; i < probabilities.size(); i++) {
            probabilities[i] /= sum;
        }

        // ����� ��������� ������� � ������ ������������
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

// �������� ���������� ������� ��� ������ � ������������ ������
vector<int> antColonyOptimization(Graph& graph, int startVertex, int numVertices, int numAnts, int numIterations) {
    // ������������� ������ ���������� ������
    vector<vector<int>> visitedVerticesLists(numAnts);

    // ������ ��������
    for (int i = 0; i < numIterations; i++) {
        // �������� �������� � �� �����������
        vector<Ant> ants;
        for (int j = 0; j < numAnts; j++) {
            ants.push_back(Ant(startVertex));
        }

        for (int j = 0; j < numVertices - 1; j++) {
            for (Ant& ant : ants) {
                ant.move(graph);
            }
        }

        // ���������� ������ ���������� ������ ��� ������� �������
        for (int j = 0; j < numAnts; j++) {
            visitedVerticesLists[j] = ants[j].visitedVertices;
        }
    }

    // ������������ ������ �� ������ ���������� ���������
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

    // ���������� ������ �� �������� ����� ���������
    sort(vertexRankings.begin(), vertexRankings.end(), [](const pair<int, int>& a, const pair<int, int>& b) { return a.second > b.second; });

    // ������� ������ ������������� ������
    vector<int> rankedVertices;
    for (pair<int, int> vertexRanking : vertexRankings) {
        rankedVertices.push_back(vertexRanking.first);
    }

    return rankedVertices;
}

int main() {
    // �������� �����
    Graph graph(6);
    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(0, 4);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(2, 5);
    graph.addEdge(3, 5);

    // ��������� ��� ������ � ������������
    int startVertex = 0;
    int numVertices = 6;
    int numAnts = 10;
    int numIterations = 100;

    // ���������� ������ � ������������
    vector<int> rankedVertices = antColonyOptimization(graph, startVertex, numVertices, numAnts, numIterations);

    // ������ �����������
    system("chcp 1251");
    cout << "������������ ������:\n";
    for (int vertex : rankedVertices) {
        cout << vertex << " ";
    }
    cout << endl;

    return 0;
}