#pragma once
#include <utility>
#include <vector>
#include <stack>
#include <list>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <chrono> 
#include <random>
#include <functional>
#include <cstdint>
#include <map>
using namespace std;

class Graph {
    //adjList structure is as follows:
    //a vector that contains a list of edges
    //vector[i] represents a node's list of edges
    //an edge is represented as a pair of <other side, weight>
    vector<list<pair<int,int>>> adjList;
    int n; //number of nodes
    int64_t m; //number of edges (uses 64 bits so user can have a fully complete graph with n = max int value)
    bool weighted;
    void fillAdjList();
    void initializeAdjList();

public:
    Graph(bool isWeighted = false, int numNodes = -1, int64_t numEdges = -1);
    void addEdge(int r, int l, int weight = 1);
    void removeEdge(int r, int l);
    bool isEqual(Graph g);
    bool adjListContains(int r, int l);
    vector<tuple<int, int, int>> findCycle();
    Graph findMST();
    double timeToFindMST();
    double timeToFindCycle();
    int getNodeSize() const;
    int64_t getEdgeSize() const;
    bool isWeighted() const;
    list<pair<int, int>> getAdjacencies(int i) const;
};
