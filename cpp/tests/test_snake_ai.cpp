#include "ai/snake_ai.hpp"
#include "doctest.h"

TEST_CASE("heads toward food on an open board") {
    SnakeAI ai(10, 10);
    Snake snake(Cell{5, 5}, Direction::Right);
    auto direction = ai.Decide(snake, {Cell{8, 5}});
    REQUIRE(direction.has_value());
    CHECK(*direction == Direction::Right);
}

TEST_CASE("safe-after rejects a path that would seal off the tail") {
    // 3x2 grid where the body already walls off a 2-cell pocket
    // containing the food; the only way in is also the only way out.
    //   (0,0) (1,0)=head (2,0)=tail
    //   (0,1) (1,1)=body (2,1)=body
    SnakeAI ai(3, 2);
    std::deque<Cell> body{Cell{1, 0}, Cell{1, 1}, Cell{2, 1}, Cell{2, 0}};
    std::vector<Cell> pathIntoThePocket{Cell{1, 0}, Cell{0, 0}};
    CHECK_FALSE(ai.SafeAfter(body, pathIntoThePocket));
}

TEST_CASE("decide avoids the trap and chases its tail instead") {
    SnakeAI ai(3, 2);
    Snake snake(Cell{1, 0}, Direction::Right);
    snake.Body() = {Cell{1, 0}, Cell{1, 1}, Cell{2, 1}, Cell{2, 0}};

    auto direction = ai.Decide(snake, {Cell{0, 0}});

    REQUIRE(direction.has_value());
    CHECK(*direction != Direction::Left);   // would walk into the dead end
    CHECK(*direction == Direction::Right);  // chases the tail instead
}

TEST_CASE("hamiltonian mode always returns a legal next step") {
    SnakeAI ai(6, 4);
    Snake snake(Cell{0, 0}, Direction::Right);
    auto direction = ai.Decide(snake, {Cell{3, 2}}, /*useHamiltonian=*/true);
    CHECK(direction.has_value());
}
