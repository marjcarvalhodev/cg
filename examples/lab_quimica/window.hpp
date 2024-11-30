#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include <array>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
  glm::vec3 position{};

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

// Hash personalizado para pares de elementos
struct pair_hash
{
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const
  {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);
    return h1 ^ h2; // Combine os dois hashes
  }
};

struct Element
{
  std::string name;   // Nome do elemento (ex: "Água", "Sódio")
  std::string symbol; // Símbolo do elemento (ex: "H2O", "Na")
  ImVec4 color;       // Cor do elemento para visualização
};

struct Reaction
{
  std::string result; // Nome do resultado (ex: "NaCl")
  ImVec4 resultColor; // Cor do resultado para visualização
};

class Window : public abcg::OpenGLWindow
{
  // Camera variables
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;
  glm::vec3 cameraPos{0.0f, 0.0f, 3.0f};    // Camera position
  glm::vec3 cameraTarget{0.0f, 0.0f, 0.0f}; // Camera target (looking at)
  glm::vec3 cameraUp{0.0f, 1.0f, 0.0f};     // Up direction for camera
  float cameraSpeed = 2.5f;

protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;

private:
  // Armazenar os elementos químicos disponíveis
  std::vector<Element> m_elements;
  std::unordered_map<std::pair<std::string, std::string>, Reaction, pair_hash>
      reactions;

  // Elementos selecionados para mistura
  std::vector<Element> m_selectedElements;
  std::optional<Reaction> m_currentReaction; // Reação atual (se houver)

  // Funções de interação
  void addToBeaker(Element element);
  void checkReaction();
  void displayBeaker();
  void loadReactions();
  void loadElements();
  void add3DLabItem();

  // Tentando criar o loader de modelo 3D
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  float m_angle{};
  int m_verticesToDraw{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void loadModelFromFile(std::string_view path);
  void standardize();
  void setup3D();
  void draw3D();
  void snapCameraToModel();
  void processInput();
};

#endif
