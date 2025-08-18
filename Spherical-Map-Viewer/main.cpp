#define _CRT_SECURE_NO_WARNINGS

#include <vgl.h>
#include <InitShader.h>
#include "MySphere.h"
#include "Targa.h"
#include "MyObject.h"

#include <vec.h>
#include <mat.h>

#define PI 3.141592

MySphere sphere;
MyObject obj;
GLuint phong_prog;
GLuint sphere_prog;

mat4 g_Mat = mat4(1.0f);
float g_aspect = 1;

float angX = 0, angZ = 0;
int prevX, prevY;
bool isRightButtonDown = false;  // 오른쪽 버튼 상태
//float zoomLevel = 7.0f;          // 초기 줌 레벨
float r = 4.0f;
float FresnelPower = 11.0f;

char filename[30];

float mShiny = 1.0f;
int isObj = 0;
bool isDiffuseColor = false;

mat4 myLookAt(vec3 eye, vec3 at, vec3 up)
{
	mat4 V = mat4(1.0f);

	up = normalize(up);
	vec3 n = normalize(at - eye);
	float a = dot(up, n);
	vec3 v = normalize(up - a * n);
	vec3 w = cross(n, v);

	V[0] = vec4(w, dot(-w, eye));
	V[1] = vec4(v, dot(-v, eye));
	V[2] = vec4(-n, dot(n, eye));

	return V;
}
mat4 myPerspective(float angle, float aspect, float zNear, float zFar)
{
	float rad = angle * 3.141592 / 180.0f;
	mat4 V(1.0f);
	float h = 2 * zFar * tan(rad / 2);
	float w = aspect * h;
	mat4 S = Scale(2 / w, 2 / h, 1 / zFar);

	float c = -zNear / zFar;

	mat4 Mpt(1.0f);
	Mpt[2] = vec4(0, 0, 1 / (c + 1), -c / (c + 1));
	Mpt[3] = vec4(0, 0, -1, 0);


	V = Mpt * S;

	return V;
}

void myInit(char* filename)
{
	sphere.Init(40, 40);
	obj.Init("bunny.obj");

	phong_prog = InitShader("vPhong.glsl", "fPhong.glsl");
	glUseProgram(phong_prog);

	sphere_prog = InitShader("vSphere.glsl", "fSphere.glsl");
	glUseProgram(sphere_prog);

	char s_file[80];
	char d_file[80];

	sprintf(s_file, "%s_spheremap.tga", filename);
	sprintf(d_file, "%s_diffusemap.tga", filename);

	STGA image;
	for (int i = 0; i < 2; i++)
	{
		if (i == 0)		image.loadTGA(s_file);
		else if (i == 1)	image.loadTGA(d_file);
		int w = image.width;
		int h = image.height;
		printf("image size = %d, %d\n", w, h);

		GLuint tex;
		glGenTextures(1, &tex);

		if (i == 0)		glActiveTexture(GL_TEXTURE0);
		else if (i == 1)	glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D,
			0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void keyboard(unsigned char ch, int x, int y)
{
	if (ch == '1') {
		if(FresnelPower > 0.5)	FresnelPower -= 0.5;
		printf("FresnelPower = %.1f\n", FresnelPower);
	}
	else if (ch == '2') {
		if(FresnelPower < 11.0) FresnelPower += 0.5;
		printf("FresnelPower = %.1f\n", FresnelPower);
	}
	else if (ch == '3') {
		isDiffuseColor = !isDiffuseColor;
		if(isDiffuseColor)
			printf("Diffuse LightMap On\n");
		else
			printf("Diffuse LightMap Off\n");
	}
	else if (ch == 'q') {
		isObj = (isObj + 1) % 3;
	}
}

void myMouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			// 왼쪽 버튼 눌림
			prevX = x;
			prevY = y;
		}
		else if (button == GLUT_RIGHT_BUTTON) {
			// 오른쪽 버튼 눌림
			prevX = x;  // 현재 x 좌표 저장
			isRightButtonDown = true;  // 오른쪽 버튼 눌림 상태 설정
		}
	}

	if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON) {
			// 왼쪽 버튼 뗌
		}
		else if (button == GLUT_RIGHT_BUTTON) {
			// 오른쪽 버튼 뗌
			isRightButtonDown = false;  // 오른쪽 버튼 눌림 상태 해제
		}
	}
}

void myMotion(int x, int y) {
	if (isRightButtonDown) {
		// 오른쪽 버튼이 눌린 상태에서 x 변화량에 따라 줌 조정
		float dx = x - prevX;  // x 방향 변화량
		r += dx * 0.005f;  // 줌 레벨 변경 (감도 조정 가능)

		// 줌 레벨 제한 설정 (필요에 따라 수정)
		if (r < 2.0) r = 2.0f;  
		if (r > 19.0f) r = 19.0f; 
	}
	else {
		// 왼쪽 버튼이 눌린 상태에서 회전 각도 조정
		angX -= (x - prevX) * 0.005f;
		angZ -= (y - prevY) * 0.002f;
		// 수직각도 제한
		if (angZ > PI / 2.0f) angZ = PI/ 2.0f;
		if (angZ < -PI / 2.0f) angZ = -PI / 2.0f;
	}

	prevX = x;
	prevY = y;

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	g_aspect = w / float(h);
	glutPostRedisplay();
}

void idle()
{
	Sleep(16);
	glutPostRedisplay();
}

