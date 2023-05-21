//Olivia Jacques-Baker
//Assignment 1 - COT5405 Analysis of Algorithms

//Problem statement: Given an undirected graph, write an algorithm to find a cycle. You can not assume the graph is connected.
//                   Given a sparse undirected graph (n nodes and at most n+8 edges), write an efficient algroithm to find the minimum spanning tree.

//this file handles program interractions with the user - taking input that specifies how a graph should be set up + what to do with the graph
//the actual functionality for building the graph & running the algos to find cycles and msts are in Graph.cpp

#include <sstream>
#include <iomanip>
#include "Graph.h"
using namespace std;

//function declarations:
void progInteractions();
void reset();
void badResponse();
void handleResponse(int response);
void validWeighted(int response);
void randomAutomatic(int response);
void numNodes(int response, int* n);
void numEdges(int64_t response, int64_t* m, int n);
void autoLevel(int response);
void handleEdgeInput(string response);
void displayGraph(Graph graph);
void getCycle();
void getMST();
void timeAnalysis();
void timeAnalysisSingle();
void runtimeResponse(int response);
bool checkAllNumerical(vector<string> pieces);
vector<string> getSplitString(string str, char delimiter);

Graph G;
bool weighted = false;
bool manual = false;
int autoLvl = -1;
int currInst = 0;
bool running = true;

int main() {
    while (running) {
        cout << "\n======================\n" << endl;
        cout << "Graph Cycles & MSTs - Olivia Jacques-Baker" << endl;
        cout << "This is the main menu. Follow the instructions below to create and interact with a graph." << endl;
        cout << "\n======================\n" << endl;

        progInteractions();
    }
    return 0;
}

//handles all interactions with the user as they create and interact with graphs
void progInteractions() {
    int response = -1; int64_t responseEdges = -1; 
    int64_t m = -1; int n = -1;
    string instructions[6] = {
        "Please choose a main operation:\n1:create a graph\t2: see average runtime analysis\t\t0: quit program",
        "To create a graph, please select an option:\n1: weighted graph\t2: unweighted graph\t0: quit program",
        "Would you like to manually enter the graph's information, or allow for random generation?\n1: manual input\t\t2: automatic\t0: quit program",
        "First, please input the number of nodes you'd like the graph to have:",
        "Please specify the level of randomness you want.\n1:completely random\t2:specified # of nodes, random # of edges\t3:specified # of nodes and edges\t0: quit program",
        "Now, please input the number of edges you'd like the graph to have:"
    };
    //handle responses of the interactions specified above (instructions[])
    while (currInst < 6) {
        cout << instructions[currInst] << endl;
        //instruction 5 (asking how many edges the user wants randomly generated) needs to be handled separately
        //since edge count is stored in a long long rather than an int
        if (currInst == 5) {
            cin >> responseEdges;
            numEdges(responseEdges, &m, n);
        }
        //all others can be handled together after checking that the user isn't asking to quit the program
        else {
            cin >> response;
            if(currInst == 3) numNodes(response, &n);
            else if (response == 0) {
                running = false;
                return;
            }
            else handleResponse(response);
        }
    }
    //if they chose manual over automatic, they must input every edge they want one by one
    if (manual) {
        G = Graph(weighted, n, 0);
        if (n > 1) {
            cout << "You currently have a graph with " << n << " nodes and no edges set up. To set up the edges, input them one at a time. When you're finished adding edges, press 'Q.'" << endl;
            string resp;
            while (manual) {
                if (weighted) cout << "To add an edge, input it in the form 'to, from, weight'. Each value should be a non-negative integer." << endl;
                else cout << "To add an edge, input it in the form 'to, from'. Both values should be non-negative integers." << endl;
                cin >> resp;
                handleEdgeInput(resp);
            }
        }
        else {
            cout << "You currently have a graph with " << n << " nodes and no edges. No edges can be added to this graph; the node size is too small." << endl;
        }

    }
    //otherwise, just create a randomly generated graph
    else if (autoLvl != -1) G = Graph(weighted, n, m);

    //this loop marks where the graph object has been created and the user can now interact with it
    while (true) {
        cout << "What would you like to do with this graph?\n1: find a cycle\t\t2: find an MST\t\t3: view the graph's edges\n4: get runtimes \t5: return to main menu\t0: quit program" << endl;
        cin >> response;
        if (response == 1) getCycle();
        else if (response == 2) getMST();
        else if (response == 3) displayGraph(G);
        else if (response == 4) timeAnalysisSingle();
        else if (response == 5) {
            reset();
            break;
        }
        else if (response == 0) {
            running = false;
            return;
        }
        else badResponse();
    }
}

