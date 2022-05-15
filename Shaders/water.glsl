#type vertex
#version 330 core

layout(location = 0) in vec3 bPosition;
layout(location = 1) in vec4 bColor;
layout(location = 2) in vec2 bTexCoord;

out vec2 vTexCoord;
out vec2 vWorldPos;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * uView * vec4(bPosition, 1.0);
    vTexCoord = bTexCoord;
    vWorldPos = vec2(bPosition);
}

#type fragment
#version 330 core

out vec4 Color;

in vec2 vTexCoord;
in vec2 vWorldPos;

uniform sampler2D uChannels[2];
uniform float uElapsedTime;
uniform vec3 uWaterColor;
uniform int uTileLevel;
uniform vec2 uLightPositions[5];
uniform int uNumLights;
uniform vec3 uLightAttenuation;
uniform vec3 uAmbient;

vec4 texCube(in sampler2D tex, in vec3 p) {
    return texture(tex, p.xz * 0.5 + 0.5);
}

float height(in vec2 uv) {
    return texture(uChannels[0], uv).x * texture(uChannels[1], uv + vec2(0.0, uElapsedTime * 0.1)).x;
}

const vec2 NE = vec2(0.05,0.0);
vec3 normal(in vec2 uv) {
    return normalize(vec3(height(uv + NE.xy) - height(uv - NE.xy),
                            0.0,
                            height(uv + NE.yx) - height(uv - NE.yx)));
}

vec4 waterColor = vec4(normalize(uWaterColor), 1.0);//vec4(0.1, 0.1, 0.32, 0.5);

void main()
{
    vec2 uv = vTexCoord * uTileLevel;
    uv.y += sin(uv.y * 20.0 + uElapsedTime) * 0.01;
    uv.x += sin(uv.y * 40.0 + uElapsedTime) * 0.005;

    float h = height(uv);
    vec3 norm = normal(uv);
    vec4 water = mix(
                    mix(waterColor, texture(uChannels[0], uv), 0.5),
                    texture(uChannels[0], norm.xz * 0.5 + 0.5), 1.0
                    ) * 0.5;
    vec2 lightPos = uLightPositions[0];
    float dist = length(vWorldPos - lightPos);
	float attenuation = 1.0 / (uLightAttenuation.x + uLightAttenuation.y * dist + uLightAttenuation.z * dist * dist);	
    Color = attenuation + vec4(uAmbient, 1);
}			
