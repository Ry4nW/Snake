"""owns food positions and spawn logic, kept out of Snake/Game"""
import random


class FoodManager:
    def __init__(self, cols, rows, count=1, rng=None):
        self.cols = cols
        self.rows = rows
        self.count = count
        self._rng = rng or random
        self.items = []

    def reset(self, occupied):
        self.items = []
        for _ in range(self.count):
            self.spawn_one(occupied | set(self.items))

    def spawn_one(self, occupied):
        free_cells = [
            (x, y)
            for x in range(self.cols)
            for y in range(self.rows)
            if (x, y) not in occupied
        ]
        if not free_cells:
            return None
        cell = self._rng.choice(free_cells)
        self.items.append(cell)
        return cell

    def consume(self, cell, occupied):
        if cell not in self.items:
            return False
        self.items.remove(cell)
        self.spawn_one(occupied)
        return True
