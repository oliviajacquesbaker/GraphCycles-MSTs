#include "Graph.h"

//this file handles all graph functionality - creating a graph, inspecting it, and performing the operations of finding cycles and MSTs

//constructor has default params in case the user does not wish to build a specific graph + instead wishes for the system to handle it
//if either or both of the params for # of nodes / # of edges is left out, it'll get set to something random (within reasonable range)
//if weighted specification is left out, defaults to unweighted graph.
Graph::Graph(bool isWeighted, int numNodes, int64_t numEdges) {
    unsigned int seed = static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count());
    default_random_engine generator(seed);
    //if less than zero, was left unspecified / as default param. 
    //in that case, constructor being used to generate a random graph. to keep the random graph interesting, 3 nodes min.
    if (numNodes < 0) {
        uniform_int_distribution<int> distributionNodes(3, 100);
        numNodes = distributionNodes(generator);
    }
    //same as above, only < 0 when being used to generate graphs of varying levels of randomness.
    int64_t max = ((static_cast<int64_t>(numNodes) * (static_cast<int64_t>(numNodes - 1))) / 2);
    if (numEdges < 0) {
        uniform_int_distribution<int64_t> distributionEdges(0, max);
        numEdges = distributionEdges(generator);
    }
    else if (numEdges > max) numEdges = max;
    n = numNodes;
    m = numEdges;
    weighted = isWeighted;
    if (m > 0) fillAdjList();
    else initializeAdjList();
}

//weight has default param of 1 for unweighted graphs
void Graph::initializeAdjList() {
    for (int i = 1; i <= n; i++) {
        list<pair<int, int>> newList;
        adjList.push_back(newList);
    }
}

//weight has default param of 1 for unweighted graphs
void Graph::addEdge(int r, int l, int weight) {
    if (r >= 1 && r <= n && l >= 1 && l <= n && r != l) {
        if (adjListContains(r,l)) return;
        adjList[r - 1].push_back(make_pair(l, weight));
        adjList[l - 1].push_back(make_pair(r, weight));
        m++;
    }
}

//fill the adjacency list with randomly placed edges
void Graph::fillAdjList() {
    initializeAdjList();
    int r = 0; int l = 0; int mCurr = 0; int weight = 1;
    unsigned int seed = static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count());
    default_random_engine generator(seed);
    uniform_int_distribution<int> distributionNode(1, n);
    uniform_int_distribution<int> distributionWeight(1, 100);
    auto randNode = bind(distributionNode, generator);
    auto randWeight = bind(distributionWeight, generator);
    while (mCurr < m) {
        while (true) {
            r = randNode();
            l = randNode();
            if (!adjListContains(r, l) && r != l) break;
        }
        weight = (weighted) ? randWeight() : 1;
        adjList[r-1].push_back(make_pair(l, weight));
        adjList[l-1].push_back(make_pair(r, weight));
        mCurr++;
    }
}

//check if an edge exists within the graph
bool Graph::adjListContains(int r, int l) {
    //return a bool indicating if the adList contains a specified edge
    list<pair<int, int>> currList = adjList[r-1];
    list<pair<int, int>>::iterator iter = currList.begin();

    for (; iter != currList.end(); iter++) {
        if (get<0>(*iter) == l) return true;
    }
    return false;
}

//check if two graphs are equivilant (same nodes + same adjacency lists)
//this function was developed for the unit tests built alongside this project
//(not included in my portfolio, as they were built using the visual studio framework and 
//are difficult to get to run on someone else's machine w/o the whole .sln file)
//but i promise this function WAS built for a purpose even if you can't see it :)
bool Graph::isEqual(Graph g) {
    if (n != g.getNodeSize()) return false;
    if (m != g.getEdgeSize()) return false;
    list<pair<int, int>>::iterator iter;
    list<pair<int, int>>::iterator iter_self;
    list<pair<int, int>> currList;
    if (m > 0) {
        for (int i = 0; i < n; i++) {
            currList = g.getAdjacencies(i);
            iter_self = adjList[i].begin();
            for (iter = currList.begin(); iter != currList.end(); iter++) {
                if (get<0>(*iter) != get<0>(*iter_self)) return false;
                if (get<1>(*iter) != get<1>(*iter_self)) return false;
                iter_self++;
            }
        }
    }
    return true;
}

