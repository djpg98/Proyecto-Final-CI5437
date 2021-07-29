from pulp import *
import sys
import time

matrix = []
variables = []
restrictions_in = []
additional = []
cities = 0
new_restrictions = 0

def dfs_visit(graph, vertex, path, visited):
    global cities
    visited.append(vertex)
    path.append(vertex)
    next_vertex = graph[vertex]

    if (next_vertex in path) and (len(path) < cities):
        tour = path[:] + [next_vertex]
        involved_vars = []

        for key in range(len(tour) - 1):
            involved_vars.append(f'X_{tour[key]}_{tour[key+1]}')
        additional.append(involved_vars)

    elif (next_vertex not in path):
        dfs_visit(graph, next_vertex, path, visited)
    del path[-1]

def dfs(graph):
    global cities
    path = []
    visited = []
    for i in range(cities):
        key = str(i)
        if key not in visited:
            dfs_visit(graph, key, path, visited)


def check_solution(model):
    global new_restrictions
    graph = {}
    additional_before = len(additional)
    for v in model.variables():
        if v.varValue > 0:
            parts = v.name.split("_")
            graph[parts[2]] = parts[3]

    dfs(graph)

    new_restrictions = len(additional) - additional_before
    return (additional_before == len(additional))


def create_variables(number):
    for i in range(number):
        new_row = [f'X_{i}_{j}' for j in range(number) if j !=i]
        variables.append(new_row)
        new_row = [f'X_{j}_{i}' for j in range(number) if j != i]
        restrictions_in.append(new_row)


def create_cost_matrix(file_name):
    global cities
    with open(file_name, 'r') as source:
        limit = 0
        for line in source:
            if line[0] != 'c':
                temp = list(map(float, line.split()))
                matrix.append(temp[0:limit] + temp[limit+1:])
                limit += 1
            else:
                rows = line.split()
                create_variables(int(rows[1]))
                cities = int(rows[1])

###########MAIN############
create_cost_matrix(sys.argv[1])
time1 = time.process_time()
flat_vars = [var for row in variables for var in row]
flat_costs = [cost for row in matrix for cost in row]
cost_d = dict(zip(flat_vars, flat_costs))
modelName = sys.argv[1][:].replace('.txt', '')
model = LpProblem(name=modelName, sense=LpMinimize)
modelVars = LpVariable.dicts("Edges", flat_vars, lowBound=0, cat='Integer')
model += lpSum([cost_d[i] * modelVars[i] for i in flat_vars])
for i in range(len(variables)):
    model += lpSum([modelVars[i] for i in variables[i]]) == 1
for i in range(len(variables)):
    model += lpSum([modelVars[i] for i in restrictions_in[i]]) == 1

model.solve()

"""for v in model.variables():
    if v.varValue > 0:
        print(v.name, "=", v.varValue)"""

while(not check_solution(model)):
    
    for i in range(1, new_restrictions + 1):
        var_set = additional[-1 * i]
        model += lpSum([modelVars[i] for i in var_set]) <= len(var_set) - 1

    model.solve()

time2 = time.process_time()

"""for v in model.variables():
    if v.varValue > 0:
        print(v.name, "=", v.varValue)"""

"""print(f"Value: {value(model.objective)}")"""

with open(sys.argv[2], 'a') as r:
    print(f'{sys.argv[1]},FIN,{value(model.objective)},{time2 - time1}\n')
    r.write(f'{sys.argv[1]},FIN,{value(model.objective)},{time2 - time1}\n')
    r.close()
