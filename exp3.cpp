#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <limits>
#include <set>
#include <map>
using namespace std;

const int INF = numeric_limits<int>::max();

class Graph {
private:
    vector<vector<int>> adjMatrix;
    vector<vector<int>> adjList;
    map<char, int> vertexMap;
    vector<char> indexToVertex;
    int vertexCount;

public:
    Graph() : vertexCount(0) {}

    void addVertex(char vertex) {
        if (vertexMap.find(vertex) == vertexMap.end()) {
            vertexMap[vertex] = vertexCount;
            indexToVertex.push_back(vertex);
            vertexCount++;
            
            for (auto &row : adjMatrix) {
                row.push_back(0);
            }
            adjMatrix.push_back(vector<int>(vertexCount, 0));
            adjList.resize(vertexCount);
        }
    }

    void addEdge(char from, char to, int weight = 1) {
        addVertex(from);
        addVertex(to);
        
        int fromIndex = vertexMap[from];
        int toIndex = vertexMap[to];
        
        adjMatrix[fromIndex][toIndex] = weight;
        adjMatrix[toIndex][fromIndex] = weight;
        
        adjList[fromIndex].push_back(toIndex);
        adjList[toIndex].push_back(fromIndex);
    }

    void printAdjMatrix() {
        cout << "邻接矩阵:" << endl;
        cout << "  ";
        for (int i = 0; i < vertexCount; i++) {
            cout << indexToVertex[i] << " ";
        }
        cout << endl;
        
        for (int i = 0; i < vertexCount; i++) {
            cout << indexToVertex[i] << " ";
            for (int j = 0; j < vertexCount; j++) {
                if (adjMatrix[i][j] == 0 && i != j) {
                    cout << "0 ";
                } else {
                    cout << adjMatrix[i][j] << " ";
                }
            }
            cout << endl;
        }
    }

    vector<char> BFS(char start) {
        vector<char> result;
        vector<bool> visited(vertexCount, false);
        queue<int> q;
        
        int startIndex = vertexMap[start];
        q.push(startIndex);
        visited[startIndex] = true;
        
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            result.push_back(indexToVertex[current]);
            
            for (int neighbor : adjList[current]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        
        return result;
    }

    void DFSUtil(int v, vector<bool> &visited, vector<char> &result) {
        visited[v] = true;
        result.push_back(indexToVertex[v]);
        
        for (int neighbor : adjList[v]) {
            if (!visited[neighbor]) {
                DFSUtil(neighbor, visited, result);
            }
        }
    }

    vector<char> DFS(char start) {
        vector<char> result;
        vector<bool> visited(vertexCount, false);
        int startIndex = vertexMap[start];
        
        DFSUtil(startIndex, visited, result);
        return result;
    }

    vector<pair<char, int>> shortestPath(char start) {
        int startIndex = vertexMap[start];
        vector<int> dist(vertexCount, INF);
        vector<bool> visited(vertexCount, false);
        
        dist[startIndex] = 0;
        
        for (int i = 0; i < vertexCount; i++) {
            int u = -1;
            for (int j = 0; j < vertexCount; j++) {
                if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
                    u = j;
                }
            }
            
            if (dist[u] == INF) break;
            visited[u] = true;
            
            for (int v = 0; v < vertexCount; v++) {
                if (adjMatrix[u][v] > 0 && !visited[v]) {
                    if (dist[u] + adjMatrix[u][v] < dist[v]) {
                        dist[v] = dist[u] + adjMatrix[u][v];
                    }
                }
            }
        }
        
        vector<pair<char, int>> result;
        for (int i = 0; i < vertexCount; i++) {
            result.push_back({indexToVertex[i], dist[i]});
        }
        return result;
    }

    vector<pair<char, char>> primMST(char start) {
        int startIndex = vertexMap[start];
        vector<int> key(vertexCount, INF);
        vector<int> parent(vertexCount, -1);
        vector<bool> inMST(vertexCount, false);
        
        key[startIndex] = 0;
        
        for (int i = 0; i < vertexCount; i++) {
            int u = -1;
            for (int j = 0; j < vertexCount; j++) {
                if (!inMST[j] && (u == -1 || key[j] < key[u])) {
                    u = j;
                }
            }
            
            if (u == -1) break;
            inMST[u] = true;
            
            for (int v = 0; v < vertexCount; v++) {
                if (adjMatrix[u][v] > 0 && !inMST[v] && adjMatrix[u][v] < key[v]) {
                    key[v] = adjMatrix[u][v];
                    parent[v] = u;
                }
            }
        }
        
        vector<pair<char, char>> result;
        for (int i = 0; i < vertexCount; i++) {
            if (parent[i] != -1) {
                result.push_back({indexToVertex[parent[i]], indexToVertex[i]});
            }
        }
        return result;
    }

    // 双连通分量算法
    void findBCC(int u, vector<int>& disc, vector<int>& low, 
                 vector<int>& parent, vector<bool>& articulation, 
                 stack<pair<int, int>>& st, vector<vector<pair<int, int>>>& bcc, 
                 int& time) {
        disc[u] = low[u] = ++time;
        int children = 0;
        
        for (int v : adjList[u]) {
            if (disc[v] == -1) {
                children++;
                parent[v] = u;
                st.push({u, v});
                
                findBCC(v, disc, low, parent, articulation, st, bcc, time);
                
                low[u] = min(low[u], low[v]);
                
                if ((parent[u] == -1 && children > 1) || 
                    (parent[u] != -1 && low[v] >= disc[u])) {
                    articulation[u] = true;
                    
                    vector<pair<int, int>> component;
                    while (!st.empty() && st.top() != make_pair(u, v)) {
                        component.push_back(st.top());
                        st.pop();
                    }
                    if (!st.empty()) {
                        component.push_back(st.top());
                        st.pop();
                    }
                    bcc.push_back(component);
                }
            } else if (v != parent[u] && disc[v] < disc[u]) {
                low[u] = min(low[u], disc[v]);
                st.push({u, v});
            }
        }
    }

