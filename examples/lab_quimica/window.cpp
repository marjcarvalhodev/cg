#include "window.hpp"
#include "imgui.h"

#include <glm/gtx/fast_trigonometry.hpp>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onCreate() {
  setup3D(); // Criar espaço 3D
  // loadElements();  // Carregar elementos químicos
  // loadReactions(); // Carregar as reações químicas
}

void Window::onPaint() {
  processInput();
  draw3D(); // Mostrar espaço 3D
}

void Window::onPaintUI() {
  return;
  auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)};
  auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};

  ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::Begin("Simulação de Laboratório Químico", nullptr,
               ImGuiWindowFlags_NoResize);

  // Elementos químicos disponíveis
  ImGui::Text("Elementos Químicos Disponíveis:");
  auto drawList = ImGui::GetWindowDrawList();
  for (size_t i = 0; i < m_elements.size(); ++i) {
    Element &element = m_elements[i];
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(100.0f, 100.0f);

    ImGui::InvisibleButton(("Erlenmeyer" + std::to_string(i)).c_str(), size);
    if (ImGui::IsItemClicked()) {
      addToBeaker(element); // Adiciona o elemento ao béquer
    }

    // Desenho do Erlenmeyer
    ImVec2 topCenter = ImVec2(pos.x + size.x / 2, pos.y);
    ImVec2 bottomLeft = ImVec2(pos.x, pos.y + size.y);
    ImVec2 bottomRight = ImVec2(pos.x + size.x, pos.y + size.y);
    ImVec2 neckLeft = ImVec2(pos.x + size.x * 0.4f, pos.y + size.y * 0.3f);
    ImVec2 neckRight = ImVec2(pos.x + size.x * 0.6f, pos.y + size.y * 0.3f);

    drawList->AddTriangleFilled(topCenter, neckLeft, neckRight,
                                ImColor(element.color));
    drawList->AddQuadFilled(neckLeft, neckRight, bottomRight, bottomLeft,
                            ImColor(element.color));

    ImVec2 textSize = ImGui::CalcTextSize(element.symbol.c_str());
    ImVec2 textPos = ImVec2(pos.x + (size.x - textSize.x) / 2,
                            pos.y + (size.y - textSize.y) / 2);
    drawList->AddText(textPos, ImColor(255, 255, 255), element.symbol.c_str());

    if (i < m_elements.size() - 1) {
      ImGui::SameLine();
    }
  }

  ImGui::Text("Recipiente de Mistura:");
  displayBeaker();

  // Botão Resetar Mistura
  if (ImGui::Button("Resetar Mistura")) {
    m_selectedElements.clear();
    m_currentReaction.reset();
  }

  ImGui::End();
}

void Window::loadElements() {
  m_elements = {{"Água", "H2O", ImVec4(0.0f, 0.5f, 1.0f, 1.0f)},
                {"Sódio", "Na", ImVec4(0.9f, 0.5f, 0.2f, 1.0f)},
                {"Cloro", "Cl", ImVec4(0.2f, 0.8f, 0.2f, 1.0f)},
                {"Oxigênio", "O2", ImVec4(0.8f, 0.8f, 0.2f, 1.0f)}};
}

void Window::add3DLabItem()
{
}

void Window::loadReactions() {
  // Definindo todas as combinações de reações
  reactions[{"Na", "Cl"}] = {"NaCl", ImVec4(1.0f, 1.0f, 0.0f, 1.0f)};
  reactions[{"H2O", "Na"}] = {"NaOH + H2", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)};
  reactions[{"H2O", "Cl"}] = {"HCl + HClO", ImVec4(0.0f, 1.0f, 1.0f, 1.0f)};
  reactions[{"H2O", "O2"}] = {"H2O2", ImVec4(0.5f, 0.5f, 1.0f, 1.0f)};
  reactions[{"Na", "O2"}] = {"Na2O", ImVec4(0.5f, 0.5f, 0.5f, 1.0f)};
  reactions[{"Cl", "O2"}] = {"Cl2O", ImVec4(0.3f, 0.6f, 1.0f, 1.0f)};
}

void Window::addToBeaker(Element element) {
  // Adiciona o elemento ao béquer se ainda não estiver nele
  if (std::find_if(m_selectedElements.begin(), m_selectedElements.end(),
                   [&](const Element &e) {
                     return e.symbol == element.symbol;
                   }) == m_selectedElements.end()) {
    m_selectedElements.push_back(element);
    checkReaction();
  }
}

