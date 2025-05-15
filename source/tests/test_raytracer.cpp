#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <glm/glm.hpp>
import Raytracer;

TEST_CASE("random number is in range", "[rand01]") {
    for (int i = 0; i < 1000; ++i) {
        double r = 0.4;
        REQUIRE(r >= 0.0);
        REQUIRE(r < 1.0);
    }
}

TEST_CASE("Reflect function works", "[reflect]") {
    glm::vec3 v = glm::vec3(1, -1, 0);
    glm::vec3 n = glm::vec3(0, 1, 0);
    glm::vec3 r = Engine::reflect(v, n);
    REQUIRE(r == glm::vec3(1, 1, 0));
}

