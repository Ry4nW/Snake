"""AI decision logic. layered fallback: safe A* food -> tail chase -> flood-fill
survival -> optional hamiltonian cycle. plain A* to food can trap the snake in
a sealed pocket, so we BFS-check head-can-reach-tail before committing to it.
"""
from collections import deque

from .hamiltonian import build_hamiltonian_cycle
from .pathfinding import astar_path, bfs_path, direction_between, flood_fill_count, neighbors


class SnakeAI:
    def __init__(self, cols, rows):
        self.cols = cols
        self.rows = rows
        self._cycle = None
        self._cycle_index = None

    def decide(self, snake, food_cells, use_hamiltonian=False):
        if use_hamiltonian:
            return self._decide_hamiltonian(snake)

        body = list(snake.body)
        head = body[0]

        target = self._nearest(head, food_cells)
        if target is not None:
            blocked = set(body[:-1])
            path = astar_path(head, target, blocked, self.cols, self.rows)
            if path and len(path) > 1 and self._safe_after(body, path):
                return direction_between(path[0], path[1])

        tail_direction = self._chase_tail(body)
        if tail_direction is not None:
            return tail_direction

        return self._safest_move(body)

    def _nearest(self, head, food_cells):
        if not food_cells:
            return None
        return min(food_cells, key=lambda cell: abs(cell[0] - head[0]) + abs(cell[1] - head[1]))

    def _simulate_path(self, body, path):
        """body after following path; last step grows, skips tail pop"""
        simulated = deque(body)
        last_index = len(path) - 1
        for i, cell in enumerate(path[1:], start=1):
            simulated.appendleft(cell)
            if i < last_index:
                simulated.pop()
        return simulated

    def _safe_after(self, body, path):
        simulated = self._simulate_path(body, path)
        head, tail = simulated[0], simulated[-1]
        blocked = set(list(simulated)[:-1])
        return bfs_path(head, tail, blocked, self.cols, self.rows) is not None

    def _chase_tail(self, body):
        head, tail = body[0], body[-1]
        blocked = set(body[:-1])
        path = bfs_path(head, tail, blocked, self.cols, self.rows)
        if path and len(path) > 1:
            return direction_between(path[0], path[1])
        return None

    def _safest_move(self, body):
        head = body[0]
        blocked = set(body[:-1])
        best_direction, best_space = None, -1
        for nxt in neighbors(head, self.cols, self.rows):
            if nxt in blocked:
                continue
            space = flood_fill_count(nxt, blocked, self.cols, self.rows)
            if space > best_space:
                best_space = space
                best_direction = direction_between(head, nxt)
        return best_direction

    def _decide_hamiltonian(self, snake):
        if self._cycle is None:
            self._cycle = build_hamiltonian_cycle(self.cols, self.rows)
            self._cycle_index = {cell: i for i, cell in enumerate(self._cycle)}
        head = snake.head
        next_cell = self._cycle[(self._cycle_index[head] + 1) % len(self._cycle)]
        return direction_between(head, next_cell)
