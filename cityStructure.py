class line(object):
    def __init__(self, start, end, street=None):
        self.start = start
        self.end = end
        self.street = street
    
    def __eq__(self, other):
        return eqLine(self.start, self.end, other.start, other.end) or eqLine(self.end, self.start, other.start, other.end)

    def __hash__(self):
        return hash((self.start.x, self.start.y, self.end.x, self.end.y))

    def __repr__(self):
        return '[' + str(self.start) + '<>' + str(self.end) + ']'

class coordinate(object):
    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y
    
    def __hash__(self):
        return hash((self.x, self.y))

    def __repr__(self):
        dispX = str(self.x)
        if isinstance(self.x, float):
            dispX = "{0:.2f}".format(self.x) if not self.x.is_integer() else str(int(self.x))
        dispY = str(self.y)
        if isinstance(self.y, float):
            dispY = "{0:.2f}".format(self.y) if not self.y.is_integer() else str(int(self.y))
        return '(' + dispX + ', ' + dispY + ')'
    
def eqLine(oneStart, oneEnd, twoStart, twoEnd):
    return oneStart.x == twoStart.x and oneStart.y == twoStart.y and oneEnd.x == twoEnd.x and oneEnd.y == twoEnd.y

def get_g_c_d(a, b): 
	if (b == 0): 
		return a 
	return get_g_c_d(b, a % b)  

def getLines(points, N, x, y): 
    lines = 0
    d = dict()
    for i in range(N): 
        form = red(points[i][1]  - y, points[i][0] - x)
        if form not in d: 
            d[form] = 1
            lines += 1
    return lines 

def red(yp, xp): 
	g = get_g_c_d(abs(yp), abs(xp)) 
	if (not (yp < 0) ^ (xp < 0) ): 
		return (abs(yp) // g, abs(xp) // g) 
	else: 
		return (-abs(yp) // g, abs(xp) // g)

def getMinLines():
    x = 1
    y = 0
    coords = [
    [-1, 3],
    [4, 3],
    [2, 1],
    [-1, -2],
    [3, -3],
    [5, 0],
    [0, 0],
    [-2, 4],
    [1, -1],
    [6, 2],
    [8, 5],
    [-3, 2],
    [2, -5],
    [7, 1],
    [-4, 0],
    [1, 4],
    [3, 6],
    [-1, 0],
    [5, -4],
    [-3, -1]
]
    a = getLines(coords, len(coords) , x, y)
