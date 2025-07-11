#pragma once

#include <GL/glew.h>
#include <stb_image.h>
#include <string>

#include "debug.h"

struct texture
{
    int width, height, bytes_per_pixel;
    std::string filepath;

    texture(const std::string& path)
        : renderer_id(0), filepath(path), local_buffer(nullptr), width(0), height(0), bytes_per_pixel(0)
    {
        stbi_set_flip_vertically_on_load(1);
        local_buffer = stbi_load(path.c_str(), &width, &height, &bytes_per_pixel, 4);

        CALL(glGenTextures(1, &renderer_id));
        // new slot of the opengl state: GL_TEXTURE_2D
        CALL(glBindTexture(GL_TEXTURE_2D, renderer_id));
        
        // these parameters are settings for the texture; these 4 are required unless texture won't be properly shown
        // min and mag filters specify how the gpu will scale the texture on smaller and bigger surfaces than the texture itself respectively
        // wrap s and wrap t specify how gpu will handle if texture extends horizontally or vertically respectively
        CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        

        if (local_buffer)
        {
            // here we provide the pixel data for the texture to the gpu
            // internal format, GL_RGBA8 in this case where 8 means bits per channel, is the format in which gpu will store this texture data
            // the other format, GL_RGBA in this case, is the format in which our local_buffer is stored in
            CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, local_buffer));
            
            CALL(glBindTexture(GL_TEXTURE_2D, 0));
    
            // freeing the local_buffer; we can also keep this local_buffer if we want to do something else
            stbi_image_free(local_buffer);
        }
        else
        {
            std::cout << "\nFailed to load texture" << std::endl;
            std::cout << stbi_failure_reason() << std::endl;
            ASSERT(0);
        }
    }

    ~texture()
    {
        CALL(glDeleteTextures(1, &renderer_id));
    }

    void bind(unsigned int slot = 0) const
    {
        CALL(glActiveTexture(GL_TEXTURE0 + slot));
        CALL(glBindTexture(GL_TEXTURE_2D, renderer_id));
    }

    void unbind() const
    {
        CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

private:
    unsigned int renderer_id;
    unsigned char* local_buffer;
};