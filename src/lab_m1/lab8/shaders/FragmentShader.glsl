#version 330

#define NUM_LIGHTS 2

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
// Light 0 = the movable light (point or spot), light 1 = extra point light
uniform vec3 light_direction;
uniform vec3 light_position[NUM_LIGHTS];
uniform vec3 light_color[NUM_LIGHTS];
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform int   is_spotlight;   // 1 = light 0 behaves as a spot
uniform float cut_off;        // spot cut-off angle, in degrees

uniform vec3 object_color;

// 1 = Blinn (half vector), 0 = Phong (reflect)
uniform int use_blinn;

// Output
layout(location = 0) out vec4 out_color;


vec3 light_contribution(vec3 light_pos, vec3 light_col, bool spot);


void main()
{
    // Ambient: Ia = 0.25, Kd used instead of Ka
    vec3 color = vec3(0.25 * material_kd);

    // Light 0 can be a spot; the others are always point lights
    color += light_contribution(light_position[0], light_color[0], is_spotlight == 1);
    for (int i = 1; i < NUM_LIGHTS; i++)
    {
        color += light_contribution(light_position[i], light_color[i], false);
    }

    out_color = vec4(object_color * color, 1.0);
}


// Diffuse + specular contribution (Blinn or Phong) of one light, with
// distance attenuation and, for a spot, angular attenuation.
vec3 light_contribution(vec3 light_pos, vec3 light_col, bool spot)
{
    vec3 N = normalize(world_normal);
    vec3 L = normalize(light_pos - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    float diffuse_light = material_kd * max(dot(N, L), 0.0);
    float specular_light = 0.0;

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light > 0.0)
    {
        if (use_blinn == 1)
        {
            specular_light = material_ks * pow(max(dot(N, H), 0.0), material_shininess);
        }
        else
        {
            vec3 R = reflect(-L, N);
            specular_light = material_ks * pow(max(dot(V, R), 0.0), material_shininess);
        }
    }

    // Distance attenuation
    float d = distance(light_pos, world_position);
    float att = 1.0 / (1.0 + 0.1 * d + 0.05 * d * d);

    if (spot)
    {
        float spot_light = dot(-L, normalize(light_direction));
        float spot_light_limit = cos(radians(cut_off));

        if (spot_light <= spot_light_limit)
        {
            return vec3(0.0);   // outside the cone
        }

        // Quadratic attenuation towards the cut-off edge
        float linear_att = (spot_light - spot_light_limit) / (1.0 - spot_light_limit);
        att *= pow(linear_att, 2.0);
    }

    return att * light_col * (diffuse_light + specular_light);
}
