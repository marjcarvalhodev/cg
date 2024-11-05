#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include <array>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Hash personalizado para pares de elementos
struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);
    return h1 ^ h2; // Combine os dois hashes
  }
};

struct Element {
  std::string name;   // Nome do elemento (ex: "Água", "Sódio")
  std::string symbol; // Símbolo do elemento (ex: "H2O", "Na")
  ImVec4 color;       // Cor do elemento para visualização
};

struct Reaction {
  std::string result; // Nome do resultado (ex: "NaCl")
  ImVec4 resultColor; // Cor do resultado para visualização
};

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaintUI() override;

private:
  static const int m_N = 3; // Tamanho do tabuleiro (para futuro uso)

  // Armazenar os elementos químicos disponíveis
  std::vector<Element> m_elements;
  std::unordered_map<std::pair<std::string, std::string>, Reaction, pair_hash>
      reactions;

  // Elementos selecionados para mistura
  std::vector<Element> m_selectedElements;

  // Funções de interação
  void addToBeaker(Element element);
  void checkReaction();
  void displayBeaker();
  void loadReactions();
  void loadElements();
};

#endif
