module;

#pragma region Header
#include <glfw_impl.hxx>
#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
module view.tiles:pdf;
import widgets;
import logging;
using namespace std;
#pragma endregion

auto poppler_to_gl_format(poppler::image::format_enum format)
{
    switch (format) {
        case poppler::image::format_enum::format_mono:
            // https://stackoverflow.com/questions/680125/can-i-use-a-grayscale-image-with-the-opengl-glteximage2d-function
            return GL_LUMINANCE; // grayscale
        case poppler::image::format_enum::format_rgb24:
            return GL_RGB; // RGB
        case poppler::image::format_enum::format_argb32:
            return GL_RGBA; // ARGB
        default:
            return 0; // Unknown format
    }
}

expected<Texture, string> image_to_gl_texture(const poppler::image& img)
{
    if (!img.is_valid())
        return unexpected("Page image is invalid.");

    // Create a OpenGL texture identifier
    GLuint image_texture{};
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

    auto gl_format = poppler_to_gl_format(img.format());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        img.width(), img.height(), 0, gl_format, GL_UNSIGNED_BYTE, img.const_data());

    if (auto e = glGetError(); e != GL_NO_ERROR)
        return unexpected(format("Failed to load image with GL error: {}", e));

    return Texture{image_texture, img.width(), img.height()};
}

expected<Texture, string> render_pdf_page(poppler::document* doc, int index)
{
    if (!doc)
        return unexpected("Unexpected null document reference");

    std::unique_ptr<poppler::page, decltype([](auto* obj) { delete obj; })> page{doc->create_page(index)};
    if (!page)
        return unexpected("Failed to get the page from the PDF document");

    poppler::page_renderer renderer;
    return image_to_gl_texture(renderer.render_page(page.get()));
}

expected<Texture, string> load_pdf(const string& path)
{
    logging::debug("Loading PDF: {}", path);

    using deleter = decltype([](auto* obj) { delete obj; });
    std::unique_ptr<poppler::document, deleter> doc{poppler::document::load_from_file(path)};
    if (!doc) {
        logging::error("Failed to load PDF file: {}", path);
        return unexpected(format("Failed to load PDF file: {}", path));
    }

    logging::debug("Loaded PDF file: {}", path);
    logging::debug("PDF page count: {}", doc->pages());

    return render_pdf_page(doc.get(), 0);
}

using deleter = decltype([](auto* obj) { delete obj; });
using document_ptr = unique_ptr<poppler::document, deleter>;
using page_ptr = unique_ptr<poppler::page, deleter>;

template<class... Ts>
struct overloaded: Ts... {
    using Ts::operator()...;
};

struct PDF {
private:
    // making illegal states unrepresentable
    variant<document_ptr, nullptr_t> doc{};
    PDF(document_ptr doc):
        doc{std::move(doc)} { }

public:
    PDF() noexcept = default;
    PDF(const PDF&) = delete;
    PDF& operator=(const PDF&) = delete;
    PDF& operator=(PDF&& other) noexcept = default;
    PDF(PDF&& other) noexcept = default;
    ~PDF() { }

    int is_valid() const
    {
        return visit(
            overloaded{
                [](const document_ptr& doc) { return bool(doc); },
                [](nullptr_t) { return false; },
            },
            doc);
    }

    int page_count() const
    {
        return visit(
            overloaded{
                [](const document_ptr& doc) { return doc->pages(); },
                [](nullptr_t) { return 0; },
            },
            doc);
    }

    expected<Texture, string> render_page(int index)
    {
        using result_type = expected<Texture, string>;
        return visit(
            overloaded{
                [=](const document_ptr& doc) {
                    page_ptr page{doc->create_page(index)};
                    if (!page)
                        return result_type{unexpected("Failed to get the page from the PDF document")};
                    poppler::page_renderer renderer;
                    return image_to_gl_texture(renderer.render_page(page.get()));
                },
                [](nullptr_t) { return result_type{unexpected("No valid PDF document present")}; },
            },
            doc);
    }

    // using external function due to unfair `error C2028: struct/union member must be inside a struct/union`
    // https://developercommunity.visualstudio.com/t/C-modules---compiler-error---C2028:-st/1510391
    friend expected<PDF, string> try_load(const string& path);
};

expected<PDF, string> try_load(const string& path)
{
    document_ptr doc{poppler::document::load_from_file(path)};
    if (!doc) {
        logging::error("Failed to load PDF file: {}", path);
        return unexpected(format("Failed to load PDF file: {}", path));
    }
    return expected<PDF, string>{PDF{std::move(doc)}};
}