#include "TriangleMesh.h"

TriangleMesh::TriangleMesh()
{
    vertex_count = 3;

    float verts[9] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    float colors[9] = {
         1.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 1.0f
    };
    createVAO(verts, colors);
}

TriangleMesh::TriangleMesh(float* verts, float* colors)
{
    vertex_count = 3;

    createVAO(verts, colors);
}
void TriangleMesh::createVAO(float* verts, float* colors)
{
    std::vector<float> data;
    for (int i = 0; i < 9; i++)
        data.push_back(verts[i]);
    for (int i = 0; i < 9; i++)
        data.push_back(colors[i]);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    int stride = 3 * sizeof(float);

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(1);

}

void TriangleMesh::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

TriangleMesh::~TriangleMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}