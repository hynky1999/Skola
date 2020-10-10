#!/usr/bin/env python3
from abc import ABC, abstractmethod

""" Czech version of board game draughts.
Attributes:
    checkboard_size (int): specifies size of checkboard_size to be used
        only one digit number should be used for correct display
    cell_size (int): specifies size of cell for one box.

Rules:

    The draughtsboard has eight ranks and eight files. Players each have twelve
    draughtsmen on opposite sides of the board,
    arranged on black squares only, in three rows.
    Men move one square diagonally forward.
    Once a man has reached the furthest rank of
    the board from the owning player, it becomes a king.
    Kings move diagonally forward or backward any number of positions.

    A player who cannot move, either because he has lost all of his pieces,
    or because no legal move remains, has lost.
    The game is a draw when it is
    theoretically impossible (i.e. with perfect play)
    to capture any of the opponent's. (!Draw is not implemented!)

    Capture rules

    Captures are mandatory in Czech draughts:
    when a man is found adjacent to an opposing piece behind
    which is an empty position, the player must attain this empty position
    and remove the opposing man
    from the board. (!Backward jump possible only for king!)
    Should a player be able to capture either with a man or a king,
    he must do so with the king.
    (!Not implemented you can capture with any piece!)
    If multiple captures are possible,
    the player may choose between capturing one
    piece or the entire line; the capture of a partial line is not permitted.
    If multiple captures are performed, the pieces are removed from the board
    en masse when the capture is concluded.
    Players cannot capture their own pieces.
    [Source wikipedia]

Input from Players:
    At the begging both players input their name.
    Then they take turns,
    each player has to input valid start and end location of piece
    player wants to move. Input should have
    \alfa\digit \alfa\digit format with spaces anywhere.
    Examples: e1 b2, E3 A2, e1B5 etc..
    If players want to end game before victory of one of them simply pres Ctr-c
"""

# Only 1 digit numbers
checkboard_size = 8
cell_size = 3


