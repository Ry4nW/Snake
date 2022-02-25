"""generic grid search (col, row) cells; blocked is a set. no snake/food knowledge."""
from collections import deque
import heapq


def in_bounds(cell, cols, rows):
    x, y = cell
    return 0 <= x < cols and 0 <= y < rows


def neighbors(cell, cols, rows):
    x, y = cell
    for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
        nxt = (x + dx, y + dy)
        if in_bounds(nxt, cols, rows):
            yield nxt


def _reconstruct(came_from, start, goal):
    path = [goal]
    while path[-1] != start:
        path.append(came_from[path[-1]])
    path.reverse()
    return path


def bfs_path(start, goal, blocked, cols, rows):
    """shortest path, or None if unreachable"""
    if start == goal:
        return [start]
    visited = {start}
    queue = deque([start])
    came_from = {}
    while queue:
        current = queue.popleft()
        for nxt in neighbors(current, cols, rows):
            if nxt in visited or nxt in blocked:
                continue
            visited.add(nxt)
            came_from[nxt] = current
            if nxt == goal:
                return _reconstruct(came_from, start, goal)
            queue.append(nxt)
    return None


def astar_path(start, goal, blocked, cols, rows):
    """manhattan-distance A*, or None if unreachable. same path length as bfs, fewer expansions."""
    def heuristic(a, b):
        return abs(a[0] - b[0]) + abs(a[1] - b[1])

    counter = 0  # tie-breaker so heap never compares cell tuples
    open_heap = [(heuristic(start, goal), 0, counter, start)]
    came_from = {}
    best_g = {start: 0}
    closed = set()

    while open_heap:
        _, g, _, current = heapq.heappop(open_heap)
        if current in closed:
            continue
        if current == goal:
            return _reconstruct(came_from, start, goal)
        closed.add(current)

        for nxt in neighbors(current, cols, rows):
            if nxt in blocked or nxt in closed:
                continue
            tentative_g = g + 1
            if tentative_g < best_g.get(nxt, float('inf')):
                best_g[nxt] = tentative_g
                came_from[nxt] = current
                counter += 1
                heapq.heappush(open_heap, (tentative_g + heuristic(nxt, goal), tentative_g, counter, nxt))
    return None


def flood_fill_count(start, blocked, cols, rows):
    """reachable cell count from start, ignoring blocked. proxy for available space."""
    if start in blocked:
        return 0
    visited = {start}
    queue = deque([start])
    count = 0
    while queue:
        current = queue.popleft()
        count += 1
        for nxt in neighbors(current, cols, rows):
            if nxt in visited or nxt in blocked:
                continue
            visited.add(nxt)
            queue.append(nxt)
    return count


def direction_between(a, b):
    return (b[0] - a[0], b[1] - a[1])
