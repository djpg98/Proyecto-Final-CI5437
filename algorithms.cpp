#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <utility>
#include <cstdint>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <queue> 
#include <set>

using namespace std;

//TO DO: BUILD OUTSIDE IN DEPTH_FIRST_BNB

vector<int> state, best_solution;
set<int> in_path, outside;
int ** dmatrix;
int g_value, best_bound, min_not_added, rows;


vector<string> split(string str, string separator){
    vector<string> split_string;
    int start, end;

    start = 0;
    end = str.find(separator);

    while ((end != string::npos) && (end != -1)){
        while (str.substr(start, separator.length()) == separator){
            start = end + 1;
            end = str.find(separator, start);
        }

        if ((end == string::npos) || (end == -1)){
            break;
        }

        split_string.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(separator, start);
    }

    split_string.push_back(str.substr(start, end - start));

    return split_string;
}

int ** read_asymetric_matrix(string file_name, int & rows, int & cols){
    ifstream file;
    string line;
    vector<string> distances;
    vector<string>::iterator iter;
    int ** matrix;
    int r_counter, c_counter;

    file.open(file_name.c_str(), ios_base::in);

    r_counter = 0;
    while(getline(file, line)){
        if (line.length() == 0){
            break;
        }

        if (line.at(0) == 'c'){
            distances = split(line, " ");
            rows = stoi(distances[1]);
            cols = stoi(distances[2]);
            matrix = (int**)malloc(sizeof(int*) * rows);
            for (int ** p = matrix; p < matrix + rows; p++){
                *p = (int*)malloc(sizeof(int) * cols);
            }
            continue;
        }

        distances = split(line, " ");
        c_counter = 0;
        for(iter = distances.begin(); iter != distances.end(); iter++){
        //for(int i = 0; i < cols; i++){
            matrix[r_counter][c_counter] = stoi(*iter);
            c_counter++;
        }
        r_counter++;
    }

    return matrix;
}

int path_cost(vector<int> path){
    
    int total = 0;
    for(int i = 0; i < path.size() - 1; i++){
        total = total + dmatrix[path[i]][path[i+1]];
    } 

    return total;
}

void addEdgestoQueue(priority_queue<pair<int,pair<int,int>>> & queue, set<int> added, int key){
    pair<int, int> connection;

    //Nota mental: Construir un conjunto con los vértices que no están en el path
    //Eso podría hacelerar un poco esta función.
    for (int i = 0; i < rows; i++){
        connection = make_pair(key, i);
        if ((added.find(i) == added.end()) && (in_path.find(i) == in_path.end())){
            queue.push(make_pair(-1 * dmatrix[key][i], connection));
        }
    }
}

//Change first member of queue and current from int to double
int prim_cost(){
    set<int> added;
    priority_queue<pair<int,pair<int,int>>> queue;
    pair<int,pair<int,int>> current;
    int extreme1, extreme2, cost;
    bool isValid;

    cost = 0;

    added.insert(min_not_added);
    addEdgestoQueue(queue, added, min_not_added);

    for (int i = 0; i < rows - 1; i++){
        isValid = false;
        while (!isValid){
            current = queue.top();
            extreme1 = current.second.first;
            extreme2 = current.second.second;
            if (added.find(extreme2) != added.end()){
                queue.pop();
            } else {
                isValid = true;
                queue.pop();
            }
        }
        cost = cost + dmatrix[extreme1][extreme2];
        added.insert(extreme2);
        addEdgestoQueue(queue, added, extreme2);
        
    }

    return cost;
}

int get_h_value(){
    int key, min1, min2;
    if (min_not_added == -1){
        return 0;
    }
    key = state[0];
    min1 = 3000000;
    min2 = min1;
    for(int i = 0; i < rows; i++){
        if (in_path.find(i) == in_path.end()){
            if (dmatrix[key][i] < min1){
                min1 = dmatrix[key][i];
            }
        }
    }

    key = state[state.size() - 1];
    for(int i = 0; i < rows; i++){
        if (in_path.find(i) == in_path.end()){
            if (dmatrix[key][i] < min2){
                min2 = dmatrix[key][i];
            }
        }
    }

    return prim_cost() + min1 + min2;
}

void depth_first_bnb_visit(){
    set<int>::iterator iter;
    if (g_value + get_h_value() >= best_bound){
        return;
    }

    if (state.size() == rows + 1){
        best_bound = g_value;
        best_solution = state; //Pendiente de si esto hace la copia o pasa la referencia
    }

    for(iter=outside.begin(); iter != outside.end(); iter++){
        state.push_back(*iter);
        g_value = g_value + dmatrix[state[state.size() - 1]][*iter];
        in_path.insert(*iter);
        outside.erase(*iter);
        min_not_added = (outside.empty() ? -1: *outside.begin());

        depth_first_bnb_visit();

        g_value = g_value - dmatrix[state[state.size() - 1]][*iter];
        state.pop_back();
        in_path.erase(*iter);
        outside.insert(*iter);
        min_not_added = (outside.empty() ? -1: *outside.begin());
    }
}

void depth_first_bnb(){
    for(int i = 0; i < rows; i++){
        best_solution.push_back(i);
        outside.insert(i);
    }
    best_solution.push_back(0);
    best_bound = path_cost(best_solution);
    state.push_back(0);
    in_path.insert(0);
    outside.erase(0);
    min_not_added = 1;
    g_value = 0;

    depth_first_bnb_visit();


}

int main(int argc, char** argv){
    string file_name;
    int cols;

    file_name = argv[1];

    dmatrix = read_asymetric_matrix(file_name, rows, cols);


    for (int i = 0; i < rows; i++){
        for (int j = 0; j < rows; j++){
            cout << dmatrix[i][j] << " ";
        }

        cout << "\n";
    }
}