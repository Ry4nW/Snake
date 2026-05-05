#include "snake.hpp"

#include <algorithm>

Snake::Snake(Cell start, Direction direction)
    : direction_(direction), pendingDirection_(direction) {
    body_.push_back(start);
}

void Snake::SetDirection(Direction direction) {
    if (Length() > 1 && direction == Opposite(direction_)) {
        return;
    }
    pendingDirection_ = direction;
}

void Snake::Grow(int amount) { growthPending_ += amount; }

Cell Snake::PeekNextHead() const {
    Cell delta = DirectionDelta(pendingDirection_);
    const Cell& head = Head();
    return Cell{head.col + delta.col, head.row + delta.row};
}

Cell Snake::Advance() {
    direction_ = pendingDirection_;
    Cell newHead = PeekNextHead();
    body_.push_front(newHead);
    if (growthPending_ > 0) {
        growthPending_--;
    } else {
        body_.pop_back();
    }
    return newHead;
}

bool Snake::HitsWall(int cols, int rows) const {
    const Cell& head = Head();
    return head.col < 0 || head.col >= cols || head.row < 0 || head.row >= rows;
}

bool Snake::HitsSelf() const {
    const Cell& head = Head();
    return std::find(body_.begin() + 1, body_.end(), head) != body_.end();
}
