#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

class Skybox {
public:
    Skybox(glm::mat4 view, glm::mat4 projection)
        : view(view), projection(projection)
    {
        initVertices();
        initVAO();
        initProgram();
        initTexture();
    }
    ~Skybox()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(program);
    }
    void draw()
    {
        glm::mat4 adjustedView = glm::mat4(glm::mat3(view)); // Remove translation from the view matrix
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glUseProgram(program);
        
        unsigned int viewLoc = glGetUniformLocation(
            program, "view"
        );
        unsigned int projectionLoc = glGetUniformLocation(
            program, "projection"
        );
        unsigned int textureLoc = glGetUniformLocation(
            program, "skybox"
        );

        glUniformMatrix4fv(
            viewLoc, 1, GL_FALSE, glm::value_ptr(adjustedView)
        );
        glUniformMatrix4fv(
            projectionLoc, 1, GL_FALSE, 
            glm::value_ptr(projection)
        );
        glUniform1i(textureLoc, 0);

        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glUseProgram(0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }
private:
    void initVertices()
    {
        vertices = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };
    }
    void initVAO()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(float),
            &vertices[0],
            GL_STATIC_DRAW
        );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            3 * sizeof(float),
            (void*)0
        );
        glBindVertexArray(0);
    }
    void initProgram()
    {
        program = glCreateProgram();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        const char* vsSource = getFile(
            "shaders/skybox.vert"
        );
        const char* fsSource = getFile(
            "shaders/skybox.frag"
        );
        glShaderSource(vs, 1, &vsSource, NULL);
        glShaderSource(fs, 1, &fsSource, NULL);
        glCompileShader(vs);
        glCompileShader(fs);
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }
    void initTexture()
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        int imageWidth, imageHeight;
        unsigned char* image = stbi_load(
            "obj/skybox_texture.jpg",
            &imageWidth, &imageHeight,
            NULL, STBI_rgb
        );
        int sideLength = imageWidth / 4;
        
        // Load the texture sections
        loadTextureSection(
            "obj/skybox_texture.jpg",
            sideLength * 2, sideLength,
            sideLength, sideLength,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X
        );
        loadTextureSection(
            "obj/skybox_texture.jpg",
            0, sideLength,
            sideLength, sideLength,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        );
        loadTextureSection(
            "obj/skybox_texture.jpg",
            sideLength, 0,
            sideLength, sideLength,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y
        );
        loadTextureSection(
            "obj/skybox_texture.jpg",
            sideLength, sideLength * 2,
            sideLength, sideLength,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        );
        loadTextureSection(
            "obj/skybox_texture.jpg",
            sideLength, sideLength,
            sideLength, sideLength,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        );
        loadTextureSection(
            "obj/skybox_texture.jpg",
            sideLength * 3, sideLength,
            sideLength, sideLength,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
        );

        glTexParameteri(
            GL_TEXTURE_CUBE_MAP, 
            GL_TEXTURE_MIN_FILTER, 
            GL_LINEAR
        );
        glTexParameteri(
            GL_TEXTURE_CUBE_MAP, 
            GL_TEXTURE_MAG_FILTER, 
            GL_LINEAR
        );
        glTexParameteri(
            GL_TEXTURE_CUBE_MAP, 
            GL_TEXTURE_WRAP_S, 
            GL_CLAMP_TO_EDGE
        );
        glTexParameteri(
            GL_TEXTURE_CUBE_MAP, 
            GL_TEXTURE_WRAP_T, 
            GL_CLAMP_TO_EDGE
        );
        glTexParameteri(
            GL_TEXTURE_CUBE_MAP, 
            GL_TEXTURE_WRAP_R, 
            GL_CLAMP_TO_EDGE
        );
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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
    GLuint loadTextureSection(
        const char* path,
        int xoffset, int yoffset, 
        int width, int height,
        int face
    )
    {
        int imgWidth, imgHeight, channels;
        stbi_set_flip_vertically_on_load(false); // Typically, you don't want to flip the skybox textures
        unsigned char* fullData = stbi_load(path, &imgWidth, &imgHeight, &channels, 0);
        if (!fullData) {
            std::cerr << "Cubemap texture failed to load at path: " << path << std::endl;
            return 0;
        }

        // Calculate the size of the bytes per pixel (assuming 8 bits per channel)
        int bytesPerPixel = channels;
        unsigned char* sectionData = new unsigned char[width * height * bytesPerPixel];

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int fullDataIndex = ((yoffset + y) * imgWidth + (xoffset + x)) * bytesPerPixel;
                int sectionDataIndex = (y * width + x) * bytesPerPixel;
                memcpy(sectionData + sectionDataIndex, fullData + fullDataIndex, bytesPerPixel);
            }
        }
        stbi_image_free(fullData);

        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glTexImage2D(face, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, sectionData);
        delete[] sectionData;
    }

    std::vector<float> vertices;
    GLuint vao;
    GLuint vbo;
    GLuint program;
    GLuint texture;
    glm::mat4 view;
    glm::mat4 projection;
};

#endif