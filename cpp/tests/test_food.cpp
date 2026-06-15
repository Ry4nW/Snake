#include <algorithm>

#include "doctest.h"
#include "food.hpp"

TEST_CASE("reset spawns the configured count") {
    FoodManager food(3, 3, 2, /*seed=*/42);
    food.Reset({Cell{0, 0}});
    CHECK(food.Items().size() == 2);
}

TEST_CASE("reset never spawns on an occupied cell") {
    FoodManager food(3, 3, 2, /*seed=*/42);
    food.Reset({Cell{0, 0}});
    bool onOccupied = std::find(food.Items().begin(), food.Items().end(), Cell{0, 0}) != food.Items().end();
    CHECK_FALSE(onOccupied);
}

TEST_CASE("consume removes the eaten item and respawns a new one") {
    FoodManager food(3, 3, 1, /*seed=*/42);
    food.Reset({});
    Cell eaten = food.Items().front();

    bool consumed = food.Consume(eaten, {eaten});

    CHECK(consumed);
    CHECK(food.Items().size() == 1);
    bool stillThere = std::find(food.Items().begin(), food.Items().end(), eaten) != food.Items().end();
    CHECK_FALSE(stillThere);
}

TEST_CASE("consume ignores a cell with no food") {
    FoodManager food(3, 3, 1, /*seed=*/42);
    food.Reset({});
    CHECK_FALSE(food.Consume(Cell{2, 2}, {}));
}
