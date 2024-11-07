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

  // Exibir os elementos químicos disponíveis
  ImGui::Text("Elementos Químicos Disponíveis:");
  auto drawList = ImGui::GetWindowDrawList();

  float elementWidth = 100.0f;  // Largura do frasco
  float elementHeight = 100.0f; // Altura do frasco
  float spacing = 10.0f;        // Espaçamento entre frascos

  for (size_t i = 0; i < m_elements.size(); ++i) {
    Element &element = m_elements[i];
    ImVec2 pos = ImGui::GetCursorScreenPos();

    // Define o tamanho do "botão"
    ImVec2 size = ImVec2(elementWidth, elementHeight);

    // Adiciona um botão invisível para capturar cliques
    ImGui::InvisibleButton(("Erlenmeyer" + std::to_string(i)).c_str(), size);
    if (ImGui::IsItemClicked()) {
      addToBeaker(element);
    }

    // Calcula as coordenadas para um "frasco de Erlenmeyer" básico
    ImVec2 topCenter = ImVec2(pos.x + size.x / 2, pos.y);
    ImVec2 bottomLeft = ImVec2(pos.x, pos.y + size.y);
    ImVec2 bottomRight = ImVec2(pos.x + size.x, pos.y + size.y);
    ImVec2 neckLeft = ImVec2(pos.x + size.x * 0.4f, pos.y + size.y * 0.3f);
    ImVec2 neckRight = ImVec2(pos.x + size.x * 0.6f, pos.y + size.y * 0.3f);

    // Desenha o frasco em duas partes: corpo e pescoço
    drawList->AddTriangleFilled(topCenter, neckLeft, neckRight,
                                ImColor(element.color));
    drawList->AddQuadFilled(neckLeft, neckRight, bottomRight, bottomLeft,
                            ImColor(element.color));

    // Desenha o símbolo do elemento sobre o frasco
    ImVec2 textSize = ImGui::CalcTextSize(element.symbol.c_str());
    ImVec2 textPos = ImVec2(pos.x + (size.x - textSize.x) / 2,
                            pos.y + (size.y - textSize.y) / 2);
    drawList->AddText(textPos, ImColor(255, 255, 255), element.symbol.c_str());

    // Verifica se há espaço suficiente para colocar o próximo botão na mesma
    // linha
    float nextPosX = pos.x + elementWidth + spacing;
    if (nextPosX + elementWidth > ImGui::GetWindowPos().x + appWindowWidth) {
      ImGui::NewLine(); // Move para a próxima linha
    } else {
      ImGui::SameLine(); // Mantém na mesma linha
    }
  }

  ImGui::Separator();

  // Exibir recipiente de mistura e resultado da reação
  ImGui::Text("Recipiente de Mistura:");
  displayBeaker();

  // Botão para resetar a mistura
  if (ImGui::Button("Resetar Mistura")) {
    m_selectedElements.clear(); // Limpa os elementos selecionados
  }

  ImGui::End();
}

void Window::addToBeaker(Element element) {
  if (m_selectedElements.size() < 2) {
    m_selectedElements.push_back(element);
  }
}

void Window::displayBeaker() {
  // Exibir os elementos dentro do recipiente
  for (auto &element : m_selectedElements) {
    ImGui::Button(element.symbol.c_str(), ImVec2(100, 100));
  }

  // Se houver dois elementos, verifica a reação
  if (m_selectedElements.size() == 2) {
    checkReaction();
  }
}

void Window::checkReaction() {
  if (m_selectedElements.size() == 2) {
    auto &el1 = m_selectedElements[0];
    auto &el2 = m_selectedElements[1];

    // Verificar se há uma reação entre os dois elementos
    auto pair = std::make_pair(el1.symbol, el2.symbol);
    auto reaction = reactions.find(pair);

    if (reaction == reactions.end()) {
      // Se não houver reação, mostrar mensagem de erro
      ImGui::Text("Não há reação entre %s e %s.", el1.symbol.c_str(),
                  el2.symbol.c_str());
    } else {
      // Mostrar o resultado da reação
      ImGui::Text("Resultado: %s", reaction->second.result.c_str());
      ImGui::Text("Cor: ");
      ImGui::ColorButton("ResultColor", reaction->second.resultColor,
                         ImGuiColorEditFlags_NoAlpha);

      // Limpar os elementos após a reação
      m_selectedElements.clear();
    }
  }
}
