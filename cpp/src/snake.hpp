// body, movement, growth, collision rules, no raylib dep
#pragma once

#include <deque>

#include "cell.hpp"
#include "direction.hpp"

class Snake {
public:
    Snake(Cell start, Direction direction);

    const Cell& Head() const { return body_.front(); }
    const Cell& Tail() const { return body_.back(); }
    const std::deque<Cell>& Body() const { return body_; }
    std::deque<Cell>& Body() { return body_; }
    int Length() const { return static_cast<int>(body_.size()); }
    Direction CurrentDirection() const { return direction_; }

    // can't reverse into own neck past length 1
    void SetDirection(Direction direction);

    void Grow(int amount = 1);

    Cell PeekNextHead() const;

    // steps one cell, applies growth, returns new head
    Cell Advance();

    bool HitsWall(int cols, int rows) const;
    bool HitsSelf() const;

private:
    std::deque<Cell> body_;
    Direction direction_;
    Direction pendingDirection_;
    int growthPending_ = 0;
};
