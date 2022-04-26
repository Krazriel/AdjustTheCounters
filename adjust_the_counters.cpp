#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <cmath>

using namespace std;

vector< pair<int, vector<int> > > depthTable;

vector<int> setState(string userInput){
    vector<int> temp;

    string counters;
    stringstream s(userInput);
    while(s >> counters){
        temp.push_back(stoi(counters));
    }
    return temp;
}

vector< vector<int> > expandNode(vector<int> state){
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

    depth += 1;
    for(int i = 0; i < result.size(); i++){
        depthTable.push_back(make_pair(depth, result[i]));
    }

    return result;
}

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

int aStar(vector<int> node, int heuristic){
    if(heuristic == 0){
        return getDepth(node);
    }
    else{
        return misplacedHeuristic(node) + getDepth(node);
    }
}

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

void generalSearch(vector<int> initState){

    //Store repeating nodes
    vector< vector<int> > repeatedNodes;

    //check if initial state is goal state
    if(goalCheck(initState)){
        cout << "Success: Found Solution:" << endl;
        printMatrix(initState, sqrt(initState.size()));
        return;
    }

    //Expand Node and insert to queue
    vector< vector<int> > nodes = expandNode(initState);

    //Start Loop
    while(1){
        //Sort queue by A_Star w/ Misplaced Heuristic (1)

        cout << "Sorting..." << endl;
        vector< pair<int, vector<int> > > nodesTable;
        for(int i = 0; i < nodes.size(); i++){
            nodesTable.push_back(make_pair(aStar(nodes[i], 1), nodes[i]));
        }
        sort(nodesTable.begin(), nodesTable.end());
        cout << "Sorting Complete!" << endl;

        vector< vector<int> > sortedNodes;

        for(int i = 0; i < nodesTable.size(); i++){
            sortedNodes.push_back(nodesTable[i].second);
        }


        // if queue is empty
        if(sortedNodes.size() == 0){
            cout << "Failure: Unable to Find Solution!" << endl;
            return;
        }

        //Pop front of element
        vector<int> node = sortedNodes.front();
        sortedNodes.erase(sortedNodes.begin());
        cout << "g(n) : " << getDepth(node) << " , h(n) : " << misplacedHeuristic(node) << endl;
        printMatrix(node, sqrt(node.size()));
        if(goalCheck(node)){
            cout << "Success: Found Solution:" << endl;
            printMatrix(node, sqrt(node.size()));
            return;
        }

        //Expand and add to queue
        vector< vector<int> > temp = expandNode(node);
        for(int i = 0; i < temp.size(); i++){
            nodes.push_back(temp[i]);
        }
    }

}

int main(){
    string userInput;
    int userChoice;
    int nSize;

    cout << "Enter 1: Default, 2: Custom: ";
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

    //initialize depth table
    depthTable.push_back(make_pair(0, initState));

    //Start search
    cout << "Initial State: " << endl;
    printMatrix(initState, nSize);
    generalSearch(initState);

    return 0;
}