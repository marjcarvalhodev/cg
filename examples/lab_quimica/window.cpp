#include "window.hpp"
#include "imgui.h"

void Window::onCreate() {
  // Carregar elementos químicos
  loadElements();

  // Carregar as reações químicas
  loadReactions();
}

void Window::loadElements() {
  // Definir elementos químicos (exemplo)
  m_elements.push_back({"Água", "H2O", ImVec4(0.0f, 0.5f, 1.0f, 1.0f)});
  m_elements.push_back({"Sódio", "Na", ImVec4(0.9f, 0.5f, 0.2f, 1.0f)});
  m_elements.push_back({"Cloro", "Cl", ImVec4(0.2f, 0.8f, 0.2f, 1.0f)});
  // Adicione outros elementos conforme necessário...
}

void Window::loadReactions() {
  // Definir algumas reações químicas
  reactions[std::make_pair("Na", "Cl")] = {
      "NaCl", ImVec4(1.0f, 1.0f, 0.0f, 1.0f)}; // Exemplo de reação
  reactions[std::make_pair("H2O", "Na")] = {"NaOH + H2",
                                            ImVec4(1.0f, 0.0f, 0.0f, 1.0f)};
  // Adicione outras reações conforme necessário...
}

void Window::onPaintUI() {
  auto const appWindowWidth{gsl::narrow<float>(getWindowSettings().width)};
  auto const appWindowHeight{gsl::narrow<float>(getWindowSettings().height)};

  ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::Begin("Simulação de Laboratório Químico", nullptr,
               ImGuiWindowFlags_NoResize);

  // Calcula o centro horizontal da janela
  float centerX = appWindowWidth / 2.0f;

  // Título: "Elementos Químicos Disponíveis" centralizado horizontalmente
  ImGui::SetCursorPosX(
      centerX - ImGui::CalcTextSize("Elementos Químicos Disponíveis:").x / 2);
  ImGui::Text("Elementos Químicos Disponíveis:");

  ImGui::Spacing(); // Espaçamento entre o título e os elementos

  // Calcula a largura total para centralizar os elementos
  float elementWidth = 100.0f;
  float elementHeight = 100.0f;
  float spacing = 20.0f;

  float totalWidth =
      (m_elements.size() * elementWidth) + ((m_elements.size() - 1) * spacing);
  float startPosX = centerX - totalWidth / 2;

  // Posiciona o cursor no início da linha para centralizar horizontalmente
  ImGui::SetCursorPosX(startPosX);

  // Exibe os elementos lado a lado
  auto drawList = ImGui::GetWindowDrawList();
  for (size_t i = 0; i < m_elements.size(); ++i) {
    Element &element = m_elements[i];
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(elementWidth, elementHeight);

    ImGui::InvisibleButton(("Erlenmeyer" + std::to_string(i)).c_str(), size);
    if (ImGui::IsItemClicked()) {
      addToBeaker(element);
    }

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
      ImGui::SameLine(); // Coloca o próximo elemento na mesma linha
      ImGui::SetCursorPosX(startPosX + (i + 1) * (elementWidth + spacing));
    }
  }

  ImGui::SetCursorPosY(
      ImGui::GetCursorPosY() +
      20.0f); // Ajuste para espaço entre elementos e o próximo título

  // Título: "Recipiente de Mistura" centralizado horizontalmente
  ImGui::SetCursorPosX(centerX -
                       ImGui::CalcTextSize("Recipiente de Mistura:").x / 2);
  ImGui::Text("Recipiente de Mistura:");

  ImGui::Spacing(); // Espaçamento entre o título e o béquer

  // Exibir o recipiente de mistura centralizado horizontalmente
  ImGui::SetCursorPosX(centerX -
                       75.0f); // Centraliza o béquer de 150 de largura
  displayBeaker();

  ImGui::Spacing(); // Espaçamento entre o béquer e o botão

  // Botão Resetar Mistura centralizado horizontalmente
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 210.0f);
  ImGui::SetCursorPosX(centerX - ImGui::CalcTextSize("Resetar Mistura").x / 2 -
                       4.0f);
  if (ImGui::Button("Resetar Mistura")) {
    m_selectedElements.clear();
    m_currentReaction.reset(); // Limpa a reação atual
  }

  ImGui::End();
}

void Window::addToBeaker(Element element) {
  if (m_selectedElements.size() < 2) {
    m_selectedElements.push_back(element);
    checkReaction(); // Verifica a reação sempre que um elemento é adicionado
  }
}

void Window::displayBeaker() {
  auto drawList = ImGui::GetWindowDrawList();
  ImVec2 beakerPos = ImGui::GetCursorScreenPos();
  ImVec2 beakerSize = ImVec2(150.0f, 200.0f);

  // Desenho do contorno do béquer
  ImVec2 beakerTopLeft = beakerPos;
  ImVec2 beakerBottomRight =
      ImVec2(beakerPos.x + beakerSize.x, beakerPos.y + beakerSize.y);
  drawList->AddRect(beakerTopLeft, beakerBottomRight, ImColor(255, 255, 255));

  if (m_currentReaction) {
    // Se houver uma reação válida, exibe o resultado no béquer
    drawList->AddRectFilled(beakerTopLeft, beakerBottomRight,
                            ImColor(m_currentReaction->resultColor));

    ImVec2 textSize = ImGui::CalcTextSize(m_currentReaction->result.c_str());
    ImVec2 textPos = ImVec2(beakerTopLeft.x + (beakerSize.x - textSize.x) / 2,
                            beakerTopLeft.y + (beakerSize.y - textSize.y) / 2);
    drawList->AddText(textPos, ImColor(255, 255, 255),
                      m_currentReaction->result.c_str());
  } else {
    // Caso contrário, preenche com os elementos misturados
    float elementHeight = beakerSize.y / m_selectedElements.size();
    for (size_t i = 0; i < m_selectedElements.size(); ++i) {
      Element &element = m_selectedElements[i];
      ImVec2 elementPos =
          ImVec2(beakerTopLeft.x, beakerTopLeft.y + i * elementHeight);
      ImVec2 elementSize = ImVec2(beakerSize.x, elementHeight);
      drawList->AddRectFilled(
          elementPos,
          ImVec2(elementPos.x + elementSize.x, elementPos.y + elementSize.y),
          ImColor(element.color));
    }
  }
}

void Window::checkReaction() {
  if (m_selectedElements.size() == 2) {
    auto it = reactions.find(
        {m_selectedElements[0].symbol, m_selectedElements[1].symbol});
    if (it != reactions.end()) {
      m_currentReaction = it->second;
    } else {
      m_currentReaction.reset(); // Caso não haja reação
    }
  }
}