void Window::displayBeaker() {
  auto drawList = ImGui::GetWindowDrawList();
  ImVec2 beakerPos = ImGui::GetCursorScreenPos();
  ImVec2 beakerSize = ImVec2(150.0f, 200.0f);

  drawList->AddRect(
      beakerPos, ImVec2(beakerPos.x + beakerSize.x, beakerPos.y + beakerSize.y),
      ImColor(255, 255, 255));

  if (m_currentReaction) {
    drawList->AddRectFilled(
        beakerPos,
        ImVec2(beakerPos.x + beakerSize.x, beakerPos.y + beakerSize.y),
        ImColor(m_currentReaction->resultColor));
    ImVec2 textSize = ImGui::CalcTextSize(m_currentReaction->result.c_str());
    ImVec2 textPos = ImVec2(beakerPos.x + (beakerSize.x - textSize.x) / 2,
                            beakerPos.y + (beakerSize.y - textSize.y) / 2);
    drawList->AddText(textPos, ImColor(255, 255, 255),
                      m_currentReaction->result.c_str());
  } else {
    float elementHeight = beakerSize.y / m_selectedElements.size();
    for (size_t i = 0; i < m_selectedElements.size(); ++i) {
      Element &element = m_selectedElements[i];
      ImVec2 elementPos = ImVec2(beakerPos.x, beakerPos.y + i * elementHeight);
      ImVec2 elementSize =
          ImVec2(beakerPos.x + beakerSize.x, elementPos.y + elementHeight);
      drawList->AddRectFilled(elementPos, elementSize, ImColor(element.color));
    }
  }
}

void Window::checkReaction() {
  if (m_selectedElements.size() == 2) {
    auto &e1 = m_selectedElements[0];
    auto &e2 = m_selectedElements[1];

    auto pair1 = std::make_pair(e1.symbol, e2.symbol);
    auto pair2 = std::make_pair(e2.symbol, e1.symbol);

    if (reactions.contains(pair1)) {
      m_currentReaction = reactions[pair1];
    } else if (reactions.contains(pair2)) {
      m_currentReaction = reactions[pair2];
    } else {
      m_currentReaction.reset();
    }
  } else {
    m_currentReaction.reset();
  }
}

void Window::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attributes{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      auto const vx{attributes.vertices.at(startIndex + 0)};
      auto const vy{attributes.vertices.at(startIndex + 1)};
      auto const vz{attributes.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = {vx, vy, vz}};

      // If map doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void Window::standardize() {
  // Center to origin and normalize bounds to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  // Center and scale
  auto const center{(min + max) / 2.0f};
  auto const scaling{2.0f / glm::length(max - min)};
  for (auto &vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void Window::draw3D() {
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);
  abcg::glBindVertexArray(m_VAO);

  // Draw triangles
  abcg::glDrawElements(GL_TRIANGLES, m_verticesToDraw, GL_UNSIGNED_INT,
                       nullptr);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Window::setup3D() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "basic_shader.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "basic_shader.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Load model
  loadModelFromFile(assetsPath + "models/tube.obj");
  standardize();

  m_verticesToDraw = m_indices.size();

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::snapCameraToModel() {
    // Calculate the bounding box of the model
    glm::vec3 minPoint = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxPoint = glm::vec3(std::numeric_limits<float>::lowest());

    for (const auto& vertex : m_vertices) {
        fmt::print("cade boneco: {}\n", m_vertices.size());

        minPoint = glm::min(minPoint, vertex.position);
        maxPoint = glm::max(maxPoint, vertex.position);
    }

    // Compute the center of the model
    glm::vec3 modelCenter = (minPoint + maxPoint) * 0.5f;

    // Compute the size of the model
    glm::vec3 modelSize = maxPoint - minPoint;
    float modelRadius = glm::length(modelSize) / 2.0f;

    // Set the camera distance to be large enough to fit the model
    float cameraDistance = modelRadius * 2.0f; // Adjust this multiplier as needed

    // Set camera position behind the model (along the Z-axis)
    cameraPos = modelCenter + glm::vec3(0.0f, 0.0f, cameraDistance);
    cameraTarget = modelCenter;

    // Update the view matrix
    viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
}

void Window::processInput() {
    // SDL event handling - check if the window is open
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            // Handle window close event
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);  // Get current key states

        // If 'N' key is pressed, snap camera to the model
    if (state[SDL_SCANCODE_N]) {
        snapCameraToModel();
    }

    // Move camera forward (W key)
    if (state[SDL_SCANCODE_W]) {
        cameraPos += cameraSpeed * glm::normalize(cameraTarget - cameraPos);
    }

    // Move camera backward (S key)
    if (state[SDL_SCANCODE_S]) {
        cameraPos -= cameraSpeed * glm::normalize(cameraTarget - cameraPos);
    }

    // Move camera left (A key)
    if (state[SDL_SCANCODE_A]) {
        cameraPos -= glm::normalize(glm::cross(cameraTarget - cameraPos, cameraUp)) * cameraSpeed;
    }

    // Move camera right (D key)
    if (state[SDL_SCANCODE_D]) {
        cameraPos += glm::normalize(glm::cross(cameraTarget - cameraPos, cameraUp)) * cameraSpeed;
    }
}
// eof