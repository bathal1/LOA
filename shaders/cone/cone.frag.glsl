#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
} fragment;

out vec4 FragColor;

uniform vec3 camera_position;
uniform vec3 color;
uniform float ambiant  = 0.2;
uniform float diffuse  = 0.8;
uniform float specular = 0.5;

vec3 light = camera_position+vec3(+1.0, +1.0, 0.0);

void main()
{
    vec3 n = normalize(fragment.normal.xyz);
    vec3 u = normalize(light-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = normalize(fragment.position.xyz-camera_position);

    float diffuse_value  = diffuse * clamp( dot(u,n), 0.0, 1.0);
    float specular_value = specular * pow( clamp( dot(r,t), 0.0, 1.0), 128.0);

    vec3 white = vec3(1.0);
    vec3 c = (ambiant+diffuse_value)*color.rgb + specular_value*white;

    FragColor = vec4(c, color_texture.a);
}
