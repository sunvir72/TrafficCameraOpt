#include <iostream>
#include <set>
#include <unordered_set>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <signal.h>
#include "a3FlagsDefVals.h"

using namespace std;

class Coordinate {
public:
    double x, y;

    Coordinate() : Coordinate(0.0, 0.0) {} 
    Coordinate(double x, double y) : x(x), y(y) {}

    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }

    string toString() const {
        if (floor(x) == x && floor(y) == y) {
            return "(" + to_string((int)x) + "," + to_string((int)y) + ")";
        } else {
            return "(" + to_string(x) + "," + to_string(y) + ")";
        }
    }
};

class Line {
public:
    Coordinate start, end;
    string street;

    Line(const Coordinate& start, const Coordinate& end, const string& street = "") : start(start), end(end), street(street) {}

    bool operator==(const Line& newLine) const {
        return (start == newLine.start && end == newLine.end) || (start == newLine.end && end == newLine.start);
    }

    string repr() const {
        return "[" + start.toString() + " --> " + end.toString() + "]";
    }
};

unordered_set<string> streetNames;
vector<vector<Coordinate>> street_coors;
vector<Line> lines_of_street;


int getRandomNumber(int min, int max) {
    if(min == max) return min;
    unsigned int rand;
    ifstream urandom("/dev/urandom", ios::binary);
    if (urandom.fail()) {
        return 1;
    }

    urandom.read(reinterpret_cast<char*>(&rand), sizeof(rand));
    urandom.close();
    return min + (int)(rand % (unsigned int)(max - min + 1));
}

bool coorAlreadyExist(Coordinate coordinate, vector<Coordinate> currStreetCoordinates) {
    auto it = find(currStreetCoordinates.begin(), currStreetCoordinates.end(), coordinate);

    if (it != currStreetCoordinates.end()) {
        return true;
    } 
    return false;
}

bool comparePoints(double point1, double point2, double point3, double point4) {
    return (point1 <= point2 && point3 < point4);
}

bool linesOverlay(const Line& lineOne, const Line& otherLine) {
    double miny1 = min(lineOne.start.y, lineOne.end.y);
    double maxy1 = max(lineOne.start.y, lineOne.end.y);
    double miny2 = min(otherLine.start.y, otherLine.end.y);
    double maxy2 = max(otherLine.start.y, otherLine.end.y);

    if (lineOne.start.x == lineOne.end.x && otherLine.start.x == otherLine.end.x &&
        lineOne.start.x == otherLine.start.x && 
        (
        comparePoints(miny1, miny2, miny2, maxy1) ||
        comparePoints(maxy2, maxy1, miny1, maxy2) ||
        comparePoints(miny2, miny1, miny1, maxy2) ||
        comparePoints(maxy1, maxy2, miny2, maxy1)
        )
        ) {
        return true;
    } 
    
    else if (lineOne.start.x != lineOne.end.x && otherLine.start.x != otherLine.end.x) {
        Coordinate coor1, coor2, coor3, coor4;
        if (lineOne.start.x < lineOne.end.x) {
            coor1 = lineOne.start;
            coor2 = lineOne.end;
        } else {
            coor1 = lineOne.end;
            coor2 = lineOne.start;
        }
        if (otherLine.start.x < otherLine.end.x) {
            coor3 = otherLine.start;
            coor4 = otherLine.end;
        } else {
            coor3 = otherLine.end;
            coor4 = otherLine.start;
        }

        double slopeCoor1And2 = (coor2.y - coor1.y) / (coor2.x - coor1.x);
        double slopeCoor3And4 = (coor4.y - coor3.y) / (coor4.x - coor3.x);
        double interceptCoor1And2 = ((coor2.x * coor1.y) - (coor1.x * coor2.y)) / (coor2.x - coor1.x);
        double interceptCoor3And4 = ((coor4.x * coor3.y) - (coor3.x * coor4.y)) / (coor4.x - coor3.x);

        if (abs(slopeCoor1And2 - slopeCoor3And4) < numeric_limits<double>::epsilon() &&
            abs(interceptCoor1And2 - interceptCoor3And4) < numeric_limits<double>::epsilon() &&
            (
                comparePoints(coor1.x, coor3.x, coor3.x, coor2.x) ||
                comparePoints(coor4.x, coor2.x, coor1.x, coor4.x) ||
                comparePoints(coor3.x, coor1.x, coor1.x, coor4.x) ||
                comparePoints(coor2.x, coor4.x, coor3.x, coor2.x)
             )) {
            return true;
        }
    }
    return false;
}

