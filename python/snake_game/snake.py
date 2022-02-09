"""snake body, movement, growth, collisions. grid cells only, no pygame import"""
from collections import deque

UP = (0, -1)
DOWN = (0, 1)
LEFT = (-1, 0)
RIGHT = (1, 0)


def opposite(direction):
    dx, dy = direction
    return (-dx, -dy)


class Snake:
    def __init__(self, start_cell, direction=RIGHT):
        self.body = deque([start_cell])
        self.direction = direction
        self._pending_direction = direction
        self._growth_pending = 0

    @property
    def head(self):
        return self.body[0]

    @property
    def tail(self):
        return self.body[-1]

    @property
    def length(self):
        return len(self.body)

    def set_direction(self, direction):
        # can't reverse into own neck if longer than 1
        if self.length > 1 and direction == opposite(self.direction):
            return
        self._pending_direction = direction

    def grow(self, amount=1):
        self._growth_pending += amount

    def peek_next_head(self):
        dx, dy = self._pending_direction
        hx, hy = self.head
        return (hx + dx, hy + dy)

    def advance(self):
        """steps one cell, applies growth, returns new head"""
        self.direction = self._pending_direction
        new_head = self.peek_next_head()
        self.body.appendleft(new_head)
        if self._growth_pending > 0:
            self._growth_pending -= 1
        else:
            self.body.pop()
        return new_head

    def hits_wall(self, cols, rows):
        x, y = self.head
        return x < 0 or x >= cols or y < 0 or y >= rows

    def hits_self(self):
        head = self.head
        body_iter = iter(self.body)
        next(body_iter)  # skip the head itself
        return head in body_iter

    def occupied_cells(self):
        return set(self.body)
