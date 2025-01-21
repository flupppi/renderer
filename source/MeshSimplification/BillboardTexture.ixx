module;
#include <GL/glew.h>

export module BillboardTexture;

import std;
namespace Engine {
    // Texture structure
    export struct BillboardTexture {
        unsigned int id;                // GPU texture ID.
        std::string type;               // Type (e.g., "billboard").
        std::string path;               // Path to texture file (optional for dynamically created textures).
        unsigned int width;             // Texture width in pixels.
        unsigned int height;            // Texture height in pixels.
        bool hasAlpha;                  // Indicates if the texture has an alpha channel.

        // Create texture dynamically from framebuffer
        void createFromFramebuffer(unsigned int width, unsigned int height) {
            this->width = width;
            this->height = height;
            this->hasAlpha = true;

            // OpenGL calls to create texture from the framebuffer
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };
}