//reset globals before returning to the main menu
void reset() {
    weighted = false;
    manual = false;
    autoLvl = -1;
    currInst = 0;
}

//used whenever user input is either not a specified option for selecting choices
//more specific error messages used when user is inputing graph info 
//(i.e. a more specific error msg is used instead of this one if user gives negative number for node count)
void badResponse() {
    cout << "Please provide input within the bounds of the instructions." << endl;
}

//routes response to appropriate handler
void handleResponse(int response) {
    if (currInst == 0) runtimeResponse(response);
    else if (currInst == 1) validWeighted(response);
    else if (currInst == 2) randomAutomatic(response);
    else if (currInst == 4) autoLevel(response);
    else badResponse();
}

//acts according to user desire to either run runtime analysis or to go through the steps of creating a single graph
void runtimeResponse(int response) {
    if (response == 1) currInst++;
    else if (response == 2) {
        timeAnalysis();
        reset();
    }
    else badResponse();
}

//set whether the user wants to build a weighted or unweighted graph
void validWeighted(int response) {
    if (response == 1 || response == 2) {
        weighted = (response == 1);
        currInst++;
    }
    else badResponse();
}

//set whether user wants to build a graph manually or automatically
void randomAutomatic(int response) {
    if (response == 1) {
        manual = true;
        cout << "Manual input has been selected. ";
        //go to 'how many nodes do you want' instruction
        currInst = 3;
    }
    else if (response == 2) {
        manual = false;
        cout << "Automatic generation has been selected. ";
        //go to 'what level of automation do you want' instruction
        currInst = 4;
    }
    else badResponse();
}

//if valid node number input, set as such. 
void numNodes(int response, int* n) {
    if (response < 1) cout << "Please provide an integer value greater than zero." << endl;
    else {
        *n = response;
        if (!manual && autoLvl == 3) {
            //if node input is too small to add any edges, alert user and skip that part of the setup
            if (*n == 1) {
                cout << "With only one node, no edges can be added. Skipping edge construction, graph generation complete." << endl;
                //break from current instruction loop
                currInst = 6;
            }
            //go to 'how many edges do you want' instruction
            else currInst = 5;
        }
        //break from current instruction loop
        else currInst = 6;
    }
}

//if valid edge number input, set as such. 
void numEdges(int64_t response, int64_t* m, int n) {
    if (response < static_cast<int64_t>(0)) cout << "Please provide a non-negative integer value." << endl;
    else if (response > (static_cast<int64_t>(n) * (static_cast<int64_t>(n) - 1) / 2)) cout << "Inputted number is too large to be the number of edges for this graph. Please input a possible number of edges given your choice of node count." << endl;
    else {
        *m = response;
        //break from current instruction loop
        currInst = 6;
    }
}

//set what level of automation the user wants
void autoLevel(int response) {
    if (response < 1 || response > 3) badResponse();
    else {
        //go to 'how many nodes do you want' instruction
        if (response == 2 || response == 3) currInst = 3;
        //break from current instruction loop
        else currInst = 6;
        autoLvl = response;
    }
}

