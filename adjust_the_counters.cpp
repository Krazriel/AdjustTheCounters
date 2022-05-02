#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

using namespace std;

//Depth Table (Depth value, state)
vector< pair<int, vector<int> > > depthTable;

//Parent Table (Node Vector, Parent Vector)
vector< pair< vector<int>, vector<int> > > parentTable;

//Action Table (Node Vector, action)
vector< pair< vector<int>, string> > actionTable;

//Global Variables 
vector< vector<int> > repeatedNodes;
int nodesExpanded = 0;
int maxQueueSize = 0;
int nodeID = 0;
int parentID = 1;

//print solution steps
void printSolution(vector<int> node, vector<int> initState){
    vector<int> currParent;
    currParent.push_back(-1);


    if(node == initState){
        return;
    }

    //retrieve parent 
    for(int i = 0; i < parentTable.size(); i++){
        if(parentTable[i].first == node){
            currParent = parentTable[i].second;
            break;
        }
    }

    //retrieve action
    for(int i = 0; i < actionTable.size(); i++){
        if(actionTable[i].first == node){
            printSolution(currParent, initState);
            cout << actionTable[i].second << endl;
            break;
        }
    }
}

//Set state from user input
vector<int> setState(string userInput){
    vector<int> temp;

    string counters;
    stringstream s(userInput);
    while(s >> counters){
        temp.push_back(stoi(counters));
    }
    return temp;
}

//set a random state based on user input of number of counters
vector<int> setRandomState(int counters){
    vector<int> temp;
    for(int i = 1; i <= counters; i++){
        temp.push_back(i);
    }
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(temp.begin(), temp.end(), default_random_engine(seed));

    return temp;
}

//expand nodes
vector< vector<int> > expandNode(vector<int> state){
    nodesExpanded++;
    vector< vector<int> > result;
    int depth = 0;

    for(int i = 0; i < state.size(); i++){
        for(int j = 0; j < state.size(); j++){
            vector<int> temp = state;
            if(i == j){
                continue;
            }
            swap(temp[i], temp[j]);
            result.push_back(temp);
            
            //add to action and parent tables
            string c1, c2, action;
            c1 = to_string(temp[i]);
            c2 = to_string(temp[j]);
            action = "Swap " + c1 + ", " + c2;
            actionTable.push_back(make_pair(temp, action));
            parentTable.push_back(make_pair(temp, state));
        }
        
    }
    //remove duplicates
    sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    //retrieve depth of parent
    for(int i = 0; i < depthTable.size(); i++){
        if(depthTable[i].second == state){
            depth = depthTable[i].first;
            break;
        }
    }

    //update depth table
    depth += 1;
    for(int i = 0; i < result.size(); i++){
        depthTable.push_back(make_pair(depth, result[i]));
    }



    return result;
}

//get the depth of the state
int getDepth(vector<int> node){
    int depth = 0;
    for(int i = 0; i < depthTable.size(); i++){
        if(depthTable[i].second == node){
            depth = depthTable[i].first;
            return depth;
        }
    }
    return -1;
}

//print matrix of the state
void printMatrix(vector<int> node, int matrixSize){
    
    int index = 0;
    for(int i = 0; i < matrixSize; i++){
        for(int j = 0; j < matrixSize; j++){
            cout << node[index] << " ";
            index++;
        }
        cout << endl;
    }
    cout << endl;
}

//calculate Manhattan distance for a node
int manhattanHeuristic(vector<int> node){
    vector<int> goalState;
    int score = 0;
    for(int i = 1; i <= node.size(); i++){
        goalState.push_back(i);
    }

    for(int i = 0; i < node.size(); i++){
        for(int j = 0; j < goalState.size(); j++){
            if(node[i] == goalState[j]){
                score += abs(i - j);
            }
        }
    }

    return score;

}

//Calculate Misplaced Tile Heuristic for a node
int misplacedHeuristic(vector<int> node){
    vector<int> goalState;
    int score = 0;
    for(int i = 1; i <= node.size(); i++){
        goalState.push_back(i);
    }

    for(int i = 0; i < node.size(); i++){
        if(node[i] != goalState[i]){
            score++;
        }
    }

    return score;
}

//Calculate A Star for a node based on a heuristic inputted by user
int aStar(vector<int> node, int heuristic){
    if(heuristic == 0){
        return getDepth(node);
    }
    if(heuristic == 1){
        return misplacedHeuristic(node) + getDepth(node);
    }
    else{
        return manhattanHeuristic(node) + getDepth(node);
    }
}

//Check if node is goal
bool goalCheck(vector<int> node){
    vector<int> goalState;
    int score = 0;
    for(int i = 1; i <= node.size(); i++){
        goalState.push_back(i);
    }

    for(int i = 0; i < node.size(); i++){
        if(node[i] != goalState[i]){
            return false;
        }
    }

    return true;
}

