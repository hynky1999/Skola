from collections import deque

class Node:
    def __init__(self, edges):
        self.edges = edges
        self.color = 0
    def __repr__(self):
        return f"{self.color} | {self.edges}"

def find_non_colored(verticles , start):
    i = start
    while(i < len(verticles)):
        if(verticles[i].color == 0):
            break
        i += 1
    return i

def color_connected(verticles, start, color):
    stack = deque()
    stack.append(start)
    verticles[start].color = color
    colored = []
    while stack:
        v = stack.pop()
        colored.append(v+1)
        for edge in verticles[v].edges:
            if verticles[edge].color == 0:
                verticles[edge].color = color
                stack.append(edge)
    return colored

def color_verticles(verticles):
    start = 0
    color = 1
    colored = []
    while(start < len(verticles)):
        colored.append(color_connected(verticles, start, color))
        start = find_non_colored(verticles, start)
        color += 1

    return colored

verticles = []
for i in range(int(input())):
    verticles.append(Node([]))

for i in range(int(input())):
    s,e = map(int, input().split())
    verticles[s-1].edges.append(e-1)
    verticles[e-1].edges.append(s-1)

colored = color_verticles(verticles)
for verts in colored:
    print(" ".join(map(str,verts)))
