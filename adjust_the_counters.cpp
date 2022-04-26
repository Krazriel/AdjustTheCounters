#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

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
    return result;
}

int getDepth(vector<int> node, vector<int> target){
    bool stateFound = false;
    int depth = 0;
    vector< vector<int> > tempNodes;
    vector< vector<int> > currNodes;
    tempNodes.push_back(node);
    while(!stateFound){
        depth++;
        for(int i = 0; i < tempNodes.size(); i++){
            currNodes = expandNode(tempNodes[i]);
            for(int i = 0; i < currNodes.size(); i++){
                if(currNodes[i] == target){
                    return depth;
                }
            }
        }
        tempNodes = currNodes;

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

int aStar(vector<int> node, vector<int> target, int heuristic){
    if(heuristic == 0){
        return getDepth(node, target);
    }
    else{
        return misplacedHeuristic(target) + getDepth(node, target);
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

int main(){
    string userInput;
    int userChoice;
    int nSize;

    cout << "Enter 1: Default, 2: Custom: ";
    cin >> userChoice;

    string defaultState = "7 24 10 19 3 12 20 8 22 23 2 15 25 18 13 11 21 5 9 16 17 4 14 1 6";
    nSize = 5;
    vector<int> initState = setState(defaultState);

    if(userChoice == 2){
        cout << "Enter puzzle size: ";
        cin >> nSize;
        cin.ignore();
        cout << "Enter counters from left to right, top to bottom seperated by space: ";
        getline(cin, userInput);
        initState = setState(userInput);
    }

    vector< vector<int> > nodes = expandNode(initState);

    sort(nodes.begin(), nodes.end());
    nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());

    vector< pair<int, vector<int> > > nodesTable;
    for(int i = 0; i < nodes.size(); i++){
        nodesTable.push_back(make_pair(aStar(initState, nodes[i], 1), nodes[i]));
    }

    sort(nodesTable.begin(), nodesTable.end());

    //Debugger Section
    cout << "Table Test:" << endl;
    for(int i = 0; i < nodesTable.size(); i++){
        for(int j = 0; j < nodesTable[i].second.size(); j++){
            cout << nodesTable[i].second[j] << " ";
        }
        cout << "\ta(n) : " << nodesTable[i].first << endl;
    }

    cout << endl;

    cout << "Expand Test:" << endl;
    for(int i = 0; i < nodes.size(); i++){
        for(int j = 0; j < nodes[0].size(); j++){
            cout << nodes[i][j] << " ";
        }
        cout << " : g(n) = " << getDepth(initState, nodes[i]) 
        << " , h(n) = " << misplacedHeuristic(nodes[i]) 
        << " , a(n) = " << aStar(initState, nodes[i], 1) << endl;
    }

    cout << endl;
    printMatrix(nodes[0], nSize);
    //Debugger Section

    return 0;
}