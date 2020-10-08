checkboard_size = 8
cell_size = 2

class Player:
    def __init__(self, name):
        self.name = name
        self.active_pieces = []
    def __str__(self):
        return self.name

class Piece:
    def __init__(self, player, y, x, movement, character, y_move):
        self.player = player
        self.movement = movement
        self.character = character
        self.y = y
        self.x = x
        self.y_move = y_move
        self.backwards = False

    def move(self, y, x):
        self.y = y
        self.x = x

    def __str__(self):
        return self.character

class Checkboard:
    def __init__(self, player1, player2):
        self.board = []
        global checkboard_size
        for i in range(checkboard_size):
            self.board += [[]]
            for j in range(checkboard_size):
                self.board[i] += [None]


        for i in range(checkboard_size-3, checkboard_size):
           for j in range((i+1)%2,checkboard_size, 2):
               self.board[i][j] = Piece(player2, i, j, 1, 'oo', -1)
               player1.active_pieces += [self.board[i][j]]

        for i in range(3):
           for j in range((i+1)%2, checkboard_size, 2):
               self.board[i][j] = Piece(player1, i, j, 1, 'xx', 1)
               player2.active_pieces += [self.board[i][j]]

    def __rowString(self):
        string = ""
        for i in range(checkboard_size + 1):
            for j in range(cell_size):
                string += "-"
            string += "+"
        string += '\n'
        return string

    def __str__(self):
        global checkboard_size
        global cell_size
        string = ""
        for i in range(checkboard_size-1, -1, -1):
            string += self.__rowString()
            string += str(i+1) + ' '
            for j in range(checkboard_size):
                string += '|'
                if(self.board[i][j] == None):
                    string += '  '
                else:
                    string += str(self.board[i][j])
            string += '|\n'
        string += self.__rowString()
        string += '  '
        for i in range(checkboard_size):
            string += '|'
            string += chr(ord('A') + i) + ' '
        string += '\n'
        string += self.__rowString()
        return string


    def getBoard(self, i, j):
        return self.board[i][j]

    def getMoveToLocation(self, start_loc, end_loc, player):
        # No error checking
        # Diag move check
        dist_y = end_loc[0] - start_loc[0]
        dist_x = end_loc[1] - start_loc[1]
        if(dist_x == 0 or abs(dist_x) != abs(dist_y)):
            raise ValueError("Not diagonal Move")

        y_direction = int(dist_y/abs(dist_y))
        x_direction = int(dist_x/abs(dist_x))
        piece = self.getBoard(*start_loc)
        if(y_direction != piece.y_move):
            raise ValueError("Cannot move that direction")

        diag_loc = [start_loc[0] + y_direction, start_loc[1] + x_direction]
        for i in range(1, abs(dist_x)):
            if(self.getBoard(*diag_loc)):
                raise ValueError("You have to put a piece right after overleaped piece")
            diag_loc = [start_loc[0] + dist_x, start_loc[1] + dist_y]
        overleaped = self.getBoard(*diag_loc)
        if(not overleaped):
            if(piece.movement > abs(dist_x)):
                raise ValueError()
            return None

        if(piece.movement + 1 > abs(dist_x) or overleaped.player == player):
            raise ValueError()

        return overleaped 


    def checkGameSate(self, player1, player2):
        # TODO add move victory
        if(not player1.active_pieces):
            return player1
        if(not player2.active_pieces):
            return player2


    def pieceJumpable(self, piece): 
        #Coordinates are in y,x order as in matrices 
        y_direction = [piece.y_move]
        if(piece.backwards):
            y_direction += [-piece.y_move]

        for j in y_direction:
            for i in (1, -1):
                diag_loc = [piece.y + j, piece.x + i]
                try:
                    for i in range(piece.movement):
                        if(piece.x == 1 and piece.y == 4):
                            __import__('pdb').set_trace()
                        if(not self.__checkBoundry([diag_loc[0] + j, diag_loc[1] + i])):
                            break
                        other_piece = self.getBoard(*diag_loc)
                        if(
                            other_piece
                            and other_piece.player != piece.player
                            and not self.getBoard(diag_loc[0] + j, diag_loc[1] + i)
                        ):
                            return True
                        diag_loc = [diag_loc[0] + j, diag_loc[1] + i]
                except:
                    continue
        return False

    def findJumpable(self, player_pieces):
        for piece in player_pieces:
            if(self.pieceJumpable(piece)):
                return piece

        return None


    def __checkBoundry(self, location):
        for loc in location:
            if(loc[0] >= checkboard_size or loc < 0):
                return False
        return True

    def move(self, player, move_dict, overleap):
        # if (not self.__checkBoundry(move_dict["start"])):
        #     error_message = "You can't move Location is out of bounds"
        #     return False

        piece = self.getBoard(*move_dict["start"])
        # if(piece is None):
        #     error_message = "There is no piece reatard"
        #     return False
        if(piece.player != player):
            raise ValueError("Not your piece fool")

        # if(not self.__checkBoundry(move_dict["end"])):
        #     error_message = "Move Location is out of bounds"
        #     return False
        overleaped_piece = self.getMoveToLocation(move_dict["start"], move_dict["end"], player)
        if(not overleaped_piece):
            if(self.findJumpable(player.active_pieces)):
                raise ValueError("You better jump you fool")

            self.board[move_dict["end"][0]][move_dict["end"][1]] = piece
            self.board[piece.y][piece.x] = None
            piece.move(move_dict["end"][0],move_dict["end"][1])
            overleap = False
            return piece

        self.board[move_dict["end"][0]][move_dict["end"][1]] = piece
        self.board[piece.y][piece.x] = None
        piece.move(move_dict["end"][0],move_dict["end"][1])
        self.board[overleaped_piece.y][overleaped_piece.x] = None
        player.active_pieces.remove(overleaped_piece)
        overleap = True
        return piece



def formatMove(move):
    move = move.upper().replace(" ", "")
    return {
        "start":[int(move[1]) - 1, ord(move[0]) - ord('A')]
        ,"end":[int(move[3]) - 1, ord(move[2]) - ord('A')]
    }

def processMove(checkboard, player):
    while(True):
        move = input("{} please input your turn\n".format(player))
        try:
            move_dict = formatMove(move)
            overleap = False
            piece = checkboard.move(player, move_dict, overleap)
            if(overleap and checkboard.pieceJumpable(piece)):
                print(checkboard)
                print("You can jump again")
                continue

            return True
        except ValueError as e:
            print(e)
        except ZeroDivisionError:
            print("Invalid Move")


    
    return True


def startGame():
    player1 = Player("Petr")
    player2 = Player("Hynek")
    checkboard = Checkboard(player1, player2)
    turn = 1
    while(True):
        print(checkboard)
        if(turn%2):
            player_on_turn = player1
        else:
            player_on_turn = player2
        processMove(checkboard, player_on_turn)
        winner = checkboard.checkGameSate(player1, player2)
        if(winner):
            print("Vyhral hrac {}".format(winner.name))
            break
        turn += 1

startGame()

