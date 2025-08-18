#version 330

in  vec4 vPosition;
in  vec3 vNormal;

out vec3 N3; 
out vec3 L3; 
out vec3 V3;  
out vec3 wV;
out vec3 wN;

uniform mat4 uModelMat; 
uniform mat4 uViewMat; 
uniform mat4 uProjMat; 
uniform vec4 uLPos; 
uniform vec4 uEPos; 

void main()
{
	gl_Position  = uProjMat*uViewMat*uModelMat*vPosition;
	gl_Position *= vec4(1,1,-1,1);
   
	vec4 N = uViewMat*uModelMat*vec4(vNormal,0); 
	vec4 L = uLPos - uViewMat*uModelMat*vPosition; 
	vec4 V = vec4(0, 0, 0, 1) - uViewMat*uModelMat*vPosition; 

	N3 = normalize(N.xyz); 
	L3 = normalize(L.xyz); 
	V3 = normalize(V.xyz); 

	wN = normalize(vNormal);
	wV = normalize(-uEPos.xyz);
}