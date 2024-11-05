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
  for (auto &element : m_elements) {
    if (ImGui::Button(element.symbol.c_str(), ImVec2(100, 100))) {
      addToBeaker(element); // Adicionar elemento ao recipiente
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
