"""
References:
https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
https://stackoverflow.com/questions/328107/how-can-you-determine-a-point-is-between-two-other-points-on-a-line-segment
"""
import sys, math
import regExHelper as regHelper
from cityStructure import coordinate, line
import cityStructure

class City():
    def __init__(self):
        self.lines = list()
        self.edges = list()
        self.V = list()
        self.E = list()
        self.inters = set()
        self.stToCoords = dict()

    def streetAdd(self, street, coords):
        self.clearAll()
        coordinates = list()
        self.clearAll()
        for coord in coords:
            xy = coord.replace("'", "").replace('(','').replace(')','').split(',')
            coordinates.append(coordinate(int(xy[0]), int(xy[1])))
        self.stToCoords[street] = coordinates
        for idx in range(0, len(coordinates) - 1):
            self.lines.append(line(coordinates[idx], coordinates[idx+1], street)) 

    def cleanStreet(self, streetName):
        self.lines = [l for l in self.lines if(l.street != streetName)]
        for line in self.lines:
            if line.street == streetName:
                self.lines.remove(line)
        self.clearAll()
    
    def clearAll(self):
        self.edges = list()
        self.V = list()
        self.inters = set()

    def isOnSameLine(self, start, intersection, end):
        return (intersection.y <= max(start.y, end.y)) and (intersection.y >= min(start.y, end.y)) and (intersection.x <= max(start.x, end.x)) and (intersection.x >= min(start.x, end.x))

    def orientation(self, p, q, r):
        val = (float(q.y - p.y) * (r.x - q.x)) - (float(q.x - p.x) * (r.y - q.y))

        if(val < 0):
            return Orient.CLOCK
        elif(val > 0):
            return Orient.ANTICLOCK
        else:
            return Orient.COLL

    def doInter(self, l1, l2):
        o1 = self.orientation(l1.start, l1.end, l2.start)
        o2 = self.orientation(l1.start, l1.end, l2.end)
        o3 = self.orientation(l2.start, l2.end, l1.start)
        o4 = self.orientation(l2.start, l2.end, l1.end)

        if((o1 == Orient.COLL) and self.isOnSameLine(l1.start, l2.start, l1.end)):
            return True, l2.start
        elif((o2 == Orient.COLL) and self.isOnSameLine(l1.start, l2.end, l1.end)):
            return True, l2.end
        elif((o3 == Orient.COLL) and self.isOnSameLine(l2.start, l1.start, l2.end)):
            return True, l1.start
        elif((o4 == Orient.COLL) and self.isOnSameLine(l2.start, l1.end, l2.end)):
            return True, l1.end
        else:
            return (o1 != o2) and (o3 != o4), None
    
    def coordinateOnEdge(self, a1, a2, a3):
        cross_product = (a3.y - a1.y) * (a2.x - a1.x) - (a3.x - a1.x) * (a2.y - a1.y)
        if abs(cross_product) > 1e-12:
            return False
        dot_product = (a3.x - a1.x) * (a2.x - a1.x) + (a3.y - a1.y) * (a2.y - a1.y)
        if not 0 <= dot_product <= (a2.x - a1.x) ** 2 + (a2.y - a1.y) ** 2:
            return False
        return True
    
    def streetMod(self, streetName, streetCoordinates):
        self.cleanStreet(streetName)
        self.streetAdd(streetName, streetCoordinates)

    def streetRm(self, streetName):
        self.cleanStreet(streetName)
        self.stToCoords.pop(streetName)

    def getStreets(self): 
        return self.stToCoords.keys()

    def summon_E(self):
        edges_to_add, edges_to_remove = [], []

        for edge in self.edges:
            intersection_point_vs_dist_map = dict()
            intersections_to_check = list()

            for intersection in self.inters:
                if intersection not in [edge.end, edge.start] and self.coordinateOnEdge(edge.start, edge.end, intersection):
                    intersections_to_check.append(intersection)

            if intersections_to_check:
                if len(intersections_to_check) == 1:
                    line_to_add = line(intersections_to_check[0], edge.end)

                    if line_to_add not in edges_to_add:
                        edges_to_add.append(line_to_add)
                    if edge not in edges_to_remove:
                        edges_to_remove.append(edge)
                else:
                    intersections_to_check.sort(key=lambda intersection: math.sqrt((edge.end.x - intersection.x)**2 + (edge.end.y - intersection.y)**2))

                    temp = edge.end
                    for intersection in intersections_to_check:
                        intersection_point_vs_dist_map[intersection] = math.sqrt((edge.end.x - intersection.x)**2 + (edge.end.y - intersection.y)**2)

                    for int_point, dist in intersection_point_vs_dist_map.items():
                        line_to_add = line(temp, int_point)

                        if line_to_add not in edges_to_add:
                            edges_to_add.append(line_to_add)
                        temp = int_point
                        if edge not in edges_to_remove:
                            edges_to_remove.append(edge)

        self.edges.extend(edges_to_add)
        self.edges = [edge for edge in self.edges if edge not in edges_to_remove]
        cityStructure.getMinLines()

    def getIntersections(self, l1, l2):
        try: 
            coord_x, coord_y = None, None
            doIntersect, collinearPoint = self.doInter(l1, l2)
            if (doIntersect):
                coord_x = collinearPoint.x if ((l1.start.x - l1.end.x) * (l2.start.y - l2.end.y)) - ((l1.start.y - l1.end.y) * (l2.start.x - l2.end.x)) == 0 else (((l1.start.x * l1.end.y - l1.start.y * l1.end.x) * (l2.start.x - l2.end.x)) - ((l1.start.x - l1.end.x) * (l2.start.x * l2.end.y - l2.start.y * l2.end.x))) / (((l1.start.x - l1.end.x) * (l2.start.y - l2.end.y)) - ((l1.start.y - l1.end.y) * (l2.start.x - l2.end.x)))
                coord_y = collinearPoint.y if ((l1.start.x - l1.end.x) * (l2.start.y - l2.end.y)) - ((l1.start.y - l1.end.y) * (l2.start.x - l2.end.x)) == 0 else ((l1.start.x * l1.end.y - l1.start.y * l1.end.x) * (l2.start.y - l2.end.y) - (l1.start.y - l1.end.y) * (l2.start.x * l2.end.y - l2.start.y * l2.end.x)) / ((l1.start.x - l1.end.x) * (l2.start.y - l2.end.y) - (l1.start.y - l1.end.y) * (l2.start.x - l2.end.x))
                intCoord = coordinate(coord_x, coord_y)
                self.inters.add(intCoord)
                for lineSegment in [line(l1.start, intCoord), line(l1.end, intCoord), line(l2.start, intCoord), line(l2.end, intCoord)]:
                    if(lineSegment not in self.edges and lineSegment.start != lineSegment.end):
                        self.edges.append(lineSegment)
                for vert in [l1.start, l1.end, l2.start, l2.end, intCoord]:
                    if vert not in self.V:
                        self.V.append(vert)
        except (CityException, Exception):
            return

    def getVerticesAndEdges(self):
        self.clearAll()
        self.summon_V()
        self.summon_E()
        # self.printCity()
        self.printVertAndEdges()

    
    def printVertAndEdges(self):
        n = len(self.edges)
        print("V", len(self.V))
        sys.stdout.flush()
        print('E {', end="", file=sys.stdout)

        self.edges = sorted(self.edges, key=lambda edge: [edge.start.x, edge.start.y, edge.end.x, edge.end.y])

        for index, edge in enumerate(self.edges):
            strToPrint = f'<{self.V.index(edge.start) + 1},{self.V.index(edge.end) + 1}>'

            if index != n - 1:
                strToPrint += ','

            print(strToPrint, end="", file=sys.stdout)

        print('}', file=sys.stdout)
        sys.stdout.flush()

    
    def printCity(self):
        self.V = sorted(self.V, key=lambda vertex: [vertex.x, vertex.y])
        
        print('V = {', file=sys.stdout)
        
        for idx, vertex in enumerate(self.V, start=1):
            cleaned_vertex_str = str(vertex).replace(")", "").replace("(", "").replace(" ", "")
            print(f'  {idx}:  ({cleaned_vertex_str})', file=sys.stdout)
        
        print('}', file=sys.stdout)
        print('E = {', file=sys.stdout)

        self.edges = sorted(self.edges, key=lambda edge:[edge.start.x, edge.start.y, edge.end.x, edge.end.y])
        
        for index, edge in enumerate(self.edges):
            str_to_print = f'  <{self.V.index(edge.start) + 1},{self.V.index(edge.end) + 1}>'
            
            if index != len(self.edges) - 1:
                str_to_print += ','
            
            print(str_to_print, file=sys.stdout)
        
        print('}', file=sys.stdout)

    def summon_V(self):
        for idx1 in range(0, len(self.lines)-1):
            for idx2 in range(idx1+1, len(self.lines)):
                line1, line2 = self.lines[idx1], self.lines[idx2]
                if line1.street != line2.street:
                    self.getIntersections(line1, line2)