bool randGenAccepted(Line lineSeg) {
    if(lines_of_street.size() != 0) {
        for(Line l: lines_of_street) {
            if(l == lineSeg || linesOverlay(l, lineSeg)) 
            {return false;}
        }
        return true;
    } else {
        return true;
    }
}

void createInputCoors() {
    int wrongInputs = 0;
    while (static_cast<int>(street_coors.size()) != steetsNum) {
        vector<Coordinate> streetCoordinatesCrrnt;
        vector<Line> currStreetLines;
        lineSegs = getRandomNumber(1, nFlagDefVal);

        while (static_cast<int>(streetCoordinatesCrrnt.size()) != lineSegs + 1) {
            int c_x = getRandomNumber(-cFlagDefVal, cFlagDefVal);
            int c_y = getRandomNumber(-cFlagDefVal, cFlagDefVal);

            Coordinate coordinate = Coordinate(c_x, c_y);
            if (!streetCoordinatesCrrnt.empty()) {
                Line currCoorLine = Line(streetCoordinatesCrrnt.back(), coordinate);
                if (coorAlreadyExist(coordinate, streetCoordinatesCrrnt) || !randGenAccepted(currCoorLine)) {
                    wrongInputs++;
                    if (wrongInputs == 25) {
                        cerr << "Error: Invalid input generayed for 25 consecutive attempts" << endl;
                        kill(getppid(), SIGUSR1);
                        exit(1);
                    }
                } else {
                    wrongInputs = 0;
                    streetCoordinatesCrrnt.emplace_back(coordinate);
                    currStreetLines.emplace_back(currCoorLine);
                }
            } else {
                streetCoordinatesCrrnt.emplace_back(coordinate);
            }
        }

        street_coors.push_back(streetCoordinatesCrrnt);
        lines_of_street.insert(lines_of_street.end(), currStreetLines.begin(), currStreetLines.end());
    }
}


void populate_streets() {

    while (static_cast<int>(streetNames.size()) != steetsNum) {
        string street;
        for (int i = 0; i < 10; ++i) {
            char randomChar = static_cast<char>(rand() % 42 + 48);
            street += randomChar;
        }
        streetNames.insert(street);
    }
}

void handleError() {
    cerr << "Error: Invalid args provided" << endl;
    kill(getppid(), SIGUSR1);
    exit(1);
}

void argsParse(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-s" && i + 1 < argc) {
            sFlagDefVal = atoi(argv[i + 1]);
            if (sFlagDefVal < 2) {
                handleError();
            }
        } 
        else if (string(argv[i]) == "-n" && i + 1 < argc) {
            nFlagDefVal = atoi(argv[i + 1]);
            if (nFlagDefVal < 1) {
                handleError();
            }
        } 
        else if (string(argv[i]) == "-l" && i + 1 < argc) {
            lFlagDefVal = atoi(argv[i + 1]);
            if (lFlagDefVal < 5) {
                handleError();
            }
        } 
        else if (string(argv[i]) == "-c" && i + 1 < argc) {
            cFlagDefVal = atoi(argv[i + 1]);
            if (cFlagDefVal < 1) {
                handleError();
            }
        }
    }
}

void giveInputToA1() {
    auto streetNameIt = streetNames.begin();
    for (const auto& points : street_coors) {
        const string& street = *streetNameIt++;

        cout << "add \"" << street << "\" ";

        for (const Coordinate& point : points) {
            cout << point.toString() << " ";
        }

        cout << endl;
    }
    cout << "gg" << endl;
}

void emptyStuff() {
    streetNames.clear();
    street_coors.clear();
    lines_of_street.clear();
}


void generateInput() {
    populate_streets();
    createInputCoors();
    giveInputToA1();
}


int main(int argc, char** argv) {
    argsParse(argc, argv);
    
    while(true) {
        steetsNum = getRandomNumber(2, sFlagDefVal);
        timeToWaitSec = getRandomNumber(5, lFlagDefVal);

        emptyStuff();
        generateInput();

        sleep(timeToWaitSec);
        for(string street: streetNames) {
            cout<< "rm \"" << street << "\"" << endl;
        }
    }
    return 0;
}