//reads in and splits up the user input, validates it, and adds it as a new edge to the graph if possible
void handleEdgeInput(string resp) {
    //input of 'q' indicates user has finished inputing all edges they want
    if (resp == "q" || resp == "Q") {
        cout << "Graph generation complete. Total of " << G.getEdgeSize() << " edges added." << endl;
        manual = false;
        return;
    }

    int l; int r; int weight;
    vector<string> pieces = getSplitString(resp, ',');
    //weighted input is 'i,j,weight' and unweighted is 'i,j'
    int expectedSize = (weighted) ? 3 : 2;
    if (pieces.size() != expectedSize) badResponse();
    else {
        if (!checkAllNumerical(pieces)) badResponse();
        else {
            l = stoi(pieces[0]);
            r = stoi(pieces[1]);
            weight = (weighted) ? stoi(pieces[2]) : 1;
            //check for validity of the actual values of each input 
            int n = G.getNodeSize();
            if (r < 1 || r > n || l < 1 || l > n) {
                cout << "Input nodes are not within the range of this graph - node number must be >=1 and not larger than the number of nodes. Operation stopped." << endl;
                return;
            }
            else if (r == l) {
                cout << "Input nodes may not be the same node. Operation stopped." << endl;
                return;
            }
            else if (G.adjListContains(r, l)) {
                cout << "Input nodes are already connected with a preexisting edge. Operation stopped." << endl;
                return;
            }
            if (weight < 0) {
                cout << "Weight must be greater than zero. Operation stopped." << endl;
                return;
            }
            //now that input type and values have been validated, add the edge.
            G.addEdge(l, r, weight);
            cout << "Edge added. ";
            //whenever an edge is added, we should check if we've reached the maximum # of edges for the graph
            //if so, stop allowing edge input
            int64_t max = (static_cast<int64_t>(G.getNodeSize()) * (static_cast<int64_t>(G.getNodeSize()) - 1)) / 2;
            if (G.getEdgeSize() + 1 > max) {
                cout << "Max number of edges reached." << endl;
                cout << "Graph generation complete. Total of " << G.getEdgeSize() << " edges added." << endl;
                manual = false;
                return;
            }
        }
    }
}

//split string based on given delimiter (used to split up edge info on commas)
vector<string> getSplitString(string str, char delimiter) {
    stringstream ss(str); string piece;
    vector<string> pieces;
    while (ss.good()) {
        getline(ss, piece, delimiter);
        pieces.push_back(piece);
    }
    return pieces;
}

//check if a set of given strings are made entirely of numerical responses
bool checkAllNumerical(vector<string> pieces) {
    for (unsigned int i = 0; i < pieces.size(); i++) {
        if (!all_of(pieces[i].begin(), pieces[i].end(), [](char str) {return isdigit(str); })) return false;
    }
    return true;
}

//for the specified graph, display all of its edges
void displayGraph(Graph graph) {
    if (graph.getEdgeSize() == 0) {
        cout << "There are no edges in this graph." << endl;
        return;
    }
    vector<tuple<int, int, int>> edges;
    list<pair<int, int>> adjL;
    int other; int weight;
    string info = "";
    for (int i = 1; i <= graph.getNodeSize(); i++) {
        adjL = graph.getAdjacencies(i - 1);
        list<pair<int, int>>::iterator iter;
        for (iter = adjL.begin(); iter != adjL.end(); iter++) {
            other = get<0>(*iter);
            weight = get<1>(*iter);
            if (other > i) {
                tuple<int, int, int> newEdge(i, other, weight);
                edges.push_back(newEdge);
            }
        }
    }
    //edges displayed as (node1, node2, weight)
    for (unsigned int i = 0; i < edges.size(); i++) {
        stringstream ss; stringstream ss2; stringstream ss3;
        ss << get<0>(edges[i]);
        ss2 << get<1>(edges[i]);
        ss3 << get<2>(edges[i]);

        info += " (" + ss.str();
        info += ", " + ss2.str();
        if (weighted) info += ", " + ss3.str();
        info += ") ";
    }
    cout << info << endl;
}

