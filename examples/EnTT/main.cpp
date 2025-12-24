#include <entt/entt.hpp>
#include <iostream>

struct position {
  float x;
  float y;
};

struct velocity {
  float dx;
  float dy;
};

void update(entt::registry& registry) {
  auto view = registry.view<const position, velocity>();

  // use a callback
  view.each([](const auto& pos, auto& vel) {
    /* ... */
  });

  // use an extended callback
  view.each([](const auto entity, const auto& pos, auto& vel) {
    /* ... */
  });

  // use a range-for
  for (auto [entity, pos, vel] : view.each()) {
    // ...
  }

  // use forward iterators and get only the components of interest
  for (auto entity : view) {
    auto& vel = view.get<velocity>(entity);
    // ...
  }
}

int main() {
  std::cout << "Hello from EnTT Minimal Project" << std::endl;
  std::cout << "Example based on: https://github.com/skypjack/entt?tab=readme-ov-file#code-example" << std::endl;

  entt::registry registry;

  for (auto i = 0u; i < 10u; ++i) {
    const auto entity = registry.create();
    registry.emplace<position>(entity, i * 1.f, i * 1.f);
    if (i % 2 == 0) {
      registry.emplace<velocity>(entity, i * .1f, i * .1f);
    }
  }

  update(registry);
}
