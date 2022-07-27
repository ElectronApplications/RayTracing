#version 420 core

#define M_PI 3.1415926535897932384626433832795
#define REFLECTIONS 32
#define PATH_SAMPLES 4

struct Material {
    vec3 color;
    float diffuse;
    bool transparent;
    bool glowing;
    float ref_ratio;
};

struct RenderObject {
    float dist;
    vec3 normal;
    Material material;
};

struct Plane {
    vec3 normal;
    float w;
    Material material;
};

struct Sphere {
    vec3 pos;
    float radius;
    Material material;
};

struct Box {
    vec3 bound_min;
    vec3 bound_max;
    Material material;
};

Sphere spheres[] = {{vec3(-7.0, 10.0, -7.0), 3.0, {vec3(0.85, 0.85, 0.85), 0.0, false, false, 1.0}},
                    {vec3(0.0, -5.0, -7.0), 3.0, {vec3(0.9, 0.9, 0.9), 0.0, true, false, 0.75}},
                    {vec3(3.0, 2.0, -7.0), 3.0, {vec3(0.9, 0.9, 0.9), 1.0, false, false, 1.0}}};

Box boxes[] = {{vec3(-8.0, 5.0, -10.0), vec3(10.0, 7.0, 4.0), {vec3(0.9, 0.2, 0.4), 1.0, false, false, 1.0}}};

Plane planes[] = {{vec3(0.0, 0.0, 1.0), -10.0, {vec3(0.25, 0.75, 0.15), 1.0, false, false, 1.0}}};

uniform vec2 u_resolution;
uniform vec3 u_position;
uniform vec2 u_direction;
uniform float u_frames;
uniform float u_rand;
uniform sampler2D u_sample;
layout(location = 0) out vec3 fragColor;

#define FK(k) floatBitsToInt(cos(k))^floatBitsToInt(k)
float hash(float a, float b) {
    int x = FK(a); int y = FK(b);
    return float((x*x+y)*(y*y-x)+x)/2.14e9;
}

vec3 randvec(float seed) {
    float h1 = hash(seed, seed);
    float h2 = hash(h1, seed);
    float h3 = hash(h2, seed);
    return vec3(h1,h2,h3);
}

