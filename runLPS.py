from datetime import datetime
import sys
import os

now = datetime.now()

mark = now.strftime('%d%H%M')

#5, 15, 17, 26, 42, 48
test_cases = ['FIVE.txt', 'P01.txt', 'GR17.txt', 'FRI26.txt', 'DANTZIG42.txt', 'ATT48.txt', 'berlin52.txt', 'st70.txt', 'pr76.txt', 'eil101.txt', 'bier127.txt']
#test_cases = ['berlin52.txt', 'st70.txt', 'pr76.txt', 'eil101.txt', 'bier127.txt']

file_name = f'results_{sys.argv[1]}_{mark}'

with open(file_name, 'w') as results:
    results.write("Dataset, status, cost, time" + '\n')
    results.close()


for case in test_cases:
    result = os.system("timeout 10m python3.6 TSP_LP.py Datasets/" + case + " " + file_name)
    if os.WIFEXITED(result):
        if os.WEXITSTATUS(result) == 124:
            with open(file_name, 'a') as result_file:
                print(f'Datasets/{case},TIMEOUT,NA,600000\n')
                result_file.write(f'Datasets/{case},TIMEOUT,NA,600000\n')
                result_file.close()
