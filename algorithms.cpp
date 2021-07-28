#include <fstream>
#include <utility>
#include <cstdint>
#include <unistd.h>
#include <chrono>
#include <queue>
#include <limits> 
#include <algorithm>
#include <float.h>
#include <bits/stdc++.h>
#include <chrono>
#include <set>
#include <csignal>
#include "utils.h"

using namespace std;

chrono::high_resolution_clock::time_point tstart, tend;
vector<int> state, best_solution;
set<int> in_path, outside;
string name;
double g_value, best_bound, initial_bound;
double ** dmatrix;
int  min_not_added, rows, depth;
unsigned long generated, expanded;

void sigalrm_handler(int sig){
    cout << name << "," << "TIMEOUT" << "," << initial_bound << "," << generated << "," << expanded << "," << best_bound << 600000 <<"\n";
}

double ** read_asymetric_matrix(string file_name, int & rows, int & cols){
    ifstream file;
    string line;
    vector<string> distances;
    vector<string>::iterator iter;
    double ** matrix;
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
            matrix = (double**)malloc(sizeof(double*) * rows);
            for (double ** p = matrix; p < matrix + rows; p++){
                *p = (double*)malloc(sizeof(double) * cols);
            }
            continue;
        }

        distances = split(line, " ");
        c_counter = 0;
        for(iter = distances.begin(); iter != distances.end(); iter++){
        //for(int i = 0; i < cols; i++){
            matrix[r_counter][c_counter] = stod(*iter);
            c_counter++;
        }
        r_counter++;
    }

    return matrix;
}

double path_cost(vector<int> path){
    
    double total = 0;
    for(double i = 0; i < path.size() - 1; i++){
        total = total + dmatrix[path[i]][path[i+1]];
    } 

    return total;
}

void addEdgestoQueue(priority_queue<pair<double,pair<int,int>>> & queue, set<int> added, int key){
    pair<int, int> connection;
    set<int>::iterator iter;

    //Nota mental: Construir un conjunto con los vértices que no están en el path
    //Eso podría hacelerar un poco esta función.
    for (iter=outside.begin(); iter != outside.end(); iter++){
        if (added.find(*iter) == added.end()){
            connection = make_pair(key, *iter);
            queue.push(make_pair(-1 * dmatrix[key][*iter], connection));
        }
    }
}

