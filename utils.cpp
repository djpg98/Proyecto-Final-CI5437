#include "utils.h"

vector<string> split(string str, string separator){
    vector<string> split_string;
    string last_one;
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

    last_one = str.substr(start, end - start);
    if (last_one != separator && !last_one.empty()){
        split_string.push_back(last_one);
    }

    return split_string;
}

void print_vector_cool(vector<int> vect){
    vector<int>::iterator iter;
    for(iter=vect.begin();iter!=vect.end();iter++){
        cout << *iter << " ";
    }
    cout << "\n";
}

void permute_solution(vector<int> & vect){
    srand(time(NULL));
    int index;
    for(int i = 0; i < vect.size(); i++){
        index = rand() % (vect.size() - i)  + i;
        swap(vect[i], vect[index]);
    }

}