    pair<vector<vector<pair<char, char>>>, set<char>> findBCCAndArticulationPoints() {
        vector<int> disc(vertexCount, -1);
        vector<int> low(vertexCount, -1);
        vector<int> parent(vertexCount, -1);
        vector<bool> articulation(vertexCount, false);
        stack<pair<int, int>> st;
        vector<vector<pair<int, int>>> bcc;
        int time = 0;
        
        for (int i = 0; i < vertexCount; i++) {
            if (disc[i] == -1) {
                findBCC(i, disc, low, parent, articulation, st, bcc, time);
                
                if (!st.empty()) {
                    vector<pair<int, int>> component;
                    while (!st.empty()) {
                        component.push_back(st.top());
                        st.pop();
                    }
                    bcc.push_back(component);
                }
            }
        }
        
        vector<vector<pair<char, char>>> charBCC;
        for (const auto& component : bcc) {
            vector<pair<char, char>> charComponent;
            for (const auto& edge : component) {
                charComponent.push_back({indexToVertex[edge.first], indexToVertex[edge.second]});
            }
            charBCC.push_back(charComponent);
        }
        
        set<char> charAP;
        for (int i = 0; i < vertexCount; i++) {
            if (articulation[i]) {
                charAP.insert(indexToVertex[i]);
            }
        }
        
        return {charBCC, charAP};
    }
};

Graph createGraph1() {
    Graph g;
    g.addEdge('A', 'B', 6);
    g.addEdge('A', 'D', 2);
    g.addEdge('A', 'G', 4);
    g.addEdge('B', 'E', 9);
    g.addEdge('B', 'C', 13);
    g.addEdge('C', 'F', 11);
    g.addEdge('D', 'E', 14);
    g.addEdge('D', 'G', 12);
    g.addEdge('E', 'F', 1);
    g.addEdge('E', 'G', 5);
    g.addEdge('F', 'H', 8);
    g.addEdge('G', 'H', 3);
    return g;
}

Graph createGraph2() {
    Graph g;
    g.addEdge('A', 'B', 4);
    g.addEdge('A', 'C', 13);
    g.addEdge('A', 'D', 11);
    g.addEdge('B', 'C', 12);
    g.addEdge('B', 'E', 1);
    g.addEdge('C', 'D', 5);
    g.addEdge('C', 'E', 8);
    g.addEdge('D', 'E', 14);
    g.addEdge('E', 'F', 2);
    g.addEdge('E', 'G', 9);
    g.addEdge('F', 'G', 3);
    g.addEdge('G', 'H', 7);
    g.addEdge('H', 'I', 6);
    g.addEdge('H', 'J', 10);
    g.addEdge('I', 'J', 15);
    g.addEdge('J', 'K', 16);
    g.addEdge('K', 'L', 17);
    return g;
}

int main() {
    cout << "=== 图1分析 ===" << endl;
    Graph g1 = createGraph1();
    
    // (1) 输出邻接矩阵
    g1.printAdjMatrix();
    cout << endl;
    
    // (2) BFS和DFS
    cout << "从A点出发的BFS遍历: ";
    vector<char> bfs = g1.BFS('A');
    for (char c : bfs) {
        cout << c << " ";
    }
    cout << endl;
    
    cout << "从A点出发的DFS遍历: ";
    vector<char> dfs = g1.DFS('A');
    for (char c : dfs) {
        cout << c << " ";
    }
    cout << endl << endl;
    
    // (3) 最短路径和最小生成树
    cout << "从A点出发的最短路径:" << endl;
    auto shortestPaths = g1.shortestPath('A');
    for (const auto& path : shortestPaths) {
        if (path.second == INF) {
            cout << "到" << path.first << "的最短距离: 不可达" << endl;
        } else {
            cout << "到" << path.first << "的最短距离: " << path.second << endl;
        }
    }
    cout << endl;
    
    cout << "从A点出发的最小生成树边:" << endl;
    auto mst = g1.primMST('A');
    for (const auto& edge : mst) {
        cout << edge.first << "-" << edge.second << " ";
    }
    cout << endl << endl;
    
    // 图1的双连通分量
    cout << "图1的双连通分量和关节点:" << endl;
    auto [bcc1, ap1] = g1.findBCCAndArticulationPoints();
    
    cout << "双连通分量:" << endl;
    for (size_t i = 0; i < bcc1.size(); i++) {
        cout << "分量 " << i + 1 << ": ";
        for (const auto& edge : bcc1[i]) {
            cout << "(" << edge.first << "-" << edge.second << ") ";
        }
        cout << endl;
    }
    
    cout << "关节点: ";
    if (ap1.empty()) {
        cout << "无";
    } else {
        for (char point : ap1) {
            cout << point << " ";
        }
    }
    cout << endl << endl;
    
    cout << "=== 图2分析 ===" << endl;
    Graph g2 = createGraph2();
    
    auto [bcc2, ap2] = g2.findBCCAndArticulationPoints();
    
    cout << "双连通分量:" << endl;
    for (size_t i = 0; i < bcc2.size(); i++) {
        cout << "分量 " << i + 1 << ": ";
        for (const auto& edge : bcc2[i]) {
            cout << "(" << edge.first << "-" << edge.second << ") ";
        }
        cout << endl;
    }
    
    cout << "关节点: ";
    if (ap2.empty()) {
        cout << "无";
    } else {
        for (char point : ap2) {
            cout << point << " ";
        }
    }
    cout << endl;
    
    return 0;
}