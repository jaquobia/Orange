#include "model.hpp"
#include <glbinding/gl/gl.h>

using namespace gl;
Model::Model() 
{
    gen();
    bind();
}
void Model::bind() 
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void Model::unbind()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::gen() 
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void Model::addAttributeVec3(uint32_t offset)
{
    glVertexAttribPointer(currentAttributeIndex, 3, GL_FLOAT, isNormalized ? GL_TRUE : GL_FALSE, sizeVertex, (void*)offset);
    glEnableVertexAttribArray(currentAttributeIndex++);
}
void Model::bufferVertexArray(void* verticies, uint32_t vertex_array_size, uint32_t vertexSize)
{
    sizeVertex = vertexSize;
    sizeVertexArray = vertex_array_size;
    glBufferData(GL_ARRAY_BUFFER, sizeVertex * sizeVertexArray, verticies, GL_STATIC_DRAW);
}
void Model::bufferElementArray(std::vector<uint32_t> indicies)
{
    sizeIndexArray = indicies.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * sizeIndexArray, indicies.data(), GL_STATIC_DRAW);
}
void Model::drawArrays()
{

}
void Model::drawElements(uint32_t num_elements, uint32_t offset)
{
    glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, (void*)offset);
}
void Model::drawElementsAll(uint32_t offset)
{
    glDrawElements(GL_TRIANGLES, sizeIndexArray, GL_UNSIGNED_INT, (void*)offset);
}