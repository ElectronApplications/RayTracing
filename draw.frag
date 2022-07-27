#version 420 core

uniform vec2 u_resolution;
uniform sampler2D u_sample;
out vec3 fragColor;

void main() {
    vec3 color = texture(u_sample, gl_FragCoord.xy / u_resolution).rgb;
    
    // float white = 2.0;
    // float exposure = 4.0;
    // color *= white * exposure;
    // color = (color * (1.0 + color / white / white)) / (1.0 + color);
    // color = clamp(color, 0.0, 1.0);
    
    // color = pow(color, vec3(1.0 / 2.0));

    fragColor = color;
}