int prim_cost(){
    set<int> added;
    priority_queue<pair<double,pair<int,int>>> queue;
    pair<double,pair<int,int>> current;
    int extreme1, extreme2;
    double cost;
    bool isValid;

    cost = 0;

    added.insert(min_not_added);
    addEdgestoQueue(queue, added, min_not_added);

    for (int i = 0; i < outside.size() - 1; i++){
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
    int key;
    double min1, min2;
    if (min_not_added == -1){
        return 0;
    }
    key = state[0];
    min1 = numeric_limits<double>::infinity();
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

double nearest_neighbour_rand(){
    set<int>::iterator iter;
    double current_min, total;
    int start, current, next_node;
    for(int i = 0; i < rows; i++){
        outside.insert(i);
    }

    srand(time(NULL));
    start = rand() % rows;

    state.push_back(start);
    outside.erase(start);

    current = start;
    total = 0;

    while (state.size() < rows){
        current_min = numeric_limits<double>::infinity();
        next_node = -1;
        for (iter=outside.begin(); iter!=outside.end(); iter++){
            if (dmatrix[current][*iter] < current_min){
                current_min = dmatrix[current][*iter];
                next_node = *iter;
            }
        }

        outside.erase(next_node);
        state.push_back(next_node);
        total += current_min;

    }

    state.push_back(start);
    total += dmatrix[next_node][start];

    return total;

}

double nearest_neighbour(int start){
    set<int>::iterator iter;
    double current_min, total;
    int current, next_node;
    for(int i = 0; i < rows; i++){
        outside.insert(i);
    }

    state.push_back(start);
    outside.erase(start);

    current = start;
    total = 0;

    while (state.size() < rows){
        current_min = numeric_limits<double>::infinity();
        next_node = -1;
        for (iter=outside.begin(); iter!=outside.end(); iter++){
            if (dmatrix[current][*iter] < current_min){
                current_min = dmatrix[current][*iter];
                next_node = *iter;
            }
        }

        outside.erase(next_node);
        state.push_back(next_node);
        total += current_min;

    }

    state.push_back(start);
    total += dmatrix[next_node][start];

    return total;

}

void iterative_nearest_neighbour(int iterations){

    double result;
    best_bound = numeric_limits<double>::infinity();

    for(int i = 0; i < iterations; i++){
        result = nearest_neighbour(i);
        if (result < best_bound){
            best_bound = result;
            best_solution = state;
        }
        state.clear();

    }

}

void iterative_nearest_neighbour_rand(int iterations){

    double result;
    best_bound = numeric_limits<double>::infinity();

    for(int i = 0; i < iterations; i++){
        result = nearest_neighbour_rand();
        if (result < best_bound){
            best_bound = result;
            best_solution = state;
        }
        state.clear();

    }

}

void two_opt_swap(int start, int end){
    int chain_len, loop_end;
    chain_len = end - start + 1;
    loop_end = chain_len / 2;

    for(int i = 0; i < loop_end; i++){
        swap(state[start + i], state[end - i]);
    }
}

void two_opt(){
    double total;
    bool improvement;

    best_bound = numeric_limits<double>::infinity();

    //This should initialize best-bound and best-solution
    iterative_nearest_neighbour(rows);

    do{
        improvement = false;
        for (int i = 1; i < rows; i++){
            for (int j = i + 1; j < rows; j++){
                state = best_solution;
                two_opt_swap(i, j);
                total = path_cost(state);
                if (total < best_bound){
                    best_solution = state;
                    best_bound = total;
                    improvement = true;
                }
            }
        }
    } while(improvement);


}

void depth_first_bnb_visit(){
    set<int> outside_copy = outside;
    set<int>::iterator iter;
    //print_vector_cool(state);
    if (g_value + get_h_value() >= best_bound){
        return;
    }

    expanded += 1;

    if (state.size() == rows + 1){
        best_bound = g_value;
        best_solution = state;
    }

    for(iter=outside_copy.begin(); iter != outside_copy.end(); iter++){
        generated += 1;
        g_value = g_value + dmatrix[state[state.size() - 1]][*iter];
        state.push_back(*iter);
        in_path.insert(*iter);
        outside.erase(*iter);
        min_not_added = (outside.empty() ? -1: *outside.begin());

        //cout << "Call by iter\n";
        depth_first_bnb_visit();

        state.pop_back();
        g_value = g_value - dmatrix[state[state.size() - 1]][*iter];
        in_path.erase(*iter);
        outside.insert(*iter);
        min_not_added = (outside.empty() ? -1: *outside.begin());
    }

    if (state.size() == rows){
        g_value = g_value + dmatrix[state[state.size() - 1]][state[0]];
        state.push_back(state[0]);
        min_not_added = (outside.empty() ? -1: *outside.begin());

        //cout << "Call by closing"<< "\n";

        depth_first_bnb_visit();

        state.pop_back();
        g_value = g_value - dmatrix[state[state.size() - 1]][state[0]];
        min_not_added = (outside.empty() ? -1: *outside.begin());
    }
}

void depth_first_bnb(){
    /*
    //With random permutation
    for(int i = 0; i < rows; i++){
        best_solution.push_back(i);
        outside.insert(i);
    }
    permute_solution(best_solution);
    best_solution.push_back(best_solution[0]);
    best_bound = path_cost(best_solution);
    */

    /*
    //With iterative nearest neieghbour
    iterative_nearest_neighbour(rows);
    state.clear();
    for(int i = 0; i < rows; i++){
        outside.insert(i);
    }
    */ 

    //With two-opt
    two_opt();
    state.clear();
    for(int i = 0; i < rows; i++){
        outside.insert(i);
    }
    initial_bound = best_bound;
    state.push_back(0);
    in_path.insert(0);
    outside.erase(0);
    min_not_added = 1;
    g_value = 0;

    generated += 1;

    depth_first_bnb_visit();

}

double run_herd_karp(unordered_set<int> set, int origin)
{
    if (set.empty()) { return dmatrix[origin][0]; }

    double min_cost = DBL_MAX;
    for (int destination: set)
    {
        unordered_set<int> sub_set = unordered_set<int>(set).erase(destination);

        double c_origin_destination = dmatrix[origin][destination];
        double g = run_herd_karp(sub_set, destination);
        double curren_cost = c_origin_destination + g;

        if (min_cost > curren_cost) { min_cost = curren_cost; }
    }

    return min_cost;
}

void herd_karp() {
    unordered_set<int> set;
    for (int i = 1; i < rows; i++) {
        set.insert(i);
    }
    cout << run_herd_karp(set, 0) << "\n";  // Just for print purposes, see what we can do with it
}

int main(int argc, char** argv){
    string file_name;
    int cols;

    file_name = argv[1];

    name = file_name;

    dmatrix = read_asymetric_matrix(file_name, rows, cols);

    //State space search Methods
    tstart = chrono::high_resolution_clock::now();
    signal(SIGALRM, &sigalrm_handler);  // set a signal handler
    alarm(600);  // set an alarm for 900 seconds from now
    // depth_first_bnb();
    herd_karp();
    tend = chrono::high_resolution_clock::now();
    chrono::milliseconds time_taken = chrono::duration_cast<std::chrono::milliseconds>( tend - tstart );
    cout << file_name << "," << "FIN" << "," << initial_bound << "," << generated << "," << expanded << "," << best_bound << "," << time_taken.count() <<"\n";

    /*TO DO: 
        THAT ALGORITHM
    */
   
    /*
    //APROXIMATE-HEURISTIC METHODS
    tstart = chrono::high_resolution_clock::now();
    //best_bound = nearest_neighbour_rand();
    //iterative_nearest_neighbour_rand(10);
    //iterative_nearest_neighbour(rows);
    two_opt();
    tend = chrono::high_resolution_clock::now();
    chrono::milliseconds time_taken = chrono::duration_cast<std::chrono::milliseconds>( tend - tstart );
    cout << "Tiempo: " << time_taken.count() << "\n";
    cout << "Costo: " << best_bound << "\n";
    */

}