//attempt to find a cycle on the user created graph. if one is found, display the edges that make it up
void getCycle() {
    vector<tuple<int, int, int>> cycle = G.findCycle();
    string cycleInfo = "";
    if (cycle.size() > 0) {
        cout << "A cycle has been found! Here are the edges that are within it:" << endl;
        for (unsigned int i = 0; i < cycle.size(); i++) {
            stringstream ss; stringstream ss2; stringstream ss3;
            ss << get<0>(cycle[i]);
            ss2 << get<1>(cycle[i]);
            ss3 << get<2>(cycle[i]);

            cycleInfo += " (" + ss.str();
            cycleInfo += ", " + ss2.str();
            if (weighted) cycleInfo += ", " + ss3.str();
            cycleInfo += ") ";
        }
        cout << cycleInfo << endl;
    }
    else {
        cout << "No cycle was found for this graph." << endl;
    }
}

//attempt to find a MST on the user created graph. if one is found, display the edges that make it up
void getMST() {
    Graph MST = G.findMST();
    if (MST.getEdgeSize() == static_cast<int64_t>(MST.getNodeSize() - 1)) {
        cout << "A MST has been found! Here are the edges that are within it:" << endl;
        displayGraph(MST);
    }
    else cout << "No MST was found for this graph." << endl;
}

//automatic time analysis - used to generate data that can be used to 
//experiementally prove the run time O(m+n) for the greedy algorithms in Graph.cpp
void timeAnalysis() {
    cout << "Timing finding cycles in graphs up to node size 1,000,000 with n-20 edges, please wait..." << endl;
    cout << "======================" << endl;
    double time = 0;
    Graph G;
    int numNodes = 0; bool weighted = false;
    int64_t hold = 0; int64_t max = 0;
    for (int i = 0; i <= 1000000; i += 10000) {
        numNodes = i;
        hold = ((numNodes - 20) < 0) ? 0 : (numNodes - 20);
        cout << "Time to find cycle for graph of node size " << numNodes << " with " << hold << " edges: " << endl;
        for (int j = 0; j < 25; j++) {
            G = Graph(weighted, numNodes, hold);
            time = G.timeToFindCycle();
            cout << fixed;
            cout << setprecision(9);
            cout << time << endl;
        }
        cout << "Complete. User is reccommended to take the median of the above times. Moving on to next node size..." << endl;
    }
    cout << "\nTiming finding MSTs in graphs up to node size 1,000,000 with n+8 edges, please wait..." << endl;
    cout << "======================" << endl;
    weighted = true;
    for (int i = 0; i <= 1000000; i += 10000) {
        numNodes = i;
        max = (((int64_t)numNodes * ((int64_t)numNodes - 1)) / 2);
        hold = (((int64_t)numNodes + 8) > max) ? max : ((int64_t)numNodes + 8);
        cout << "Time to find MST for graph of node size " << numNodes << " with " << hold << " edges: " << endl;
        for (int j = 0; j < 25; j++) {
            G = Graph(weighted, numNodes, hold);
            time = G.timeToFindMST();
            cout << fixed;
            cout << setprecision(9);
            cout << time << endl;
        }
        cout << "Complete. User is reccommended to take the median of the above times. Moving on to next node size..." << endl;
    }
}

//manual (single) time analysis
//allows the user to see how long finding a cycle + MST takes for their manually created graph
void timeAnalysisSingle() {
    double time = G.timeToFindCycle();
    cout << fixed;
    cout << setprecision(9);
    cout << "Time to find cycle for this graph: " << time << " seconds" << endl;
    time = G.timeToFindMST();
    cout << fixed;
    cout << setprecision(9);
    cout << "Time to find MST for this graph: " << time << " seconds" << endl;
}