def parse(command, city):
    commandInfo = False
    commandInfo = regHelper.match(commandInfo, command)

    if not commandInfo:
        raise CityException("Incorrect input. Input must be in one of the following formats:\n1. add or mod, followed by street name encapsulated in double quotes, followed by coordinates enclosed in round brackets and x and y coordinates separated by a comma\n2. rm followed by street name encapsulated in double quotes\n3. gg")
    else:
        stName = None
        stCoords = None
        action = command.strip().split()[0]
        
        if(action == 'add' or action == 'mod'):
            if(not commandInfo.group(1) or len(commandInfo.group(1)) == 0):
                raise CityException('Street name must be specified with %s command' % action)
            if(not commandInfo.group(2) or len(commandInfo.group(2)) == 0):
                raise CityException('Coordinates must be specified with %s command' % action)
            stCoords = [coord.replace(" ", "") for coord in regHelper.getCoords(commandInfo)]

            if(not len(stCoords) > 1):
                raise CityException('Street must have atleast 2 coordinates.')
            
            stName = commandInfo.group(1).replace('"', '').strip().lower()
            
        elif(action == 'rm'):
            if(not commandInfo.group(1) or len(commandInfo.group(1)) == 0):
                raise CityException('Street name must be specified with %s command' % action)
            
            stName = commandInfo.group(1).replace('"', '').strip().lower()

        return action, stName, stCoords

class CityException(Exception):
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)

def sanityCheck(action, street, city):
    if action == 'add':
        if street in city.getStreets():
            raise CityException('%s operated requested for a street that already exists' % action)
    elif action in ['rm', 'mod'] and street not in city.getStreets():
        if street not in city.getStreets():
            raise CityException('%s operation requested for a street that does not exist' % action)

class Orient():
    COLL = 1
    CLOCK = 2
    ANTICLOCK = 3

def main():
    city = City()
    city.clearAll()
    try:
        while True:
            command = sys.stdin.readline()
            if command in ["", "\n"] :
                break
            try:
                action, street, coords = parse(command, city)
                sanityCheck(action, street, city)
                if action == 'add':
                    city.streetAdd(street, coords)
                elif action == 'rm':
                    city.streetRm(street)
                elif action == 'mod':
                    city.streetMod(street, coords)
                elif action == 'gg':
                    city.getVerticesAndEdges()
                else:
                    raise CityException('Unsupported command')
            except CityException as e:
                print('Error: ' + str(e), file=sys.stderr)
    except KeyboardInterrupt:
        sys.exit(0)
    sys.exit(0)

if __name__ == "__main__":
    main()