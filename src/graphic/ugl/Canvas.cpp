#include "Canvas.h"

void unlogic::Canvas::DrawPrimitive(const unlogic::VertexBuffer &vbo)
{
    // Setup camera and transformation matrices
    glm::mat4 view(1.f);
    view = glm::translate(view, this->camera_);

    glm::vec2 world_size = glm::vec2(this->screen_.x, this->screen_.y) / this->px_per_unit;
    glm::mat4 projection = glm::ortho(world_size.x / -2.f, world_size.x / 2.f, world_size.y / -2.f, world_size.y / 2.f);

    // Setup shader
    glUseProgram(this->shader_program_);

    GLint view_uniform = glGetUniformLocation(this->shader_program_, "view");
    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

    GLint projection_uniform = glGetUniformLocation(this->shader_program_, "projection");
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

    // Generate object buffers
    GLuint vao = -1;
    glGenVertexArrays(1, &vao);

    // Bind vertex array
    glBindVertexArray(vao);

    // Bind data to buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo.vbo_);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); // Position
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); // Color
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Draw
    glDrawArrays((GLenum)vbo.primitive_type_, 0, vbo.size_);

    // Cleanup
    glDeleteVertexArrays(1, &vao);
}

void unlogic::Canvas::Draw(unlogic::Drawable &drawable)
{
    drawable.DrawOnto(*this);
}

void unlogic::Canvas::Clear(float r, float g, float b, float a)
{
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void unlogic::Canvas::SetScreenSize(const glm::ivec2 &size)
{
    this->screen_ = size;
}

void unlogic::Canvas::Initialize(const glm::ivec2 &screen_size)
{
    this->GetContext()->make_current();

    // Compile and link shaders
    GLuint vertex_shader = this->LoadShaderFromFile("resource/shaders/plot.vert");
    GLuint fragment_shader = this->LoadShaderFromFile("resource/shaders/plot.frag");

    this->shader_program_ = glCreateProgram();
    glAttachShader(this->shader_program_, vertex_shader);
    glAttachShader(this->shader_program_, fragment_shader);
    glLinkProgram(this->shader_program_);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    this->SetScreenSize(screen_size);
}

void unlogic::Canvas::Destroy()
{
    glDeleteProgram(this->shader_program_);
    this->shader_program_ = 0;
}

GLuint unlogic::Canvas::LoadShaderFromFile(const std::filesystem::path &path)
{
    GLenum shader_type = path.extension() == ".vert" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    GLuint shader = glCreateShader(shader_type);

    std::string source = unlogic::load_file(path);

    char const* source_array[] = { source.c_str(), nullptr };
    glShaderSource(shader, 1, source_array, nullptr);
    glCompileShader(shader);

    return shader;
}

glm::vec2 unlogic::Canvas::GetWorldSize() const
{
    auto world_bounds = this->GetWorldBounds();

    return {world_bounds.z - world_bounds.x, world_bounds.y - world_bounds.w};
}

glm::vec4 unlogic::Canvas::GetWorldBounds() const
{
    glm::vec2 world_size = glm::vec2(this->screen_.x, this->screen_.y) / this->px_per_unit;
    return {world_size.x / -2.f, world_size.y / 2.f, world_size.x / 2.f, world_size.y / -2.f};
}
