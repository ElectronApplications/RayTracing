#version 420 core

uniform vec2 u_resolution;
uniform sampler2D u_sample;
out vec3 fragColor;

vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main() {
    vec3 color = texture(u_sample, gl_FragCoord.xy / u_resolution).rgb;
    color = ACESFilm(color);
    color = pow(color, vec3(1.0 / 2.2));
    fragColor = color;
}