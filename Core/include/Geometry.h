#pragma once

#include <Node.h>
#include <vector>
#include <Vertex.h>

class Geometry : public Node
{
public:
    Geometry() {}

    std::vector<Vertex> vertices;
    std::vector<glm::u32vec3> indices;
};