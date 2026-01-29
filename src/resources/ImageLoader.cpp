#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#include <iostream>
#include <filesystem>

namespace Lengine {

    GLTexture ImageLoader::loadPNG(const std::string& filePath) {
        GLTexture texture = {};

        std::vector<unsigned char> in;
        std::vector<unsigned char> out;

        unsigned long width, height;

        if (!IOManager::readFileToBuffer(filePath, in)) {
            fatalError("Failed to load PNG file to buffer! ");
        };
        int errorCode = decodePNG(out, width, height, &(in[0]), in.size());

        if (errorCode) {
            fatalError("decodePNG failed with error: " + std::to_string(errorCode));
        }

        glGenTextures(1, &(texture.id));

        glBindTexture(GL_TEXTURE_2D, texture.id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(out[0]));

       
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        texture.width = width;
        texture.height = height;

        return texture;
    }

    std::shared_ptr<ImageData> ImageLoader::stbiLoader(const std::string& filePath)
    {
        std::filesystem::path path = filePath;
        if (!std::filesystem::exists(path)) {
            std::cout << "File does not exist: " << path << std::endl;
            return nullptr;
        }

        auto img = std::make_shared<ImageData>();

        unsigned char* data = stbi_load(
            filePath.c_str(),
            &img->width,
            &img->height,
            &img->channels,
            0
        );

        if (!data) {
            std::cerr << "STB failed: " << stbi_failure_reason() << std::endl;
            return nullptr;
        }

        size_t size = static_cast<size_t>(img->width) *
            static_cast<size_t>(img->height) *
            static_cast<size_t>(img->channels);

        img->pixels.assign(data, data + size);

        stbi_image_free(data);
        return img;
    }

      /*
      void ImageLoader::uploadToGPU(const ImageData& img, bool srgb)
      {
          GLint id = img.id
          GLenum format =
              img.channels == 1 ? GL_RED :
              img.channels == 3 ? GL_RGB :
              GL_RGBA;

          GLenum internalFormat = srgb
              ? (format == GL_RGBA ? GL_SRGB_ALPHA : GL_SRGB)
              : format;

          glGenTextures(1, &id);
          glBindTexture(GL_TEXTURE_2D, id);

          glTexImage2D(
              GL_TEXTURE_2D, 0, internalFormat,
              img.width, img.height,
              0, format, GL_UNSIGNED_BYTE,
              img.pixels.data()
          );

          glGenerateMipmap(GL_TEXTURE_2D);

          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

          glBindTexture(GL_TEXTURE_2D, 0);

          width = img.width;
          height = img.height;
      }
      */

    GLTexture ImageLoader::loadTexture2D(
        const std::string& filePath,
        bool srgb
    ) {
        GLTexture texture = {};


        int width, height, channels;
        unsigned char* data = stbi_load(
            filePath.c_str(),
            &width,
            &height,
            &channels,
            0
        );

        if (!data) {
            fatalError("Failed to load texture: " + filePath);
        }

        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;

        GLenum internalFormat;
        if (srgb)
        {
            internalFormat = (format == GL_RGBA)
                ? GL_SRGB8_ALPHA8
                : GL_SRGB8;
        }
        else
        {
            internalFormat = (format == GL_RGBA)
                ? GL_RGBA8
                : GL_RGB8;
        }


        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internalFormat,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        texture.width = width;
        texture.height = height;

        return texture;
    }

    GLTexture ImageLoader::loadTextureCubemap(std::vector<std::string> faces)
    {


        GLTexture texture = {};
        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);


        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    format,
                    width,
                    height,
                    0,
                    format,
                    GL_UNSIGNED_BYTE,
                    data
                );

                stbi_image_free(data);
            }
            else
            {
                
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


        texture.width = width;
        texture.height = height;



        return texture;
    }

    GLTexture ImageLoader::loadHDRTextureCubemap(std::vector<std::string> faces)
    {
        GLTexture texture = {};
        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);

        int width = 0, height = 0, nrComponents = 0;

        stbi_set_flip_vertically_on_load(false); // IMPORTANT for cubemaps

        for (unsigned int i = 0; i < faces.size(); i++)
        {
            float* data = stbi_loadf(faces[i].c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;

                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    (format == GL_RGBA) ? GL_RGBA16F : GL_RGB16F,
                    width,
                    height,
                    0,
                    format,
                    GL_FLOAT,
                    data
                );

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Failed to load HDR cubemap face: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }

        // Cubemap sampling params
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        texture.width = width;
        texture.height = height;

        return texture;
    }


    GLTexture ImageLoader::LoadHDRTexture(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);
        GLTexture texture = {};
        int width, height, nrComponents;
        float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glGenTextures(1, &texture.id);
            glBindTexture(GL_TEXTURE_2D, texture.id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);

            std::cout << "Loaded HDR image: " << path << std::endl;

        }
        else
        {
            std::cout << "Failed to load HDR image: " << path << std::endl;
        }

        texture.width = width;
        texture.height = height;

        stbi_set_flip_vertically_on_load(false);

        return texture;
    }

     glm::vec2 SampleSphericalMap(const glm::vec3& v)
    {
        glm::vec2 uv;
        uv.x = atan2(v.z, v.x);
        uv.y = asin(v.y);

        uv *= glm::vec2(0.1591f, 0.3183f);
        uv += 0.5f;

        return uv;
    }


    glm::vec3 GetCubeDirection(int face, float u, float v)
    {
        // u, v in range [-1, 1]
        switch (face)
        {
        case 0: return glm::normalize(glm::vec3(1.0f, v, -u)); // +X
        case 1: return glm::normalize(glm::vec3(-1.0f, v, u)); // -X
        case 2: return glm::normalize(glm::vec3(u, 1.0f, -v)); // +Y
        case 3: return glm::normalize(glm::vec3(u, -1.0f, v)); // -Y
        case 4: return glm::normalize(glm::vec3(u, v, 1.0f)); // +Z
        case 5: return glm::normalize(glm::vec3(-u, v, -1.0f)); // -Z
        }
        return glm::vec3(0.0f);
    }
    
    void ImageLoader::ConvertEquirectangularToCubemapCPU(
        const float* hdrData,
        int hdrWidth,
        int hdrHeight,
        int faceSize,
        std::vector<float> cubemapData[6]
    )
    {
        for (int i = 0; i < 6; i++)
            cubemapData[i].resize(faceSize * faceSize * 3);

        for (int face = 0; face < 6; face++)
        {
            for (int y = 0; y < faceSize; y++)
            {
                for (int x = 0; x < faceSize; x++)
                {
                    float u = (2.0f * x / (faceSize - 1)) - 1.0f;
                    float v = (2.0f * y / (faceSize - 1)) - 1.0f;

                    glm::vec3 dir = GetCubeDirection(face, u, v);
                    glm::vec2 uv = SampleSphericalMap(dir);

                    int px = int(uv.x * hdrWidth);
                    int py = int(uv.y * hdrHeight);

                    px = glm::clamp(px, 0, hdrWidth - 1);
                    py = glm::clamp(py, 0, hdrHeight - 1);

                    int hdrIndex = (py * hdrWidth + px) * 3;
                    int cubeIndex = (y * faceSize + x) * 3;

                    cubemapData[face][cubeIndex + 0] = hdrData[hdrIndex + 0];
                    cubemapData[face][cubeIndex + 1] = hdrData[hdrIndex + 1];
                    cubemapData[face][cubeIndex + 2] = hdrData[hdrIndex + 2];
                }
            }
        }
    }


}