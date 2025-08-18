#version 330

in vec3 N3; 
in vec3 T3;
in vec3 L3; 
in vec3 V3; 
in vec3 wV;
in vec3 wN;
in vec4 Color;

out vec4 fColor;

uniform mat4 uModelMat; 
uniform mat4 uProjMat; 
uniform vec4 uLPos; 
uniform vec4 uEPos; 
uniform vec4 uAmb; 
uniform vec4 uDif; 
//uniform vec4 uSpc; 
uniform float uShininess; 
uniform float uFresnelPower; 
uniform bool uisDiffuseColor;

uniform sampler2D uSphereTex;
uniform sampler2D uDiffuseTex;

void main()
{
	vec3 N = normalize(N3);  
    vec3 L = normalize(L3);  
    vec3 V = normalize(V3);   
    vec3 H = normalize(V+L);   

    float d = max(0.0, dot(N, L));  
    //float VR = pow(max(dot(H, N), 0), uShininess);   

    vec4 amb = uAmb;  
    vec4 dif = uDif * d; 
    //vec4 spec = uSpc * VR;  

    vec3 dir = normalize(2 * dot(wV, wN) * wN - wV);
	float PI = 3.141592f;
    float u = (atan(dir.z, dir.x) + PI) / (2 * PI);
    float v = -asin(dir.y) / PI + 0.5;

    if(uisDiffuseColor){
        float x = (atan(wN.z, wN.x) + PI) / (2 * PI);
        float y = asin(wN.y) / PI + 0.5;
        vec4 diffuseColor = texture(uDiffuseTex, vec2(x, y));
        dif = diffuseColor;
    }
    
    fColor = amb + dif; 
	fColor.w = 1;
    vec4 reflectionColor = texture(uSphereTex, vec2(u, v));

	float F = 0.0;
	float ratio = F+(1.0-F)*pow((1.0+dot(wV, wN)), uFresnelPower);
	if(uFresnelPower>10) ratio = 0; // 10보다 클경우 반사 없음
    fColor = mix(fColor, reflectionColor, ratio);
}
