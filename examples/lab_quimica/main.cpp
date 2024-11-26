#include "window.hpp"
#include <fmt/core.h>
#include <stdexcept>

int main(int argc, char **argv) {
  try {
    abcg::Application app(argc, argv);

    Window window;
    window.setWindowSettings({.width = 1080,
                              .height = 960,
                              .title = "Simulação de Laboratório Químico"});

    app.run(window);
  } catch (std::exception const &exception) {
    fmt::print(stderr, "{}\n", exception.what());
    return -1;
  }
  return 0;
}
