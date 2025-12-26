#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

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

      ImageData ImageLoader::stbiLoader(const std::string& filePath)
        {
            ImageData img;

            unsigned char* data = stbi_load(
                filePath.c_str(),
                &img.width,
                &img.height,
                &img.channels,
                0
            );

            if (!data)
                fatalError("Failed to load image: " + filePath);

            size_t size = img.width * img.height * img.channels;
            img.pixels.assign(data, data + size);

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

        GLenum internalFormat = srgb
            ? (format == GL_RGBA ? GL_SRGB_ALPHA : GL_SRGB)
            : format;

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

}