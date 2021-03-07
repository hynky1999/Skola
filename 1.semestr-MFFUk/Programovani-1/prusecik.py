from decimal import *
import sys
def gcd(a, b):
    while b > 0:
        a,b = b, a%b
    return a

class Pos:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class Line:
    def __init__(self, start, end):
        if start.x > end.x:
            self.start = end
            self.end = start
        else:
            self.start = start
            self.end = end
        self.a = -(self.end.y - self.start.y)
        self.b = (self.end.x - self.start.x)
        self.c = -(self.start.x*self.a + self.b*self.start.y)

def get_intersection(line1, line2):
    if cmp_float(line1.a*line2.b, line1.b*line2.a):
        return None
    return (line1.b*line2.c - line1.c*line2.b)/(line1.a*line2.b - line1.b*line2.a)

def cmp_float(f1, f2):
    return abs(f1 - f2) <= abs(max(f1,f2) * 100 * sys.float_info.epsilon)

def less_float(f1, f2):
    return f1 < f2 or cmp_float(f1,f2)

def intersect(line1, line2):
    if line2.start.x < line1.start.x:
        line1, line2 = line2, line1
    x = get_intersection(line1, line2)
    # print(line1.c, line1.a, line1.b)
    # print(line2.c, line2.a, line2.b)
    # print(x)
    if(x is None):
        if cmp_float(line1.c, line2.c) and line1.end.x >= line2.start.x:
            print("ANO")
        else:
            print("NE")
        return

    if less_float(max(line1.start.x, line2.start.x), x) and less_float(x, min(line1.end.x, line2.end.x)):
        print("ANO")
    else:
        print("NE")


ax, ay, bx, by, cx, cy, dx, dy = map(int, input().split())
# ax, ay, bx, by, cx, cy, dx, dy = (0, 0, 1, 1, -6, 10, 5,0)
intersect(Line(Pos(ax, ay), Pos(bx, by)), Line(Pos(cx, cy), Pos(dx, dy)))
