import sys
import math
coordinates = []
def euclidean(p1, p2):
    x_coord =  p1[0] - p2[0]
    y_coord = p1[1] - p2[1]
    return math.sqrt(x_coord*x_coord + y_coord*y_coord)
    
with open(sys.argv[1], 'r') as cities:
    for line in cities:
        info = line.split()
        coordinates.append((int(info[1].replace('.0', '')), int(info[2].replace('.0',''))))

    cities.close()

with open(sys.argv[1].replace('.tsp', '.txt'), 'w') as distance_file:
    for pair in coordinates:
        distances = list(map(lambda p: int(euclidean(pair, p) + 0.5), coordinates))
        distance_file.write(" ".join(map(str, distances)) + '\n')
    distance_file.close()
        