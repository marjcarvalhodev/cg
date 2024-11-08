#include "window.hpp"
#include "imgui.h"

void Window::onCreate() {
  loadElements();  // Carregar elementos químicos
  loadReactions(); // Carregar as reações químicas
}

void Window::loadElements() {
  m_elements = {{"Água", "H2O", ImVec4(0.0f, 0.5f, 1.0f, 1.0f)},
                {"Sódio", "Na", ImVec4(0.9f, 0.5f, 0.2f, 1.0f)},
                {"Cloro", "Cl", ImVec4(0.2f, 0.8f, 0.2f, 1.0f)},
                {"Oxigênio", "O2", ImVec4(0.8f, 0.8f, 0.2f, 1.0f)}};
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

void Window::onPaintUI() {
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
