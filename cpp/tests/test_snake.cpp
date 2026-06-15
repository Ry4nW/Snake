#include <algorithm>

#include "doctest.h"
#include "snake.hpp"

TEST_CASE("advances in the current direction") {
    Snake snake(Cell{5, 5}, Direction::Right);
    snake.Advance();
    CHECK(snake.Head() == Cell{6, 5});
    CHECK(snake.Length() == 1);
}

TEST_CASE("grow delays the tail pop by one tick") {
    Snake snake(Cell{5, 5}, Direction::Right);
    snake.Grow();
    snake.Advance();
    CHECK(snake.Length() == 2);
    bool tailStillPresent = std::find(snake.Body().begin(), snake.Body().end(), Cell{5, 5}) != snake.Body().end();
    CHECK(tailStillPresent);
}

TEST_CASE("cannot reverse directly into its own neck") {
    Snake snake(Cell{5, 5}, Direction::Right);
    snake.Grow();
    snake.Advance();  // length 2 now, so reversing would hit its own body
    snake.SetDirection(Direction::Left);
    CHECK(snake.CurrentDirection() == Direction::Right);
}

TEST_CASE("a single-segment snake can reverse") {
    Snake snake(Cell{5, 5}, Direction::Right);
    snake.SetDirection(Direction::Left);
    snake.Advance();
    CHECK(snake.Head() == Cell{4, 5});
}

TEST_CASE("hits wall outside the grid") {
    Snake snake(Cell{0, 0}, Direction::Left);
    snake.Advance();
    CHECK(snake.HitsWall(10, 10));
}

TEST_CASE("does not hit wall inside the grid") {
    Snake snake(Cell{5, 5}, Direction::Right);
    snake.Advance();
    CHECK_FALSE(snake.HitsWall(10, 10));
}

TEST_CASE("hits self when the head overlaps the body") {
    Snake snake(Cell{5, 5}, Direction::Right);
    snake.Body() = {Cell{5, 5}, Cell{6, 5}, Cell{6, 6}, Cell{5, 6}, Cell{5, 5}};
    CHECK(snake.HitsSelf());
}

TEST_CASE("does not hit self on a clear body") {
    Snake snake(Cell{5, 5}, Direction::Right);
    snake.Body() = {Cell{5, 5}, Cell{4, 5}, Cell{3, 5}};
    CHECK_FALSE(snake.HitsSelf());
}