//Check if node is a repeat state
bool repeatCheck(vector<int> node){
    for(int i = 0; i < repeatedNodes.size(); i++){
        if(repeatedNodes[i] == node){
            return true;
        }
    }
    return false;
}

//General algorithm inspired from slide
vector<int> generalSearch(vector<int> initState, int heuristic){

    vector< vector<int> > sortedNodes;

    //check if initial state is goal state
    if(goalCheck(initState)){
        cout << "Success! Found Solution:" << endl;
        cout << "Nodes Expanded: " << nodesExpanded << endl;
        cout << "Max Queue Size: " << maxQueueSize << endl;
        printMatrix(initState, sqrt(initState.size()));
        return initState;
    }

    //Expand Node and insert to queue
    vector< vector<int> > nodes = expandNode(initState);
    repeatedNodes.push_back(initState);

    //Start Loop
    while(1){

        //Sort queue by A_Star w/ Uniform Cost Search (0), Misplaced Heuristic (1), Manhattan Heuristic (2)
        vector< pair<int, vector<int> > > nodesTable;
        for(int i = 0; i < nodes.size(); i++){
            nodesTable.push_back(make_pair(aStar(nodes[i], heuristic), nodes[i]));
        }
        sort(nodesTable.begin(), nodesTable.end());

        //grab sorted vectors from table into a queue vector
        for(int i = 0; i < nodesTable.size(); i++){
            sortedNodes.push_back(nodesTable[i].second);
        }

        sort(sortedNodes.begin(), sortedNodes.end());
        sortedNodes.erase(std::unique(sortedNodes.begin(), sortedNodes.end()), sortedNodes.end());

        // if queue is empty
        if(sortedNodes.size() == 0){
            cout << "Failure: Unable to Find Solution!" << endl;
            return initState;
        }

        //update max queue size
        if(sortedNodes.size() >= maxQueueSize){
            maxQueueSize = sortedNodes.size();
        }

        //Pop front of element
        vector<int> node = sortedNodes.front();

        //check if its a repeating node
        while(repeatCheck(node)){
            sortedNodes.erase(sortedNodes.begin());
            node = sortedNodes.front();
        }
        sortedNodes.erase(sortedNodes.begin());

        //add node to repeating nodes
        repeatedNodes.push_back(node);

        //if heuristic is uniform cost search
        if(heuristic == 0){
            cout << "g(n) : " << getDepth(node) << endl;
        }

        //if heuristic is misplaced tile
        if(heuristic == 1){
            cout << "g(n) : " << getDepth(node) << " , h(n) : " << misplacedHeuristic(node) << endl;
        }

        //if heuristic is manhattan distance
        if(heuristic == 2){
            cout << "g(n) : " << getDepth(node) << " , h(n) : " << manhattanHeuristic(node) << endl;
        }

        printMatrix(node, sqrt(node.size()));
        //if goal is found
        if(goalCheck(node)){
            cout << "-------------------------" << endl;
            cout << "Success! Found Solution:" << endl;
            cout << "Nodes Expanded: " << nodesExpanded << endl;
            cout << "Max Queue Size: " << maxQueueSize << endl;
            printMatrix(node, sqrt(node.size()));
            cout << "-------------------------" << endl;
            cout << "Solution Step:" << endl;
            return node;
        }

        //Expand and add to queue
        vector< vector<int> > temp = expandNode(node);
        for(int i = 0; i < temp.size(); i++){
            nodes.push_back(temp[i]);
        }
    }
    return initState;

}

int main(){
    string userInput;
    int userChoice;
    int nSize;
    int nCounters;
    int heuristicChoice;

    cout << "Enter Algorithm: 0 - Uniform Cost Search, 1 - A* w/ Misplaced Tile Heuristic, 2 - A* w/ Manhattan Distance Heuristic: ";
    cin >> heuristicChoice;
    cout << "Enter 1: Default, 2: Custom, 3: Random: ";
    cin >> userChoice;
    cin.ignore();

    string defaultState = "7 24 10 19 3 12 20 8 22 23 2 15 25 18 13 11 21 5 9 16 17 4 14 1 6";
    vector<int> initState = setState(defaultState);
    nSize = sqrt(initState.size());

    if(userChoice == 2){
        cout << "Enter counters from left to right, top to bottom seperated by space: ";
        getline(cin, userInput);
        initState = setState(userInput);
        nSize = sqrt(initState.size());
    }

    if(userChoice == 3){
        cout << "Enter number of counters that are perfect squares: ";
        cin >> nCounters;
        initState = setRandomState(nCounters);
        nSize = sqrt(initState.size());
    }

    //initialize depth table
    depthTable.push_back(make_pair(0, initState));

    //initialize parent table and action table
    parentTable.push_back(make_pair(initState, initState));

    //Start search
    cout << endl << "Solution Trace:" << endl;
    cout << "-------------------------" << endl;
    cout << "Initial State: " << endl;
    printMatrix(initState, nSize);
    printSolution(generalSearch(initState, heuristicChoice), initState);
    cout << endl;


    return 0;
}