//find a cycle in the graph G
vector<tuple<int, int, int>> Graph::findCycle() {
    vector<tuple<int, int, int>> cycle;
    if (m < 3) return cycle; //uninterested in cycles unless they're of size 3 or greater

    vector<bool> discovered(n + 1, false);
    vector<bool> included(n+1, false);
    vector<pair<int, int>> parent(n + 1);
    int s; int currNode; int prevNode;
    bool cycleFound = false;
    vector<int> storeWeight(n+1, -1);
    stack<int> DFSstack;
    //since we can't assume the graph is connected, must run the algo starting from a 
    //new undiscovered node until a cycle is found or all nodes are marked as discovered
    while (!cycleFound) {
        s = -1;
        for (unsigned int i = 1; i < discovered.size(); i++) {
            if (!discovered[i]) {
                s = i; 
                break;
            }
        }
        if (s == -1) return cycle;
        //run a DFS to search for a cycle 
        DFSstack.push(s);
        while (!cycleFound && DFSstack.size() > 0) {
            currNode = DFSstack.top();
            DFSstack.pop();
            if (!discovered[currNode]) {
                discovered[currNode] = true;
                list<pair<int, int>> listCur = adjList[currNode - 1];
                for (list<pair<int, int>>::iterator iter = listCur.begin(); iter != listCur.end(); iter++) {
                    //when adding a node to the stack, also record its 'parent' - the node that put it on the stack
                    //this will let us backtrack to find all edges of the cycle later
                    if (get<0>(parent[currNode]) != get<0>(*iter)) {
                        DFSstack.push(get<0>(*iter));
                        prevNode = currNode;
                        if (!discovered[get<0>(*iter)]) parent[get<0>(*iter)] = make_pair(currNode, get<1>(*iter));
                        else storeWeight[get<0>(*iter)] = get<1>(*iter);
                    }
                }
            }
            //discovering a node that was already marked as discovered earlier -> we've looped back around -> cycle found!
            else {
                cycleFound = true;
                parent[currNode] = make_pair(prevNode, storeWeight[currNode]);
            }
        }
    }
    
    //use recorded parent relations to climb back through the cycle and record all edges within it
    if (cycleFound) {
        while (!included[currNode]) {
            cycle.push_back(make_tuple(currNode, get<0>(parent[currNode]), get<1>(parent[currNode])));
            included[currNode] = true;
            currNode = get<0>(parent[currNode]);
        }
    }
    //return the set of edges that make up a cycle
    return cycle;
}

//find a minimim spanning tree of the graph G
//this method was developed specifically for the problem specifications of 'sparse' graphs
//it is a poor algorithm for dense graphs
Graph Graph::findMST() {
    //not possible unless there are at least n-1 edges (the amount of edges necessary to form a tree)
    int64_t x = 0;
    if (m < ((int64_t)n - 1)) {
        return Graph(isWeighted(), 0, x);
    }
    vector<tuple<int, int, int>> cycle;
    Graph MST = *this;
    int expCost = -1;
    tuple<int, int, int> expEdge;
    //while we have more than n-1 edges, find a cycle in G
    while (!(MST.getEdgeSize() < ((int64_t)n - 1))) { //O(9) on problem-defined sparse graphs
        cycle = MST.findCycle(); //O(n+m)
        //if a cycle was found, run through its edges to determine the most expensive edge within it. then, remove that edge
        if (cycle.size() > 0) {
            expCost = -1;
            for (unsigned int i = 0; i < cycle.size(); i++) {
                if (expCost < get<2>(cycle[i])) {
                    expCost = get<2>(cycle[i]);
                    expEdge = cycle[i];
                }
            }
            MST.removeEdge(get<0>(expEdge), get<1>(expEdge)); //O(2m)
        }
        else break;
    }
    //return the graph representing the MST
    if(MST.getEdgeSize() == ((int64_t)n - 1)) return MST;
    else return Graph(isWeighted(), 0, x);
}

//remove a specified edge - used to remove the most expensive edge from a cycle
void Graph::removeEdge(int r, int l) {
    list<pair<int, int>>::iterator iter_r = adjList[r - 1].begin();
    list<pair<int, int>>::iterator iter_l = adjList[l - 1].begin();
    for (; iter_r != adjList[r - 1].end(); iter_r++) {
        if (get<0>(*iter_r) == l) {
            adjList[r - 1].erase(iter_r);
            break;
        }
    }
    for (; iter_l != adjList[l - 1].end(); iter_l++) {
        if (get<0>(*iter_l) == r) {
            adjList[l - 1].erase(iter_l);
            break;
        }
    }
    m--;
}

//use stopwatch to measure the time it takes to run the find cycle algo
double Graph::timeToFindCycle() {
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    findCycle();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    chrono::duration<double> timeSpent = chrono::duration_cast<chrono::duration<double>>(end - start);
    return timeSpent.count();
}

//use stopwatch to measure the time it takes to run the find mst algo
double Graph::timeToFindMST() {
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    findMST();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    chrono::duration<double> timeSpent = chrono::duration_cast<chrono::duration<double>>(end - start);
    return timeSpent.count();
}

int Graph::getNodeSize() const {
    return n;
}

int64_t Graph::getEdgeSize() const {
    return m;
}

bool Graph::isWeighted() const {
    return weighted;
}

list<pair<int, int>> Graph::getAdjacencies(int i) const {
    return adjList[i];
}