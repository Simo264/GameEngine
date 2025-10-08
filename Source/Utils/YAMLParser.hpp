#pragma once

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <glm/vec3.hpp>

namespace YAML
{
  template<>
  struct convert<glm::vec3>
  {
    static bool decode(const Node& node, glm::vec3& rhs)
    {
      if (!node.IsSequence() || node.size() != 3) return false;
      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
      rhs.z = node[2].as<float>();
      return true;
    }

    static Node encode(const glm::vec3& rhs)
    {
      auto node = Node{};
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      node.SetStyle(YAML::EmitterStyle::Flow);
      return node;
    }
  };
}
