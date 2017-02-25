#include "graphics.h"

#include <array>
#include <exception>

#include <glm/gtc/matrix_transform.hpp>

#include "common.h"
#include "content.h"
#include "game.h"
#include "glad/glad.h"
#include "stb/image.h"

using namespace std;

FT_Library Freetype = 0;

Font
DEBUG_LoadFont(std::string filename, int pxSize, Shader s)
{
    if (!Freetype) {
        auto err = FT_Init_FreeType(&Freetype);
        if (err) {
            throw err;
        }
    }

    FT_Face face;

    auto err = FT_New_Face(Freetype, filename.c_str(), 0, &face);
    if (err) {
        throw err;
    }

    err = FT_Set_Pixel_Sizes(face, 0, pxSize);
    if (err) {
        throw err;
    }

    std::map<uint64_t, Character> points{};
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            Log("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                     face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            new Texture{ int(face->glyph->bitmap.width),
                    int(face->glyph->bitmap.rows), std::vector<uint8_t>{},
                    texture },
            { face->glyph->bitmap_left, face->glyph->bitmap_top },
            { face->glyph->advance.x, face->glyph->advance.y }
        };
        points.insert(std::pair<GLchar, Character>(c, character));
    }

    FT_Done_Face(face);

    return Font(std::move(points), s);
}

const std::array<const vec2, 4> uvFullImage{ {
  {
    0.0f, 1.0f,
  }, // Bottom left
  {
    1.0f, 1.0f,
  }, // Bottom right
  {
    0.0f, 0.0f,
  }, // Top Left
  {
    1.0f, 0.0f,
  }, // Top right
} };

void
Font::RenderText(std::string text, vec2 pos, float scale, vec4 color)
{
    _shader.Apply();

    for (auto c : text) {
        auto ch = _codepoints[c];

        vec2 chPos = { pos.x + scale * ch.Offset.x,
                       pos.y - scale * (ch.Texture->Height - ch.Offset.y) };

        vec2 wh = { ch.Texture->Width * scale, ch.Texture->Height * scale };

        glm::mat3 screenToHUD =
          Translate({ -1, -1 }) * Scale({ 2 / 1920.0f, 2 / 1080.0f }) *
          Translate(chPos) * Translate(wh / 2) * Scale(wh); /**/

        glUniform3f(glGetUniformLocation(_shader._program, "textColor"),
                    color.r, color.g, color.b);

        DEBUG_DrawTexture(ch.Texture, screenToHUD,
                         FullImage, Colors::White);
        pos.x += ch.Advance.x / 64 * scale;
    }
}

Texture
DEBUG_LoadTexture(std::string filename)
{
    Log("loading texture " + filename);
    Texture s;
    uint8_t* mem = stbi_load(filename.c_str(), &s.Width, &s.Height, nullptr, 4);
    if (!mem)
        throw std::exception("file load failed");
    s.Memory.assign(mem, mem + s.Width * s.Height * 4);

    glGenTextures(1, &s.TextureID);
    glBindTexture(GL_TEXTURE_2D, s.TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s.Width, s.Height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, s.Memory.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return s;
}

GLuint vertexArrayID;
GLuint vertexBuffers[2];
GLuint& locationBuffer = vertexBuffers[0];
GLuint& uvBuffer = vertexBuffers[1];

vector<vec3> locationBufferData{
    {
      -0.5f, -0.5f, 1.0f,
    }, // Bottom left
    {
      0.5f, -0.5f, 1.0f,
    }, // Bottom right
    {
      -0.5f, 0.5f, 1.0f,
    }, // top left
    {
      0.5f, 0.5f, 1.0f,
    }, // top right
};

vector<vec2> uvBufferData{
    {
      0.0f, 1.0f,
    },
    {
      1.0f, 1.0f,
    },
    {
      0.0f, 0.0f,
    },
    {
      1.0f, 0.0f,
    },
};

internal_function void
initializeBuffers()
{
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(2, vertexBuffers);

    glBindBuffer(GL_ARRAY_BUFFER, locationBuffer);
    // glBufferData(/*
    //    GL_ARRAY_BUFFER,
    //    sizeof_vector(locationBufferData),
    //    locationBufferData.data(),
    //    GL_STATIC_DRAW);*/

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof_vector(uvBufferData),
                 uvBufferData.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

internal_function GLuint
getUniformLocation(std::string name)
{
    GLuint prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&prog);

    return glGetUniformLocation(prog, name.c_str());
}

void
SetUniform(std::string name, const glm::vec4& color)
{
    auto loc = getUniformLocation(name);
    glUniform4f(loc, color.r, color.g, color.b, color.a);
}

void
SetUniform(std::string name, const glm::mat3& value)
{
    auto loc = getUniformLocation(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, (GLfloat*)&value);
}

void
SetUniform(std::string name, const vec4& value)
{
    auto loc = getUniformLocation(name);
    glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void
DEBUG_DrawTexture(const Texture * tex, glm::mat3 projection, Rectangle texPart,
                 vec4 color)
{
    if (locationBuffer == 0 || uvBuffer == 0) {
        initializeBuffers();
    }

    uvBufferData[0].x = texPart.Min().x;
    uvBufferData[0].y = texPart.Max().y;
    uvBufferData[1].x = texPart.Max().x;
    uvBufferData[1].y = texPart.Max().y;
    uvBufferData[2].x = texPart.Min().x;
    uvBufferData[2].y = texPart.Min().y;
    uvBufferData[3].x = texPart.Max().x;
    uvBufferData[3].y = texPart.Min().y;

    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof_vector(uvBufferData),
                 uvBufferData.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, locationBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof_vector(locationBufferData),
                 locationBufferData.data(), GL_DYNAMIC_DRAW);

    glBindTexture(GL_TEXTURE_2D, tex->TextureID);

    GLint shader;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shader);
    glUniformMatrix3fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
                       reinterpret_cast<GLfloat*>(&projection));
    glUniform4f(glGetUniformLocation(shader, "color"), color.r, color.g,
                color.b, color.a);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void
OrthoView::DrawTexturePart(const Texture * tex, vec2 pos, Rectangle texPart, vec2 scale,
                          float rotation, vec4 color)
{
    auto projection =
      Matrix() * Translate(pos) * Rotate(-rotation) * Scale(scale);

    DEBUG_DrawTexture(tex, projection, texPart, color);
}
