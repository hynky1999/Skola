checkboard_size = 8

class Piece:
    def __init__(self, player):
        self.player = player
        self.jump_list = []
        self.threat_list = []
        self.movement = 1
        self.__y_move = [1]
        self.__backwards = false
        
class Checkboard:
    def __init__(self, player1, player2):
        self.board = []
        global checkboard_size
        for i in range(checkboard_size):
            board.append([])
            for j in range(checkboard_size):
                board += [Piece(None)]
        
        for i in range(2):
           for j in range(checkboard_size):
               board[i][j] = Piece(player1)

        for i in range(2):
           for j in range(checkboard_size):
               board[i][j] = Piece(player2)
               
    def getBoard(self, i, j):
        return self.board[i][j]

    def canJumpFromToLoc(self, start_loc, end_loc):
        # No error checking
        # Diag move check
        dist_y = abs(end_loc[0] - start_loc[0])
        dist_x = abs(end_loc[1] - start_loc[1])
        if(dist_x == 0 or abs(dist_x) != abs(dist_y)):
            return false

        y_direction = int(dist_y/abs(dist_y))
        x_direction = int(dist_x/abs(dist_x))
        piece = getboard(*start_loc)
        other_piece = getboard(*end_loc)
        if(piece.movement < dist or y_direction not in piece.movement or piece.player == other_piece.player):
            return false

        if(getboard(end_loc[0] + y_direction, end_loc[1] + x_direcion)):
            return true
        return false

    def canMoveFromToLoc(self, start_loc, end_loc):
        # No error checking
        # Diag move check
        dist_y = abs(end_loc[0] - start_loc[0])
        dist_x = abs(end_loc[1] - start_loc[1])
        if(dist_x == 0 || dist_x != dist_y):
            return false
        y_direction = int((dist_y/abs(dist_y))
        x_direction = int(dist_x/abs(dist_x))
        if(piece.movement < dist_x || y_direction not in piece.movement):
            return false
        diag_loc = [start_loc[0] + dist_y, start_loc[1] + dist_x]
        for(i in range(dist)):
            if(board[*diag_loc]):
                return false      
            diag_loc = [start_loc[0] + dist_x, start_loc[1] + dist_y]

        return true

    def __getItem__(self, key):
       return __checkBoundry(*key) ? board[key[0]][key[1]] : None

    def lumpListUpdateEnter(self, piece, location): 
        #Coordinates are in y,x order as in matrices 
        for(j in (1, -1)):
            for(i in (1, -1)):
                diag_loc = [location[0] + j, location[1] + i]
                while(__checkBoundry(*diag_loc)):
                    other_piece = board[diag_loc]
                    if(other_piece):
                        if(canJumpFromToLoc(location, diag_loc)):
                            other_piece.threat_dict[(-j, -i)] = piece
                            piece.jump_dict[(j, i)] += other_piece                        
                        if(canJumpFromToLoc(diag_loc, location)):
                            piece.threat_dict[(-j, -i)] = piece
                            other_piece.jump_dict[(j, i)] = other_piece 
                        break
                    
                    diag_loc = [diag_loc[0] + j, diag_loc[1] + i]
        
    def __checkBoundry(location)
        for(loc in location):
            if(loc > checkboard_size || loc < 0):
                return false
        return true
               
    def move(self, player, move_dict, error_message):
        if (not self.__checkBoundry(move_dict["start"])):
            error_message = "You can't move Location is out of bounds"
            return false

        piece = board[*move_dict["start"]]
        if(piece is None):
            error_message = "There is no piece reatard"
            return false

        if(piece.player != player):
            error_message = "Not your piece fool"

        if(not self.__checkBoundry(move_dict["end"])):
            error_message = "Move Location is out of bounds"
            return false
            
        if(canMoveFromToLoc())
            
        forced_jump_pieces = [
            jump_piece for jump_piece in player.active_pieces if
            [jump_to_piece for jump_to_piece in piece.jump_dict.values()] == [None, None, None, None]
        ]
        if(forced_jump_pieces and (piece not in forced_jumps || move_dict["end"] not in forced_jumps[piece])):
            error_message = "Force jump is required please jump"
            return false
        
        board[*move_dict["end"]] = piece
        del(board(*move_dict["start"]))
        return true

            
class Box:
    def __init__(self, location):
        self.piece = None
        self.location = location
    def addPiece(self, piece):
        self.piece != None ? return false: self.peice = piece; return true
            

def processMove(checkboard, player):
    while(true):
        move = input("{} please input your turn".format(player)
        move_dict = {}
        if(not formatMove(move, move_list)):
            print("Wrong input fool")
            continue
        message = ""
        if(not checkboard.move(player, move_list, message))
            print("Can't move there fool")
    
    return true


def startGame():
    checkboard = []
    player1 = Player()
    player2 = Player()
    checkboard = Checkboard(player1, player2)
    turn = 1
    while(true):
        if(turn%2):
            player_on_turn = player1
        else:
            player_on_turn = player2
        processMove(player_on_turn, checkboard)
        if(not checkGameSate(checkboard)):
            break
    print("Sucessfully finished game")
    return 1

def initGame(player1, player2):
    
startGame()


            