void display()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	float theta = angX * PI / 5;
	float phi = angZ * PI / 5;
	//vec3 ePos = vec3(4 * cos(theta) * cos(phi), 4 * sin(phi), 4 * sin(theta) * cos(phi));
	vec3 ePos = vec3(r * cos(theta) * cos(phi), r * sin(phi), r * sin(theta) * cos(phi));

	//float objSize = 0.65 * zoomLevel / 7.0f;
	float objSize = 0.65; 
	mat4 ModelMat = Scale(objSize, objSize, objSize);
	mat4 ViewMat = myLookAt(ePos, vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 ProjMat = myPerspective(45, g_aspect, 0.01, 100.0f);

	g_Mat = ProjMat * ViewMat * ModelMat;

	glUseProgram(sphere_prog);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); // 앞쪽 면만 제거
	GLuint uMat = glGetUniformLocation(sphere_prog, "uMat");
	GLuint uTex = glGetUniformLocation(sphere_prog, "uTex");
	GLuint uCPos = glGetUniformLocation(phong_prog, "uCPos");
	glUniform3f(uCPos, ePos[0], ePos[1], ePos[2]);
	//glUniformMatrix4fv(uMat, 1, GL_TRUE, ProjMat * ViewMat * Scale(zoomLevel, zoomLevel, zoomLevel));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, ProjMat * ViewMat * Scale(7, 7, 7));
	glUniform1i(uTex, 0);

	sphere.Draw(sphere_prog);

	glUseProgram(phong_prog);
	glDisable(GL_CULL_FACE);	// 컬링을 비활성화
	GLuint uSphereTex = glGetUniformLocation(phong_prog, "uSphereTex");
	GLuint uDiffuseTex = glGetUniformLocation(phong_prog, "uDiffuseTex");
	GLuint uModelMat = glGetUniformLocation(phong_prog, "uModelMat");
	GLuint uViewMat = glGetUniformLocation(phong_prog, "uViewMat");
	GLuint uProjMat = glGetUniformLocation(phong_prog, "uProjMat");

	// 1. Define Light Properties
	vec4 lPos = vec4(2, 2, 0, 1);
	vec4 lAmb = vec4(0.5, 0.5, 0.5, 1);
	vec4 lDif = vec4(1, 1, 1, 1);
	//vec4 lSpc = lDif;

	// 2. Define Material Properties
	vec4 mAmb = vec4(0.3, 0.3, 0.3, 1);
	vec4 mDif = vec4(0.8, 0.8, 0.8, 1);
	//vec4 mSpc = vec4(0.3, 0.3, 0.3, 1);

	// I = lAmb*mAmb + lDif*mDif*(N·L) + lSpc*mSpc*(V·R)^n; 
	vec4 amb = lAmb * mAmb;
	vec4 dif = lDif * mDif;
	//vec4 spc = lSpc * mSpc;


	GLuint uLPos = glGetUniformLocation(phong_prog, "uLPos");
	GLuint uEPos = glGetUniformLocation(phong_prog, "uEPos");
	GLuint uAmb = glGetUniformLocation(phong_prog, "uAmb");
	GLuint uDif = glGetUniformLocation(phong_prog, "uDif");
	//GLuint uSpc = glGetUniformLocation(phong_prog, "uSpc");
	GLuint uShininess = glGetUniformLocation(phong_prog, "uShininess");
	GLuint uFresnelPower = glGetUniformLocation(phong_prog, "uFresnelPower");
	GLuint uisDiffuseColor = glGetUniformLocation(phong_prog, "uisDiffuseColor");

	glUniformMatrix4fv(uModelMat, 1, true, ModelMat);
	glUniformMatrix4fv(uViewMat, 1, true, ViewMat);
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat);
	glUniform4f(uLPos, lPos[0], lPos[1], lPos[2], lPos[3]);
	glUniform4f(uAmb, amb[0], amb[1], amb[2], amb[3]);
	glUniform4f(uDif, dif[0], dif[1], dif[2], dif[3]);
	//glUniform4f(uSpc, spc[0], spc[1], spc[2], spc[3]);
	glUniform4f(uEPos, ePos[0], ePos[1], ePos[2], 1);
	glUniform1f(uShininess, mShiny);
	glUniform1f(uFresnelPower, FresnelPower);
	glUniform1i(uisDiffuseColor, isDiffuseColor);

	glUniform1i(uSphereTex, 0);
	glUniform1i(uDiffuseTex, 1);

	if (isObj == 1)	sphere.Draw(phong_prog);
	else if (isObj == 2) obj.Draw(phong_prog);
	
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	printf("SIMPLE SPHERICAL MAP VIEWER\n");
	printf("Programming Assignment #5 for Computer Graphics\n");
	printf("Department of Software, Sejong University\n");
	printf("\n");
	printf("----------------------------------------------------------------\n");
	printf("Left Mouse Button Dragging : rotating the viewpoint\n");
	printf("Right Mouse Button Dragging : zoom - in / out\n");
	printf("\n");
	printf("`1' key : Decreasing Fresnel power parameter for shading\n");
	printf("`2' key : Increasing Fresnel power parameter for shading\n");
	printf("`3' key : Turn on / off the diffuse map\n");
	printf("`q' key : toggling between Sphere Model and Bunny Model\n");
	printf("Spacebar : start / stop rotating the model\n");
	printf("----------------------------------------------------------------\n");
	printf("\n");
	printf("Input Image Group Name(ex : class1 / class2 / ny / ice etc.) : ");
	scanf("%s", filename);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 500);

	glutCreateWindow("Homework5");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));


	myInit(filename);

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(myMouse);
	glutMotionFunc(myMotion);
	glutMainLoop();

	return 0;
}
