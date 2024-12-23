from typing import List, Dict, Set
from dataclasses import dataclass
from enum import Enum
import copy
from sudoku_net.src.utils.logging_config import setup_logger

# Setup logging configuration
logger = setup_logger(__name__)

class GameState(Enum):
    in_progress = "in_progress"
    complete = "complete"
    failed = "failed"

@dataclass
class Cell:
    key: str
    candidates: List[int]
    peers: Set[str]
    units: List[List[str]]

    def has_candidate(self, digit: int) -> bool:
        return digit in self.candidates

    def remove_candidate(self, digit: int) -> None:
        if digit in self.candidates:
            self.candidates.remove(digit)

class Game:
    def __init__(self, cells: Dict[str, Cell]):
        self.cells = copy.deepcopy(cells)
        self.state = GameState.in_progress

class Variables:
    def __init__(self):
        self.digits = list(range(1, 10))
        self.rows = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I']
        self.cols = ['1', '2', '3', '4', '5', '6', '7', '8', '9']
        self.squares = self.cross(self.rows, self.cols)
        self.cells: Dict[str, Cell] = {}
        
        unitlist = []
        self.setup_unitlist(unitlist)
        self.setup_cells(unitlist)
        self.setup_peers()

    def cross(self, A: List[str], B: List[str]) -> List[str]:
        return [a + b for a in A for b in B]

    def setup_unitlist(self, unitlist: List[List[str]]) -> None:
        # Column-wise units
        col_cross = [[r + c for r in self.rows] for c in self.cols]
        
        # Row-wise units
        row_cross = [[r + c for c in self.cols] for r in self.rows]
        
        # Box units
        row_units = [self.rows[i:i + 3] for i in range(0, 9, 3)]
        col_units = [self.cols[i:i + 3] for i in range(0, 9, 3)]
        
        unit_cross = []
        for ru in row_units:
            for cu in col_units:
                unit_cross.append([r + c for r in ru for c in cu])
        
        unitlist.extend(col_cross)
        unitlist.extend(row_cross)
        unitlist.extend(unit_cross)

    def setup_cells(self, unitlist: List[List[str]]) -> None:
        for k in self.squares:
            cell = Cell(key=k, candidates=list(self.digits), peers=set(), units=[])
            for unit in unitlist:
                if k in unit:
                    cell.units.append(unit)
            self.cells[k] = cell

    def setup_peers(self) -> None:
        for k in self.squares:
            peers = set()
            for unit in self.cells[k].units:
                for v in unit:
                    if v != k:
                        peers.add(v)
            self.cells[k].peers = peers

class Solver:
    def __init__(self):
        self.data = Variables()

    def solve(self, grid: List[int]) -> None:
        game = Game(self.data.cells)
        success = self.setup_game_with_grid(grid, game)
        
        if not success:
            logger.error("Could not setup game! Please check your grid.")
            return
        
        finished = self.search(game)
        return finished

    def setup_game_with_grid(self, grid: List[int], game: Game) -> bool:
        success = True
        for i, digit in enumerate(grid):
            if digit != 0:
                square = self.data.squares[i]
                if not self.assign(game, square, digit):
                    success = False
        return success

    def assign(self, game: Game, key: str, digit: int) -> bool:
        cell = game.cells[key]
        others = [d for d in cell.candidates if d != digit]
        
        for other in others:
            if not self.eliminate(game, key, other):
                return False
        return True

    def eliminate(self, game: Game, key: str, digit: int) -> bool:
        cell = game.cells[key]
        if not cell.has_candidate(digit):
            return True
            
        cell.remove_candidate(digit)

        if len(cell.candidates) == 0:
            return False
        elif len(cell.candidates) == 1:
            value = cell.candidates[0]
            for peer in cell.peers:
                if not self.eliminate(game, peer, value):
                    return False

        for unit in cell.units:
            dplaces = [s for s in unit if game.cells[s].has_candidate(digit)]
            if len(dplaces) == 0:
                return False
            elif len(dplaces) == 1:
                if not self.assign(game, dplaces[0], digit):
                    return False
        return True

    def search(self, game: Game) -> Game:
        if self.is_solved(game.cells):
            game.state = GameState.complete
            logger.info("Solved!")
            return game

        if game.state == GameState.failed:
            return game

        # Find the square with the fewest candidates
        n, s = min((len(game.cells[s].candidates), s) 
                   for s in self.data.squares 
                   if len(game.cells[s].candidates) > 1)

        for d in game.cells[s].candidates:
            game_copy = Game(game.cells)
            if self.assign(game_copy, s, d):
                result = self.search(game_copy)
                if result.state != GameState.failed:
                    return result
        
        game.state = GameState.failed
        return game

    @staticmethod
    def is_solved(cells: Dict[str, Cell]) -> bool:
        return all(len(cell.candidates) == 1 for cell in cells.values())