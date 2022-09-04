#pragma once
#include <cstdint>
#include <vector>
class Model {
    public:

    Model();

    uint32_t VAO;
    uint32_t VBO;
    uint32_t EBO;
    bool isNormalized = false;

    void addAttributeVec3(uint32_t offset = 0);
    template <typename T>
    void bufferVertexArray(std::vector<T> verticies) { bufferVertexArray(verticies.data(), verticies.size(), sizeof(T)); }
    void bufferElementArray(std::vector<uint32_t> indicies);

    void drawArrays();
    void drawElements(uint32_t num_elements = 0, uint32_t offset = 0);
    void drawElementsAll(uint32_t offset = 0);

    private:
    void bind();
    void unbind();
    void gen();
    void bufferVertexArray(void* verticies, uint32_t vertex_array_size, uint32_t vertexSize);
    uint32_t currentAttributeIndex = 0;
    uint32_t sizeVertex;
    uint32_t sizeVertexArray;
    uint32_t sizeIndexArray;
};