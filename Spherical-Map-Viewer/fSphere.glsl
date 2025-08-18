#version 330

in vec2 T2;
in vec4 color;

out vec4 fColor;

uniform sampler2D uTex;

void main()
{
	fColor = color;
	vec4 texColor = texture(uTex, vec2(1 - T2.x, T2.y));

	fColor = texColor;
	fColor.w = 1;
}
