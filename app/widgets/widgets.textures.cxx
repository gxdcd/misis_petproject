module;

#pragma region Header
#include <glfw_impl.hxx>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize2.h>
export module widgets:textures;
import themes;
using namespace std;
#pragma endregion

// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
bool LoadTextureFromFile1(const char* filename, GLuint* out_texture, int* out_width, int* out_height, optional<float> scale)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);

    // scale image
    if (scale) {
        auto new_width = static_cast<int>(image_width * *scale);
        auto new_height = static_cast<int>(image_height * *scale);
        auto new_image_data = new unsigned char[new_width * new_height * 4];
        // stbir_resize_uint8(image_data, image_width, image_height, 0, new_image_data, new_width, new_height, 0, 4);
        stbir_resize_uint8_linear(image_data, image_width, image_height, 0, new_image_data, new_width, new_height, 0, (stbir_pixel_layout)4);
        stbi_image_free(image_data);
        image_data = new_image_data;
        image_width = new_width;
        image_height = new_height;
    }

    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

export struct Texture {
private:
    GLuint texture{};
    int width{};
    int height{};

public:
    Texture() = default;

    Texture(const char* filename, optional<float> scale)
    {
        LoadTextureFromFile1(filename, &texture, &width, &height, scale);
    }

    Texture(GLuint texture, int width, int height):
        texture(texture), width(width), height(height)
    {
    }

    // copy constructor and assignment operator
    // delete them unless we use reference counting
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // move constructor and assignment operator
    Texture(Texture&& other) noexcept
        :
        texture(other.texture),
        width(other.width), height(other.height)
    {
        other.texture = 0;
    }

    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other) {
            release();
            texture = other.texture;
            width = other.width;
            height = other.height;
            other.texture = 0;
        }
        return *this;
    }

    ~Texture()
    {
        // https://stackoverflow.com/questions/8350077/do-gl-textures-need-to-be-released-freed
        release();
    }

    void release()
    {
        if (texture != 0) {
            glDeleteTextures(1, &texture);
            texture = 0;
        }
    }

    auto get_texture() const
    {
        return texture;
    }

    auto get_texture_id() const
    {
        return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(get_texture()));
    }

    auto get_size() const
    {
        return ImVec2{float(width), float(height)};
    }
};

export struct ThemedTexture {
    ThemedTexture(const char* filename, optional<float> scale)
    {
        auto dark_file_path = format(R"(theme\dark\{})", filename);
        auto light_file_path = format(R"(theme\light\{})", filename);
        light_theme = Texture(light_file_path.c_str(), scale);
        dark_theme = Texture(dark_file_path.c_str(), scale);
    }

    auto get_texture() const
    {
        return ImGui::Utils::IsThemeDark() ? dark_theme.get_texture() : light_theme.get_texture();
    }

    auto get_texture_id() const
    {
        return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(get_texture()));
    }

private:
    Texture light_theme;
    Texture dark_theme;
};
