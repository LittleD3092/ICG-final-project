#ifndef GRASS_PLANE_H
#define GRASS_PLANE_H

#include <glad/glad.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

class GrassPlane
{
public:
    GrassPlane(glm::mat4 view, glm::mat4 projection)
        : view(view), projection(projection)
    {
        initVertices();
        initVAO();
        initProgram();
        initTexture();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-width / 2, 0, -depth / 2));
    }
    ~GrassPlane()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(program);
    }
    void draw(glm::vec3 windshift)
    {
        drawPlane();
        drawBlade(windshift);
    }
private:
    void drawBlade(glm::vec3 windshift)
    {
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glUseProgram(bladeProgram);

        unsigned int viewLoc = glGetUniformLocation(
            bladeProgram, "view"
        );
        unsigned int projectionLoc = glGetUniformLocation(
            bladeProgram, "projection"
        );
        unsigned int modelLoc = glGetUniformLocation(
            bladeProgram, "model"
        );
        unsigned int textureLoc = glGetUniformLocation(
            bladeProgram, "grass_blade"
        );
        unsigned int windshiftLoc = glGetUniformLocation(
            bladeProgram, "windshift"
        );

        // printMatrices();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(textureLoc, 0);
        glUniform3fv(windshiftLoc, 1, glm::value_ptr(windshift));

        glBindVertexArray(bladeVao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bladeTexture);

        glDrawArrays(GL_POINTS, 0, bladeVertices.size()/3);
        glBindVertexArray(0);

        glUseProgram(0);
    }
    void drawPlane()
    {
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glUseProgram(program);

        unsigned int viewLoc = glGetUniformLocation(
            program, "view"
        );
        unsigned int projectionLoc = glGetUniformLocation(
            program, "projection"
        );
        unsigned int modelLoc = glGetUniformLocation(
            program, "model"
        );
        unsigned int textureLoc = glGetUniformLocation(
            program, "grass"
        );

        // printMatrices();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(textureLoc, 0);

        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);
        glBindVertexArray(0);

        glUseProgram(0);
    }
    void initVertices()
    {
        width = 100;
        depth = 5;
        density = 7;
        for(float x = 0; x < width; x += 1/density)
        {
            for(float z = 0; z < depth; z += 1/density)
            {
                // first triangle
                vertices.push_back(x);
                vertices.push_back(0);
                vertices.push_back(z);

                vertices.push_back(x + 1/density);
                vertices.push_back(0);
                vertices.push_back(z);

                vertices.push_back(x);
                vertices.push_back(0);
                vertices.push_back(z + 1/density);

                // second triangle
                vertices.push_back(x);
                vertices.push_back(0);
                vertices.push_back(z + 1/density);

                vertices.push_back(x + 1/density);
                vertices.push_back(0);
                vertices.push_back(z);

                vertices.push_back(x + 1/density);
                vertices.push_back(0);
                vertices.push_back(z + 1/density);

                // blade
                bladeVertices.push_back(x);
                bladeVertices.push_back(0);
                bladeVertices.push_back(z);

                // blade texture
                bladeTexCoords.push_back(x / width);
                bladeTexCoords.push_back(z / depth);

                // blade length
                bladeLengths.push_back(0.25 + float(rand() % 50) / 100.0);
            }
        }
    }

    void initVAO()
    {
        initPlaneVAO();
        initBladeVAO();
    }

    void initPlaneVAO()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(
            GL_ARRAY_BUFFER, 
            vertices.size() * sizeof(float),
            &vertices[0], GL_STATIC_DRAW
        );

        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 
            3 * sizeof(float), (void*)0
        );
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void initBladeVAO()
    {
        glGenVertexArrays(1, &bladeVao);
        glBindVertexArray(bladeVao);
        glGenBuffers(3, bladeVbo);

        glBindBuffer(GL_ARRAY_BUFFER, bladeVbo[0]);
        glBufferData(
            GL_ARRAY_BUFFER, 
            bladeVertices.size() * sizeof(float),
            &bladeVertices[0], GL_STATIC_DRAW
        );
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 
            3 * sizeof(float), (void*)0
        );
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, bladeVbo[1]);
        glBufferData(
            GL_ARRAY_BUFFER, 
            bladeTexCoords.size() * sizeof(float),
            &bladeTexCoords[0], GL_STATIC_DRAW
        );
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 
            2 * sizeof(float), (void*)0
        );
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, bladeVbo[2]);
        glBufferData(
            GL_ARRAY_BUFFER, 
            bladeLengths.size() * sizeof(float),
            &bladeLengths[0], GL_STATIC_DRAW
        );
        glVertexAttribPointer(
            2, 1, GL_FLOAT, GL_FALSE, 
            0, (void*)0
        );
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    void initProgram()
    {
        initPlaneProgram();
        initBladeProgram();
    }

    void initPlaneProgram()
    {
        program = glCreateProgram();
        GLuint vs = glCreateShader(
            GL_VERTEX_SHADER
        );
        GLuint fs = glCreateShader(
            GL_FRAGMENT_SHADER
        );
        const char* vsSource = getFile(
            "shaders/grass_plane.vert"
        );
        const char* fsSource = getFile(
            "shaders/grass_plane.frag"
        );
        glShaderSource(vs, 1, &vsSource, NULL);
        glShaderSource(fs, 1, &fsSource, NULL);
        glCompileShader(vs);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vs, 512, NULL, infoLog);
            std::cout << 
                "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << 
                infoLog << std::endl;
        }

        glCompileShader(fs);
        // check for shader compile errors
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fs, 512, NULL, infoLog);
            std::cout << 
                "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << 
                infoLog << std::endl;
        }

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        // check for linking errors
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cout << 
                "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << 
                infoLog << std::endl;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
        free((void*)vsSource);
        free((void*)fsSource);
    }

    void initBladeProgram()
    {
        bladeProgram = glCreateProgram();
        GLuint vs = glCreateShader(
            GL_VERTEX_SHADER
        );
        GLuint gs = glCreateShader(
            GL_GEOMETRY_SHADER
        );
        GLuint fs = glCreateShader(
            GL_FRAGMENT_SHADER
        );
        const char* vsSource = getFile(
            "shaders/grass_blade.vert"
        );
        const char* gsSource = getFile(
            "shaders/grass_blade.geom"
        );
        const char* fsSource = getFile(
            "shaders/grass_blade.frag"
        );
        glShaderSource(vs, 1, &vsSource, NULL);
        glShaderSource(gs, 1, &gsSource, NULL);
        glShaderSource(fs, 1, &fsSource, NULL);
        glCompileShader(vs);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vs, 512, NULL, infoLog);
            std::cout << 
                "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << 
                infoLog << std::endl;
        }

        glCompileShader(gs);
        // check for shader compile errors
        glGetShaderiv(gs, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(gs, 512, NULL, infoLog);
            std::cout << 
                "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << 
                infoLog << std::endl;
        }

        glCompileShader(fs);
        // check for shader compile errors
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fs, 512, NULL, infoLog);
            std::cout << 
                "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << 
                infoLog << std::endl;
        }

        glAttachShader(bladeProgram, vs);
        glAttachShader(bladeProgram, gs);
        glAttachShader(bladeProgram, fs);
        glLinkProgram(bladeProgram);
        // check for linking errors
        glGetProgramiv(bladeProgram, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(bladeProgram, 512, NULL, infoLog);
            std::cout << 
                "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << 
                infoLog << std::endl;
        }

        glDeleteShader(vs);
        glDeleteShader(gs);
        glDeleteShader(fs);
        free((void*)vsSource);
        free((void*)gsSource);
        free((void*)fsSource);
    }

    void initTexture()
    {
        initPlaneTexture();
        initBladeTexture();
    }

    void initPlaneTexture()
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        int imgWidth, imgHeight, nrChannels;
        unsigned char* data = stbi_load(
            "obj/grass.jpg",
            &imgWidth, &imgHeight, 
            &nrChannels, 0
        );
        if(data)
        {
            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGB, 
                imgWidth, imgHeight, 0, 
                GL_RGB, GL_UNSIGNED_BYTE, 
                data
            );
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            std::cout << 
                "Failed to load texture." << 
                std::endl;
        }
        stbi_image_free(data);
    }

    void initBladeTexture()
    {
        glGenTextures(1, &bladeTexture);
        glBindTexture(GL_TEXTURE_2D, bladeTexture);

        int imgWidth, imgHeight, nrChannels;
        unsigned char* data = stbi_load(
            "obj/grass_blades.jpg",
            &imgWidth, &imgHeight, 
            &nrChannels, 0
        );
        if(data)
        {
            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGB, 
                imgWidth, imgHeight, 0, 
                GL_RGB, GL_UNSIGNED_BYTE, 
                data
            );
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            std::cout << 
                "Failed to load texture." << 
                std::endl;
        }
        stbi_image_free(data);
    }

    const char* getFile(const char* path)
    {
        FILE* fp = fopen(path, "rb");
        if (fp == NULL) {
            std::cout << "Failed to open file: " << path << std::endl;
            perror("Error"); // Print the error message from the system
            return NULL;
        }
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);
        char* buffer = (char*)malloc(sizeof(char) * (size + 1));
        if(buffer == NULL) {
            std::cout << "Failed to allocate memory for file: " << path << std::endl;
            return NULL;
        }
        
        size_t result = fread(buffer, 1, size, fp);
        if (result != size) {
            if (feof(fp)) {
                std::cout << "Unexpected end of file." << std::endl;
            }
            if (ferror(fp)) {
                std::cout << "Error reading the file." << std::endl;
            }
            free(buffer);
            fclose(fp);
            return NULL;
        }
        buffer[size] = '\0';
        fclose(fp);
        return buffer;
    }

    void printMatrices()
    {
        std::cout << "view: " << view[0][0] << " " << view[0][1] << " " << view[0][2] << " " << view[0][3] << std::endl;
        std::cout << "      " << view[1][0] << " " << view[1][1] << " " << view[1][2] << " " << view[1][3] << std::endl;
        std::cout << "      " << view[2][0] << " " << view[2][1] << " " << view[2][2] << " " << view[2][3] << std::endl;
        std::cout << "      " << view[3][0] << " " << view[3][1] << " " << view[3][2] << " " << view[3][3] << std::endl;
        std::cout << "projection: " << projection[0][0] << " " << projection[0][1] << " " << projection[0][2] << " " << projection[0][3] << std::endl;
        std::cout << "            " << projection[1][0] << " " << projection[1][1] << " " << projection[1][2] << " " << projection[1][3] << std::endl;
        std::cout << "            " << projection[2][0] << " " << projection[2][1] << " " << projection[2][2] << " " << projection[2][3] << std::endl;
        std::cout << "            " << projection[3][0] << " " << projection[3][1] << " " << projection[3][2] << " " << projection[3][3] << std::endl;
        std::cout << "model: " << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << std::endl;
        std::cout << "       " << model[1][0] << " " << model[1][1] << " " << model[1][2] << " " << model[1][3] << std::endl;
        std::cout << "       " << model[2][0] << " " << model[2][1] << " " << model[2][2] << " " << model[2][3] << std::endl;
        std::cout << "       " << model[3][0] << " " << model[3][1] << " " << model[3][2] << " " << model[3][3] << std::endl;
    }

    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 model;

    // vertices
    std::vector<float> vertices;
    std::vector<float> bladeVertices;
    std::vector<float> bladeTexCoords;
    std::vector<float> bladeLengths;
    int width;
    int depth;
    float density;

    // vertex array object
    unsigned int vao;
    unsigned int bladeVao;
    // vertex buffer object
    unsigned int vbo;
    unsigned int bladeVbo[3];

    // program
    unsigned int program;
    unsigned int bladeProgram;

    // texture
    unsigned int texture;
    unsigned int bladeTexture;
};

#endif