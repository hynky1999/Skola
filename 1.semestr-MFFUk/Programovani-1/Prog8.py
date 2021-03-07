from collections import deque
import math
class Pos:
    def __init__(self, x,y):
        self.x = x
        self.y = y

    def __add__(self, other):
        return Pos(self.x+other.x, self.y+other.y)

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def __hash__(self):
        return hash(self.x, self.y)

    def __repr__(self):
        return f"{self.x} | {self.y}"


class Checkboard:
    def __init__(self, obstacles):
        self.checkboard = []
        for y in range(10):
            self.checkboard.append([])
            for x in range(10):
                if(y == 0 or y == 9 or x == 0 or x == 9):
                    self.checkboard[y].append(-1)
                else:
                    self.checkboard[y].append(0)

        for obstacle in obstacles:
            self.set_tile(obstacle, -1)

    def set_tile(self, pos, value):
        self.checkboard[pos.y][pos.x] = value

    def neighbours(self, pos):
        return [pos + Pos(x,y) for x,y in [(1,0), (1,1), (1,-1), (0,1), (0, -1), (-1, 0), (-1, 1), (-1, -1)]]

    def get_tile(self, pos):
        return self.checkboard[pos.y][pos.x]

    def __str__(self):
        output = ""
        for y in range(len(self.checkboard)):
            output += "\n"
            for x in range(len(self.checkboard[y])):
                output += "{:>10}".format(self.checkboard[y][x])
        return output

    def bf_search(self, start, end):
        queue = deque()
        self.set_tile(start, 1)
        queue.appendleft(start)
        while queue:
            searchx_tile = queue.pop()
            value = self.get_tile(search_tile)
            if search_tile == end:
                return value

            for neigh in self.neighbours(search_tile):
                if(self.get_tile(neigh) == 0):
                    self.set_tile(neigh, value+1)
                    queue.appendleft(neigh)
        return 0


obstacles = []
for x in range(int(input())):
    obstacles.append(Pos(*map(int, input().split())))
ch = Checkboard(obstacles)
start = Pos(*map(int, input().split()))
end = Pos(*map(int, input().split()))
print(ch.bf_search(start, end) -1)