class Location:
    """ Simple wrapper for location definition

    Attributes:
        y (int): y coordinate
        x (int): x coordinate
    """

    def __init__(self, y, x):
        """ Constructor of Location

        Args:
            y = point on horizontal axes
            x = point on vertical axes
        """
        self.y = y
        self.x = x

    def isDiagonal(self):
        """ Checks if location is diagonal eg. (1,-1)
        Returns:
            bool: true if diagonal else otherwise
        """
        return abs(self.y) == abs(self.x)

    def isZero(self):
        """ Checks if location is Zero eg. (0,0)

        Returns:
            bool: true if diagonal else otherwise
        """
        return self.x == 0 and self.y == 0

    def normalized(self):
        """ Creates and returns normalized location

        Doesn't make sense for non diagonal
        eg (4,4) -> (1,1), (-5,5) -> (-1,1)

        Returns:
            Location: normalized location
        """
        return Location(self.y//abs(self.y), self.x//abs(self.x))

    def __add__(self, other):
        return Location(self.y + other.y, self.x + other.x)

    def __iadd__(self, other):
        self.y += other.y
        self.x += other.x
        return self

    def __sub__(self, other):
        return Location(self.y - other.y, self.x - other.x)

    def __isub__(self, other):
        self.y -= other.y
        self.x -= other.x
        return self

    def __eq__(self, other):
        return self.y == other.y and self.x == other.x

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return "{}{}".format(chr(self.x + ord('A')), self.y + 1)


class Player:
    """ Class for representing/storing data about player his active pieces.

    Attributes:
        active_pieces (list): Player's pieces which are still in game.
        piece_chars (dict): mapping from pice type to 1 character.

    """

    def __init__(self, name, upgrade_row, piece_chars):
        """ Constructor of player class.

        Args:
            name (str): Name of player.

        """

        self.active_pieces = []
        self.piece_chars = dict(
                (k, v*cell_size) for k, v in piece_chars.items()
        )
        self.__name = name
        self.__upgrade_row = upgrade_row

    def findJumpable(self, checkboard):
        """ Function which finds first player's piece that can jump.

        Args:
            checkboard (Checkboard): context = checkboard on which
                the piece is placed.

        Returns Piece: first jumpable piece found else None.
        """
        for piece in self.active_pieces:
            if(piece.jumpPossible(checkboard)):
                return piece

        return None

    def findMoveable(self, checkboard):
        """ Function which finds first player's piece that can move/jump.

        Args:
            checkboard (Checkboard): context = checkboard on
                which the piece is placed.

        Returns Piece: first moveable piece by non jumping found else None.
        """
        for piece in self.active_pieces:
            if(
                piece.movePossible(checkboard)
                or piece.jumpPossible(checkboard)
            ):
                return piece
        return None

    def upgradableLocation(self, piece):
        """ Function to check if piece in upgadable row.

        Args:
            piece (Piece): piece to be checked.

        """
        if(piece.getLocation().y == self.__upgrade_row):
            return True
        return False

    def __str__(self):
        return self.__name


class Piece(ABC):
    """ Abstract Base class representing single piece on checkboard.

    Piece is abstract class which serves as wrapper
    for checking possible moves.
    Piece is aware of its position board however
    when moving piece it has to be populated
    to Piece and Checkboard matrix(board).

    Attributes:
        player (Player): owner of piece.

    """

    def __init__(self, player, location):
        """ Constructor of Piece

        Args:
            player (Player): owner of piece.
            location (Location): location of piece on checkboard.

        """

        self.player = player
        self._location = location

    @abstractmethod
    def canMoveTo(self, checkboard, end):
        """ Method which checks if piece can move to end location.

            Care with usage of this function there is no checking of boundries.

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed.
            end (Location): end location of movement.

        Returns:
            Location: Returns location of overleaped piece if exists else None.
                If move is not possible raises Error.

        Raises:
            IndexError: If end location is not valid.
            ValueError: If move is not diagonal/too long or direction is wrong.

        """
        pass

    def moveTo(self, end):
        """ Method for moving piece to specified location no checking.

        This method is basically setter for self._location
        and python decorators could be
        used in this place but this makes more sense from object perspective.

        Args:
            end (Location): end location of movement.

        Returns:
            None
        """
        self._location = end

    def getLocation(self):
        """ Method for getting location of Piece on board

        This method is basically getter for self._location
        and python decorators could be
        used in this place but this makes more sense from object perspective.

        Returns:
            Location: location of piece
        """
        return self._location

    @abstractmethod
    def movePossible(self, checkboard):
        """ Method which checks if piece can move to any location.

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed.

        Returns:
            Location: location of possible jump/move, else None

        """
        pass

    @abstractmethod
    def jumpPossible(self, checkboard):
        """ Method which checks if piece can jump over enemey piece.

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed.

        Returns:
            Location: location of possible jump, else None.
        """
        pass

    @abstractmethod
    def __str__(self):
        """ Overloaded str method returning piece character.

        Returns:
            str: Character which represents the piece.
        """
        pass


class Man(Piece):
    """ Class representing single man on checkboard.

    Implementation of Piece class and is little bit overkill for such a small
    application but would be usefull for further scalability.
    Man can only jump in 1 y axis diagonally and by one box at a time.
    """

    def __init__(self, player, location, y_direction=1):
        """ Constructor of Man class

        Args:
            player (Player): owner of man
            location (Location): location of piece.
            y_direction (int): integer
                which represent moveable direction on y axis.
        """

        super().__init__(player, location)
        self.__y_direction = y_direction

    def canMoveTo(self, checkboard, end):
        """ Implementation of Base class function canMoveTo.

        Function checks if it is possible to move/jump to end location
        Care with usage of this function there is no checking of boundries.

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed.
            end (Location): end location of movement.

        Returns:
            Location: Returns location of overleaped piece if exists else None.
                If move is not possible raises Error.

        Raises:
            IndexError: If end location is not valid.
            ValueError: If move is not diagonal/too long or direction is wrong.
        """

        if(checkboard.getBoard(end)):
            raise IndexError(
                    "Can't move to {} because it is already occupied !"
                    .format(end)
                    )

        direction = end - self._location
        if(direction.isZero() or not direction.isDiagonal()):
            raise ValueError("Not diagonal Move !")

        normalized_dir = direction.normalized()
        if(self.__y_direction != normalized_dir.y):
            raise ValueError(
                    "This man is allowed to only move forward({}) !"
                    .format(self.__y_direction)
                    )

        check_loc = self._location + normalized_dir
        overleaped = checkboard.getBoard(check_loc)
        if(check_loc == end and not overleaped):
            return None

        elif(
            overleaped
            and overleaped.player != self.player
            and check_loc + normalized_dir == end
        ):
            return check_loc

        raise ValueError("Piece can only move 1 box foward !")

    def movePossible(self, checkboard):
        """ Implementation of Base class function movePossible.

        Method which checks if piece can move to any location.

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed.

        Returns:
            Location: location of possible move/jump.

        """

        # Try move to nearest boxes
        for j in (self.__y_direction,):
            for i in (1, -1):
                try:
                    self.canMoveTo(checkboard, self._location + Location(j, i))
                    return Location(j, i)

                # except (IndexError, ValueError):
                except (ValueError, IndexError):
                    continue
        # If not possible move try to jump over them
        return None

    def jumpPossible(self, checkboard):
        """ Implementation of Base class function jumpPossible.

        Function tries all possible jump location
        until it find one possible or none.

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed.
            start (Location): start location of jump.

        Returns:
            Location: location of possible jump else None.
        """

        # Go to all directions possible
        for j in (self.__y_direction,):
            for i in (1, -1):
                # Set check location at only jumpable box
                check_loc = self._location + Location(j, i)
                try:
                    other_piece = checkboard.getBoard(check_loc)
                    if(
                        other_piece
                        and other_piece.player != self.player
                        and not checkboard.getBoard(check_loc + Location(j, i))
                    ):
                        return check_loc + Location(j, i)
                except IndexError:
                    # Men cannot jump to box not on checkboard
                    continue
        return None

    def __str__(self):
        return self.player.piece_chars['Man']


class King(Piece):
    """ Class representing single King on checkboard.

    Implementation of Piece which represents King
    little bit overkill for such a small
    application but would be usefull for further scalability.
    Unlike Man, King can jump to any diagonal location.
    """

    def __init__(self, player, location):
        super().__init__(player, location)

    def canMoveTo(self, checkboard, end):
        """ Implementation of Base class function canMoveFromTo

            Checks if man can move/jump to specific location from start
            Care with usage of this function there is no checking of boundries

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed
            end (Location): end location of movement

        Returns:
            bool, Location: Returns if it is possible to move to location
                Location is location of overleaped piece if exists

        Raises:
            IndexError: If end location is not valid
            ValueError: If move is not diagonal or direction is wrong
        """

        if(checkboard.getBoard(end)):
            raise IndexError(
                    "Can't move to {} because it is already occupied !"
                    .format(end)
            )

        direction = end - self._location
        if(direction.isZero() or not direction.isDiagonal()):
            raise ValueError("Not diagonal Move !")

        normalized_dir = direction.normalized()

        check_loc = self._location + normalized_dir
        # Check boxes between start and end-2
        for i in range(1, abs(direction.x) - 1):
            if(checkboard.getBoard(check_loc)):
                raise ValueError(
                        "There is something in the way on {} !"
                        .format(check_loc)
                )
            check_loc += normalized_dir

        overleaped = checkboard.getBoard(check_loc)
        if(not overleaped):
            return None

        elif(overleaped.player != self.player):
            return check_loc

        raise ValueError("Cannot jump over my player's piece or over nothing ")

    def movePossible(self, checkboard):
        """ Implementation of Base class function movePossible.

        Method which checks if piece can move to any location.

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed.

        Returns:
            Location: location of possible move/jump.

        """

        # Try move to nearest boxes
        for j in (1, -1):
            for i in (1, -1):
                try:
                    # If we cannot move by 1 box we cannot move by more
                    self.canMoveTo(checkboard, self._location + Location(j, i))
                    return Location(j, i)

                except (IndexError, ValueError):
                    continue

        # If not possible move try to jump over them
        # Even at this class it will try to jump over nearest boxes
        return None

    def jumpPossible(self, checkboard):
        """ Implementation of Base class function jumpPossible.

        Function tries all possible jump location
        until it finds one possible or none.

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed
            start (Location): start location of jump

        Returns:
            Location: location of possible jump else None.
        """

        # Go to all directions possible
        for j in (1, -1):
            for i in (1, -1):
                # Set check location at only jumpable box
                try:
                    check_loc = self._location + Location(j, i)
                    other_piece = checkboard.getBoard(check_loc)
                    while(not other_piece):
                        check_loc += Location(j, i)
                        other_piece = checkboard.getBoard(check_loc)

                    if(
                        other_piece.player != self.player
                        and not checkboard.getBoard(check_loc + Location(j, i))
                    ):
                        return check_loc + Location(j, i)
                except IndexError:
                    # Men cannot jump to box not on checkboard
                    continue
        return None

    def __str__(self):
        return self.player.piece_chars['King']


class Checkboard:
    """ Class representing checkboard represent as matrix.

    This class is basically interfrace for manipulating with pieces on
    checkboard itself. Each box on checkboard is represented by matrix
    thus number is as in matrix first y axis then x axis and indexing is
    from zero.
    """

    def __init__(self, player1, player2):
        """ Constructor of Checkboard class.

        Args:
            player1 (Player): first player in the game.
            player2 (Player): second player in the game.
        """

        self.__board = []
        global checkboard_size
        # Create board
        for i in range(checkboard_size):
            self.__board += [[]]
            for j in range(checkboard_size):
                self.__board[i] += [None]

        # Add default setup for both players
        for i in range(3):
            for j in range(i % 2, checkboard_size, 2):
                self.__board[i][j] = Man(player1, Location(i, j), 1)
                player1.active_pieces += [self.__board[i][j]]

        for i in range(checkboard_size-3, checkboard_size):
            for j in range(i % 2, checkboard_size, 2):
                self.__board[i][j] = Man(player2, Location(i, j), -1)
                player2.active_pieces += [self.__board[i][j]]

    def getBoard(self, location):
        """ Getter of self.board.

        Args:
            location (Location): lookup location.

        Returns:
            Piece: returns Piece on board at specified location
            or None if there is none.

        Raises:
            IndexError: If location is not on board.
        """
        if(not self.__checkBoundry(location)):
            raise IndexError("Location is out of bound")

        return self.__board[location.y][location.x]

    def checkPlayerDefeat(self, checkboard,  player):
        """ Function which checks if player is defeated.

        Player is defeat if either 1) has no active pieces on board
        2) cannot move with any of his pieces.

        Args:
            checkboard (Checkboard): context = checkboard
                on which the piece is placed.
            player (Player): player to check against.

        Returns:
            bool: True if is defeat else otherwise.
        """
        if(
            not player.active_pieces
            or not player.findMoveable(checkboard)
        ):
            return True

        return False

    def move(self, player, move_dict):
        """ Core function Checkboard, moves piece to specified location by move_dict.

        move_dict must contain two keys: "start" and "end"
        which specifies starting and ending location of move.

        Args:
            player (Player): player who is doing the move.
            move_dict (dict:`str`:`Location`): Dictionary
                with keys "start" and "end" and their values of type Location

        Returns:
            (Piece, Location): piece which was moved
                and location if an overleap occured

        Raises:
            ValueError: If there was problem with moving a piece
                because of ownership etc.
            IndexError: If locations are out of bounds
                or end location is occupied etc.
        """

        piece = self.getBoard(move_dict["start"])
        if(not piece):
            raise IndexError("No piece on {} !".format(move_dict["start"]))

        if(piece.player != player):
            raise ValueError(
                    "You are trying to move with other's player piece !"
            )

        overleaped_location = piece.canMoveTo(self, move_dict["end"])
        if(not overleaped_location):
            jumpable_piece = player.findJumpable(self)
            if(jumpable_piece):
                # We have to move piece back if failed
                raise ValueError(
                        "You have to make a jump with {} !"
                        .format(jumpable_piece.getLocation())
                )

            self.__board[move_dict["end"].y][move_dict["end"].x] = piece
            self.__board[move_dict["start"].y][move_dict["start"].x] = None
            piece.moveTo(move_dict["end"])
        else:
            overleaped_piece = self.getBoard(overleaped_location)

            self.__board[move_dict["end"].y][move_dict["end"].x] = piece
            self.__board[move_dict["start"].y][move_dict["start"].x] = None
            piece.moveTo(move_dict["end"])

            self.__board[overleaped_location.y][overleaped_location.x] = None
            overleaped_piece.player.active_pieces.remove(overleaped_piece)

        if(player.upgradableLocation(piece) and isinstance(piece, Man)):
            king = King(player, piece.getLocation())
            player.active_pieces.remove(piece)
            player.active_pieces.append(king)
            self.__board[king.getLocation().y][king.getLocation().x] = king
            piece = king
        return piece, overleaped_location

    def __checkBoundry(self, location):
        """ Helper method for checking boundries of checkboard

        Returns bool: True if location is withing boundries else False
        """

        if(
            location.x >= len(self.__board)
            or location.x < 0
            or location.y >= len(self.__board)
            or location.y < 0
        ):
            return False
        return True

    def __rowString(self):
        """ Helper method for creating line for print """

        string = ""
        for i in range(checkboard_size + 1):
            for j in range(cell_size):
                string += "-"
            string += "+"
        string += '\n'
        return string

    def __str__(self):
        """ Reprenation of checkboard where each size has size of (1,cell_size)
        eg. 1 height and cell_size in width

        Returns:
            str: Reprenation of checkboard
        """

        global checkboard_size
        global cell_size
        string = ""
        for i in range(checkboard_size-1, -1, -1):
            string += self.__rowString()
            string += ' ' * ((cell_size-1) // 2) \
                + str(i+1) + \
                ' ' * ((cell_size-1) - (cell_size-1)//2)
            for j in range(checkboard_size):
                string += '|'
                if(self.__board[i][j] is None):
                    string += ' ' * cell_size
                else:
                    string += str(self.__board[i][j])
            string += '|\n'
        string += self.__rowString()
        string += '#' * cell_size
        for i in range(checkboard_size):
            string += '|'
            string += ' ' * ((cell_size-1)//2) \
                + chr(ord('A') + i) \
                + ' ' * ((cell_size-1) - (cell_size-1)//2)

        string += '|\n'
        return string


def formatMove(move):
    """ Simple function which formats user input
    Expected input is [a-zA-Z]\\d[a-zA-z]\\d with spaces anywhere.

    Args:
        move (str): input of user describing move.

    Returns:
        dict::`str`:`Location`: dictionary with keys "start" and "end".
            and their respecitve location
            in range(checkboard_size-1, checkboard_size-1).
    Raises:
        ValueError: if format is wrong.
    """
    move = move.upper().replace(" ", "")
    try:
        return {
            "start": Location(int(move[1]) - 1, ord(move[0]) - ord('A'))
            , "end": Location(int(move[3]) - 1, ord(move[2]) - ord('A'))
        }
    except (IndexError, ValueError):
        raise ValueError("Input should look like [a-z][0-9] [a-z][0-9]")


def processMove(checkboard, player):
    """ Function for getting user input and moving Piece.

    Args:
        checkboard (Checkboard): context = checkboard
            on which the piece is placed.
        player (Player): player who is doing the move.

    Returns:
        None
    """

    while(True):
        print(checkboard)
        move = input("{} please input your turn: ".format(player))
        try:
            move_dict = formatMove(move)
            piece, overleap = checkboard.move(player, move_dict)
            if(overleap and piece.jumpPossible(checkboard)):
                print("You can jump again !")
                continue
            break

        except (ValueError, IndexError) as e:
            print(e)


def startGame():
    """Function which setup and loops the game.

    Current implementation setup game for two player who input
    their names and then game starts.
    White man is 'x', white king is 'X'.
    Black man is 'o', black ing is 'O'.
    First white piece is placed on A1 second on C1 etc. for first 3 rows.
    First black piece is placed on A8 second on C8 etc. for last 3 rows.

    Returns:
        None
    """
    name1 = input("Player1 please input your name: ")
    name2 = input("Player2 please input your name: ")

    player1 = Player(name1, checkboard_size-1, {'Man': 'x', 'King': 'X'})
    player2 = Player(name2, 0, {'Man': 'o', 'King': 'O'})
    checkboard = Checkboard(player1, player2)
    turn = 1
    while(True):
        if(turn % 2):
            player_on_turn = player1
        else:
            player_on_turn = player2

        winner = checkboard.checkPlayerDefeat(checkboard, player_on_turn)
        if(winner):
            print(checkboard)
            print(
                "{} has won !"
                .format(player2 if player_on_turn == player1 else player1)
            )
            # Zero on success
            return 0

        print("Turn: {}".format(turn))
        try:
            processMove(checkboard, player_on_turn)
        except EOFError:
            print("No more commands")
            return 1

        turn += 1


startGame()
