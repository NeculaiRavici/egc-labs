#version 330

// Input (vezi Lab6::CreateMesh si structura VertexFormat)
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Bonus
uniform float Time;
uniform int Bonus;

// Output
out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture_coord;
out vec3 frag_color;

void main()
{
    vec3 position = v_position;

    // Bonus: obiectul "respira" si oscileaza pe Y in functie de timp
    if (Bonus == 1) {
        position *= 1.0 + 0.2 * sin(3.0 * Time);
        position.y += 0.5 * sin(2.0 * Time + v_position.x);
    }

    // Send output to fragment shader
    frag_position = position;
    frag_normal = v_normal;
    frag_texture_coord = v_texture_coord;
    frag_color = v_color;

    // Compute gl_Position
    gl_Position = Projection * View * Model * vec4(position, 1.0);
}
