#version 330

// Input (interpolate liniar intre varfurile primitivei)
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coord;
in vec3 frag_color;

// Bonus
uniform float Time;
uniform int Bonus;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    // Cerinta 3 (culoarea varfului): out_color = vec4(frag_color, 1);
    // Cerinta 4: normala pe post de culoare
    vec3 color = frag_normal;

    // Bonus: canalele R si B variaza dupa functii trigonometrice de timp
    if (Bonus == 1) {
        color.r = 0.5 + 0.5 * sin(Time);
        color.b = 0.5 + 0.5 * cos(1.5 * Time);
    }

    out_color = vec4(color, 1);
}