mat2 rot(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

vec3 skyColor(vec3 rd) {
    vec3 color = vec3(0.2, 0.4, 0.7);
    color += max(0.0, pow(dot(normalize(vec3(0, 2, 1)), rd), 128.0));
    return clamp(color, 0.0, 1.0);
}

vec2 sphereIntersect(vec3 ro, vec3 rd, Sphere sphere) {
    vec3 L = sphere.pos - ro;
    float tca = dot(L, rd);
    if(tca < 0)
        return vec2(-1, -1);
    float d2 = dot(L, L) - tca * tca;
    if(d2 > sphere.radius*sphere.radius)
        return vec2(-1, -1);
    float thc = sqrt(sphere.radius*sphere.radius - d2);
    return vec2(tca - thc, tca + thc);
}

vec2 boxIntersect(vec3 ro, vec3 rd, Box box, out vec3 normal) {
    vec3 invrd = 1.0 / rd;
    
    vec3 t1 = (box.bound_min - ro) * invrd;
    vec3 t2 = (box.bound_max - ro) * invrd;

    float tmin = max(max(min(t1.x, t2.x), min(t1.y, t2.y)), min(t1.z, t2.z));
    float tmax = min(min(max(t1.x, t2.x), max(t1.y, t2.y)), max(t1.z, t2.z));

    if(tmax >= tmin && tmin >= 0) {
        vec3 point = tmin*rd + ro;
        vec3 center = (box.bound_max + box.bound_min) * 0.5;
        vec3 pc = center - point;
        pc /= box.bound_max - box.bound_min;

        if(abs(pc.x) > abs(pc.y) && abs(pc.x) > abs(pc.z))
            normal = vec3(-sign(pc.x), 0.0, 0.0);
        else if(abs(pc.y) > abs(pc.x) && abs(pc.y) > abs(pc.z))
            normal = vec3(0.0, -sign(pc.y), 0.0);
        else
            normal = vec3(0.0, 0.0, -sign(pc.z));

        return vec2(tmin, tmax);
    }
    else
        return vec2(-1, -1);
}

float planeIntersect(vec3 ro, vec3 rd, Plane plane) {
    return -(dot(ro, plane.normal) - plane.w) / dot(rd, plane.normal);
}

vec3 castRay(vec3 ro, vec3 rd, float sampleNumber) {
    vec3 curcolor = vec3(1.0, 1.0, 1.0);

    for(int i = 0; i <= REFLECTIONS; i++) {
        RenderObject object = {1e30, vec3(0.0, 0.0, 0.0), {vec3(0.0, 0.0, 0.0), 0.0, false, false, 1.0}};
        
        for(int i = 0; i < spheres.length(); i++) {
            Sphere sphere = spheres[i];
            vec2 dists = sphereIntersect(ro, rd, sphere);
            if(dists.x != -1 && dists.x < object.dist) {
                object.material = sphere.material;
                if(object.material.transparent)
                    object.dist = dists.y;
                else
                    object.dist = dists.x;
                object.normal = normalize(ro + rd*dists.x - sphere.pos);
            }
        }

        for(int i = 0; i < boxes.length(); i++) {
            Box box = boxes[i];
            vec3 normal;
            vec2 dists = boxIntersect(ro, rd, box, normal);
            if(dists.x != -1 && dists.x < object.dist) {
                object.material = box.material;
                if(object.material.transparent)
                    object.dist = dists.y;
                else
                    object.dist = dists.x;
                    object.normal = normal;
            }
        }
        
        for(int i = 0; i < planes.length(); i++) {
            Plane plane = planes[i];
            float dist = planeIntersect(ro, rd, plane);
            if(dist > 0.001 && dist < object.dist) {
                object.material = plane.material;
                object.dist = dist;
                object.normal = -sign(dot(rd, plane.normal))*plane.normal;
            }
        }

        if(object.dist == 1e30) {
            curcolor *= skyColor(rd);
            break;
        }
        else if (i == REFLECTIONS)
            curcolor = vec3(0.0, 0.0, 0.0);
        else if(object.material.glowing) {
            curcolor *= object.material.color;
            break;
        }
        else if(object.material.transparent) {
            curcolor *= object.material.color;
            ro += rd * object.dist;
            rd = refract(rd, object.normal, object.material.ref_ratio);
            vec3 rand_dir = randvec(hash(u_frames*u_rand, sampleNumber*u_rand));
            rand_dir = normalize(rand_dir * dot(rand_dir, object.normal));
            rd = mix(rd, rand_dir, object.material.diffuse);
        }
        else {
            curcolor *= object.material.color;
            ro += rd * object.dist;
            rd = rd - 2*object.normal*dot(rd, object.normal);
            vec3 rand_dir = randvec(hash(u_frames*u_rand*ro.x*ro.z*rd.y*gl_FragCoord.x, sampleNumber*u_rand*rd.x*rd.z*ro.y*gl_FragCoord.y));
            rand_dir = normalize(rand_dir * dot(rand_dir, object.normal));
            rd = mix(rd, rand_dir, object.material.diffuse);
        }
    }

    return curcolor;
}

void main() {
    float ry = (2.0*gl_FragCoord.x - u_resolution.x) / u_resolution.x;
    float rz = ((2.0*gl_FragCoord.y - u_resolution.y) / u_resolution.y) * (u_resolution.y/u_resolution.x);
    vec3 rd = normalize(vec3(1.0, ry, rz));

    rd.zx *= rot(-u_direction.y);
    rd.xy *= rot(u_direction.x);

    vec3 color;
    for(int i = 0; i < PATH_SAMPLES; i++)
        color += castRay(u_position, rd, i);
    color /= PATH_SAMPLES;
    color = clamp(color, 0.0, 1.0);

    vec3 sampleColor = texture(u_sample, gl_FragCoord.xy / u_resolution).rgb;
    color = mix(sampleColor, color, 1.0 / u_frames);

    fragColor = color;
}