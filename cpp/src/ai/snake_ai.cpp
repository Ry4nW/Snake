#include "snake_ai.hpp"

#include <cstdlib>
#include <unordered_set>

#include "hamiltonian.hpp"
#include "pathfinding.hpp"

SnakeAI::SnakeAI(int cols, int rows) : cols_(cols), rows_(rows) {}

std::optional<Direction> SnakeAI::Decide(const Snake& snake, const std::vector<Cell>& food,
                                          bool useHamiltonian) {
    debug_ = AIDebugInfo{};

    if (useHamiltonian) {
        debug_.strategy = AIDebugInfo::Strategy::Hamiltonian;
        return DecideHamiltonian(snake);
    }

    const std::deque<Cell>& body = snake.Body();
    const Cell& head = body.front();

    auto target = Nearest(head, food);
    if (target) {
        std::unordered_set<Cell, CellHash> blocked(body.begin(), body.end() - 1);
        auto path = pathfinding::AStarPath(head, *target, blocked, cols_, rows_);
        if (path) {
            debug_.consideredPath = *path;
            debug_.consideredPathSafe = path->size() > 1 && SafeAfter(body, *path);
            if (debug_.consideredPathSafe) {
                debug_.strategy = AIDebugInfo::Strategy::SafeAStar;
                debug_.chosenPath = *path;
                return pathfinding::DirectionBetween((*path)[0], (*path)[1]);
            }
        }
    }

    if (auto tailDirection = ChaseTail(body)) return tailDirection;

    return SafestMove(body);
}

std::optional<Cell> SnakeAI::Nearest(const Cell& head, const std::vector<Cell>& food) const {
    if (food.empty()) return std::nullopt;
    Cell best = food.front();
    int bestDist = std::abs(best.col - head.col) + std::abs(best.row - head.row);
    for (const Cell& c : food) {
        int dist = std::abs(c.col - head.col) + std::abs(c.row - head.row);
        if (dist < bestDist) {
            bestDist = dist;
            best = c;
        }
    }
    return best;
}

std::deque<Cell> SnakeAI::SimulatePath(const std::deque<Cell>& body, const std::vector<Cell>& path) const {
    // last step grows, tail stays put; earlier steps pop the tail
    std::deque<Cell> simulated(body);
    int lastIndex = static_cast<int>(path.size()) - 1;
    for (int i = 1; i <= lastIndex; ++i) {
        simulated.push_front(path[static_cast<std::size_t>(i)]);
        if (i < lastIndex) simulated.pop_back();
    }
    return simulated;
}

bool SnakeAI::SafeAfter(const std::deque<Cell>& body, const std::vector<Cell>& path) const {
    std::deque<Cell> simulated = SimulatePath(body, path);
    Cell head = simulated.front();
    Cell tail = simulated.back();
    std::unordered_set<Cell, CellHash> blocked(simulated.begin(), simulated.end() - 1);
    return pathfinding::BfsPath(head, tail, blocked, cols_, rows_).has_value();
}

std::optional<Direction> SnakeAI::ChaseTail(const std::deque<Cell>& body) {
    const Cell& head = body.front();
    const Cell& tail = body.back();
    std::unordered_set<Cell, CellHash> blocked(body.begin(), body.end() - 1);
    auto path = pathfinding::BfsPath(head, tail, blocked, cols_, rows_);
    if (path && path->size() > 1) {
        debug_.strategy = AIDebugInfo::Strategy::TailChase;
        debug_.chosenPath = *path;
        return pathfinding::DirectionBetween((*path)[0], (*path)[1]);
    }
    return std::nullopt;
}

std::optional<Direction> SnakeAI::SafestMove(const std::deque<Cell>& body) {
    const Cell& head = body.front();
    std::unordered_set<Cell, CellHash> blocked(body.begin(), body.end() - 1);

    std::optional<Direction> bestDirection;
    int bestSpace = -1;
    std::vector<Cell> bestRegion;
    for (const Cell& next : pathfinding::Neighbors(head, cols_, rows_)) {
        if (blocked.count(next)) continue;
        std::vector<Cell> region = pathfinding::FloodFillRegion(next, blocked, cols_, rows_);
        if (static_cast<int>(region.size()) > bestSpace) {
            bestSpace = static_cast<int>(region.size());
            bestDirection = pathfinding::DirectionBetween(head, next);
            bestRegion = std::move(region);
        }
    }
    if (bestDirection) {
        debug_.strategy = AIDebugInfo::Strategy::FloodFillSurvival;
        debug_.floodFillRegion = std::move(bestRegion);
    }
    return bestDirection;
}

std::optional<Direction> SnakeAI::DecideHamiltonian(const Snake& snake) {
    if (cycle_.empty()) {
        cycle_ = hamiltonian::BuildCycle(cols_, rows_);
        for (std::size_t i = 0; i < cycle_.size(); ++i) cycleIndex_[cycle_[i]] = i;
    }
    const Cell& head = snake.Head();
    std::size_t idx = cycleIndex_.at(head);
    Cell next = cycle_[(idx + 1) % cycle_.size()];
    return pathfinding::DirectionBetween(head, next);
}
