from datetime import datetime
import sys
import os

now = datetime.now()

mark = now.strftime('%d%H%M')

#5, 15, 17, 26, 42, 48
test_cases = ['FIVE.txt', 'P01.txt', 'GR17.txt', 'FRI26.txt', 'DANTZIG42.txt', 'ATT48.txt', 'berlin52.txt', 'st70.txt', 'pr76.txt', 'eil101.txt', 'bier127.txt']
solutions = ['19', '291', '2085', '937', '699', '10628', '7542', '675', '108159', '629', '118282']

assert(len(test_cases) == len(solutions))

file_name = f'results_h_{sys.argv[1]}_{mark}.csv'

"""with open(file_name, 'w') as results:
    results.write("Dataset, status, cost, sol_cost, error_p, time" + '\n')
    results.close()"""


for case, result in zip(test_cases, solutions):
    os.system('./main ' + 'Datasets/' + case + " " + result + " | tee -a " + file_name)