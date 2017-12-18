#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"


double camfl = 0;
bool textureMode = true;
bool lightMode = true;
double vx = 0;
double vy = 0;
double vz = 0;
int movement = 0;
bool flag = true;
int time = 0;
int ht = 1;
GLuint texId[11];
bool alpha = false;
bool cutP = false;
//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 487;


		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);
		//�������� �� ������� ������ ������
		lookPoint.setCoords(vx,vy, vz);

		/*pos.setCoords(camDist*cos(fi2)*cos(fi1),
		camDist*cos(fi2)*sin(fi1),
		camDist*sin(fi2));*/

		pos.setCoords(vx + camDist*cos(fi2)*cos(fi1),
			vy + camDist*cos(fi2)*sin(fi1),
			vz + camDist*sin(fi2));
		//pos.setCoords(camDist*cos(fi2)*cos(fi1),camDist*cos(fi2)*sin(fi1),camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		if (pos.X() > 488)
			pos = Vector3(487,pos.Y(),pos.Z());
		else if(pos.X()<-488)
			pos = Vector3(-487, pos.Y(), pos.Z());
		 if (pos.Y() > 488)
			pos = Vector3(pos.X(),487, pos.Z());
		else if (pos.Y()<-488)
			pos = Vector3(pos.X(), -487, pos.Z());
		 if (pos.Z() > 488)
			pos = Vector3(pos.X(), pos.Y(),487);
		else if (pos.Z()<-488)
			pos = Vector3(pos.X(), pos.Y(), -487);
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
		


	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 478)
		return;

	camera.camDist += 0.07*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'C'&&!cutP)//�������,�������� ��� �������� ��������� ������
	{
		ht = 1;
		if(camfl!=0)
		cutP = true;

	}
	else if (key == 'C' && cutP) {

		ht = 1;
		cutP = false;;

	}

	if (key == 'A' && !alpha)
	{
		lightMode = true;
		textureMode = false;
		cutP = false;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		alpha = true;
	}
	else if (key == 'A' && alpha)
	{
		textureMode = !textureMode;

		glDisable(GL_BLEND);
		alpha = false;

		cutP = false;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{/*
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;
*/		camfl = 0;
		 vx = 0;
		 vy = 0;
		 vz = 0;
		camera.camDist = 487;
		camera.fi1 = 1;
		camera.fi2 = 1;
		cutP = false;
		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	if (key == '1')//��������
	{
		ht = 1;
		camfl = 1;
		cutP = false;
	}
	if (key == '2')//������
	{
		ht = 1;
		camfl = 2;

		cutP = false;
	}
	if (key == '3')//�����
	{
		ht = 1;
		camfl = 3;

		cutP = false;
	}
	if (key == '4')//����
	{
		ht = 1;
		camfl = 4;

		cutP = false;
	}
	if (key == '5')//cerera
	{
		ht = 1;
		camfl = 5;

		cutP = false;
	}
	if (key == '6')//������
	{
		ht = 1;
		camfl = 6;

		cutP = false;
	}
	if (key == '7')//������
	{
		ht = 1;
		camfl = 7;

		cutP = false;
	}
	if (key == '8')//����
	{
		ht = 1;
		camfl = 8;

		cutP = false;
	}
	if (key == '9')//������
	{
		ht = 1;
		camfl = 9;

		cutP = false;
	}
	if (key == '0')//������
	{
		ht = 1;
		camfl = 10;

		cutP = false;
	}
	if (key == 'M')//��������
	{
		ht = 1;
		camfl = 11;

		cutP = false;
	}
	if (key == 'O')//-
	{
		ht = 1;
		if (camfl == 1)camfl = 11;
		else camfl--;


		cutP = false;

	}
	if (key == 'P')//+
	{
		ht = 1;
		if (camfl == 11)camfl = 1;
		else camfl++;


		cutP = false;
	}


}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}




//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;//������
	RGBTRIPLE *texarray1; //�����
	RGBTRIPLE *texarray2; //������
	RGBTRIPLE *texarray3; //��������
	RGBTRIPLE *texarray4; //������
	RGBTRIPLE *texarray5; //����
	RGBTRIPLE *texarray6; //������
	RGBTRIPLE *texarray7; //������
	RGBTRIPLE *texarray8; //����
	RGBTRIPLE *texarray9; //������
	RGBTRIPLE *texarray10;//������

						  //������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	/*
	GLuint texId;*/
	//���������� �� ��� ��������
	glGenTextures(11, texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId[0]);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray1; //�
	OpenGL::LoadBMP("texture1.bmp", &texW, &texH, &texarray1); //�
	OpenGL::RGBtoChar(texarray1, texW, texH, &texCharArray1); //�
	glBindTexture(GL_TEXTURE_2D, texId[1]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray1); //�
	free(texCharArray1); //�
	free(texarray1); //�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray10; //�
	OpenGL::LoadBMP("texture10.bmp", &texW, &texH, &texarray10); //�
	OpenGL::RGBtoChar(texarray10, texW, texH, &texCharArray10); //�
	glBindTexture(GL_TEXTURE_2D, texId[10]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray10); //�
	free(texCharArray10); //�
	free(texarray10); //�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	char *texCharArray2; //�
	OpenGL::LoadBMP("texture2.bmp", &texW, &texH, &texarray2); //�
	OpenGL::RGBtoChar(texarray2, texW, texH, &texCharArray2); //�
	glBindTexture(GL_TEXTURE_2D, texId[2]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2); //�
	free(texCharArray2); //�
	free(texarray2); //�
					 //������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);



	char *texCharArray3; //�
	OpenGL::LoadBMP("texture3.bmp", &texW, &texH, &texarray3); //�
	OpenGL::RGBtoChar(texarray3, texW, texH, &texCharArray3); //�
	glBindTexture(GL_TEXTURE_2D, texId[3]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray3); //�
	free(texCharArray3); //�
	free(texarray3); //�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray4; //�
	OpenGL::LoadBMP("texture4.bmp", &texW, &texH, &texarray4); //�
	OpenGL::RGBtoChar(texarray4, texW, texH, &texCharArray4); //�
	glBindTexture(GL_TEXTURE_2D, texId[4]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray4); //�
	free(texCharArray4); //�
	free(texarray4); //�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray5; //�
	OpenGL::LoadBMP("texture5.bmp", &texW, &texH, &texarray5); //�
	OpenGL::RGBtoChar(texarray5, texW, texH, &texCharArray5); //�
	glBindTexture(GL_TEXTURE_2D, texId[5]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray5); //�
	free(texCharArray5); //�
	free(texarray5); //�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray6; //�
	OpenGL::LoadBMP("texture6.bmp", &texW, &texH, &texarray6); //�
	OpenGL::RGBtoChar(texarray6, texW, texH, &texCharArray6); //�
	glBindTexture(GL_TEXTURE_2D, texId[6]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray6); //�
	free(texCharArray6); //�
	free(texarray6); //�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray7; //�
	OpenGL::LoadBMP("texture7.bmp", &texW, &texH, &texarray7); //�
	OpenGL::RGBtoChar(texarray7, texW, texH, &texCharArray7); //�
	glBindTexture(GL_TEXTURE_2D, texId[7]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray7); //�
	free(texCharArray7); //�
	free(texarray7); //�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray8; //�
	OpenGL::LoadBMP("texture8.bmp", &texW, &texH, &texarray8); //�
	OpenGL::RGBtoChar(texarray8, texW, texH, &texCharArray8); //�
	glBindTexture(GL_TEXTURE_2D, texId[8]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray8); //�
	free(texCharArray8); //�
	free(texarray8); //�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray9; //�
	OpenGL::LoadBMP("texture9.bmp", &texW, &texH, &texarray9); //�
	OpenGL::RGBtoChar(texarray9, texW, texH, &texCharArray9); //�
	glBindTexture(GL_TEXTURE_2D, texId[9]); //�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray9); //�
	free(texCharArray9); //�
	free(texarray9); //�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}



//��� �������
//������ ������� ��� ���������
void norm(double A[], double B[], double C[], double vector[])
{
	double vector1[] = { A[0] - B[0], A[1] - B[1] ,A[2] - B[2] };
	double vector2[] = { C[0] - B[0], C[1] - B[1] ,C[2] - B[2] };

	vector[0] = vector1[1] * vector2[2] - vector2[1] * vector1[2];
	vector[1] = -vector1[0] * vector2[2] + vector2[0] * vector1[2];
	vector[2] = vector1[0] * vector2[1] - vector2[0] * vector1[1];

	double length = pow((pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2)), 0.5);
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}
void normal(double a1[], double b1[], double c1[], double tv[]) {


	double a[] = { a1[0] - b1[0],a1[1] - b1[1],a1[2] - b1[2] };
	double b[] = { c1[0] - b1[0],c1[1] - b1[1],c1[2] - b1[2] };

	tv[0] = a[1] * b[2] - b[1] * a[2];
	tv[1] = -a[0] * b[2] + b[0] * a[2];
	tv[2] = a[0] * b[1] - b[0] * a[1];


	double l = pow((pow(tv[0], 2) + pow(tv[1], 2) + pow(tv[2], 2)), 0.5);
	tv[0] = tv[0] / l;
	tv[1] = tv[1] / l;
	tv[2] = tv[2] / l;

}
//��������� ������ ��� �������
void planet(float R, int s2, int s1, int m, int l, double rgb1[], double rgb2[], bool f)
{//������ � ���������� ��������� � ������ ���������,�� ������� ������ ������ �����
	float x1, y1, z1, x2, y2, z2;
	float a = 1, a2 = 1;
	double t1=1024, t2=1024;
	glColor3dv(rgb1);
	for (int i = 0; i < s1 / m; i++)//��� �� �������� ����� ����� s1 �� 2 ���� ���� ���������,�� 3 ���� ���� ����� � � � 
	{
		t1 = 0;
		t2 -= 1024.0 / s1;
		a2 = (i*M_PI * 2) / (s1 / m);
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j <= s2 / l; j++)
		{//��� �� �������� ����� ����� s2 �� 2 ���� ���� ���������,�� 3 ���� ���� ����� � � � 

			x1 = R * sin(-2 * M_PI * j / s2) * sin(M_PI * i / s1);
			x2 = R * sin(-2 * M_PI * j / s2) * sin(M_PI * (i + 1) / s1);
			y1 = R * cos(-2 * M_PI * j / s2) * sin(M_PI * i / s1);
			y2 = R * cos(-2 * M_PI * j / s2) * sin(M_PI * (i + 1) / s1);
			z1 = R * cos(M_PI * i / s1);
			z2 = R * cos(M_PI * (i + 1) / s1);
			if (f == true) {
				a2 = (i*M_PI * 2) / (s1 / m);
			}
			else {
				a2 = ((i + 1)*M_PI * 2) / (s1 / m);
			}
			t1 -= 1024.0 / s2;
			glNormal3d(x1 / R * sin(M_PI * i / s1), y1 / R * sin(M_PI * i / s1), z1 / R);
			glTexCoord2d(t1 / 1024.0, t2 / 1024.0);
			//glTexCoord2d(x1 / 1024.0, y1 / 1024.0);
			glVertex3d(x1, y1, z1);


			if (f == true) {
				a2 = ((i + 1)*M_PI * 2) / (s1 / m);
			}
			else {
				a2 = (i*M_PI * 2) / (s1 / m);
			}
			glNormal3d(x2 / R * sin(M_PI * (i + 1) / s1), y2 / R * sin(M_PI * (i + 1) / s1), z2 / R);
			glTexCoord2d(t1 / 1024.0, t2 / 1024.0);
			//glTexCoord2d(x2 / 1024.0, y2 / 1024.0);
			glVertex3d(x2, y2, z2);
		}
		glEnd();
	}

}
//��������� ����� ��� �������
void sphere(float R, int s2, int s1,int m,int l, double rgb1[], double rgb2[],bool f)
{//������ � ���������� ��������� � ������ ���������,�� ������� ������ ������ �����
	//glBindTexture(GL_TEXTURE_2D, texId[1]);
	float x1, y1, z1, x2, y2, z2;
	float a = 1, a2 = 1;
	float y3 = 1024 - cos(a)*cos(a2) * 100 / 1024.0;
	for (int i = 0; i < s1/m; i++)//��� �� �������� ����� ����� s1 �� 2 ���� ���� ���������,�� 3 ���� ���� ����� � � �
	{
		a2 = (i*M_PI * 2) / (s1/m);
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j <= s2/l; j++) 
		{//��� �� �������� ����� ����� s2 �� 2 ���� ���� ���������,�� 3 ���� ���� ����� � � �

			x1 = R * sin(-2 * M_PI * j / s2) * sin(M_PI * i / s1);
			x2 = R * sin(-2 * M_PI * j / s2) * sin(M_PI * (i + 1) / s1);
			y1 = R * cos(-2 * M_PI * j / s2) * sin(M_PI * i / s1);
			y2 = R * cos(-2 * M_PI * j / s2) * sin(M_PI * (i + 1) / s1);
			z1 = R * cos(M_PI * i / s1);
			z2 = R * cos(M_PI * (i + 1) / s1);
			if (f == true) {
				glColor3dv(rgb1);
				a2 = (i*M_PI * 2) / (s1 / m);
			}
			else {
				glColor3dv(rgb1);
				a2 = ((i + 1)*M_PI * 2) / (s1 / m);
			}
			//glNormal3d(x1 / R * sin(M_PI * i / s1), y1 / R * sin(M_PI * i / s1), z1 / R);
			////glTexCoord2d(i/(s1/m)/1024.0,1- j/(s2/l)/1024.0);
			//////glTexCoord2d(i / s1, j / s2);
			////glTexCoord2d(-sin(a2), y3);
			//glVertex3d(x1, y1, z1);
			glNormal3d(x1 / R * sin(M_PI * i / s1), y1 / R * sin(M_PI * i / s1), z1 / R);
			//glTexCoord2d(x1 / 1024.0, y1 / 1024.0);
			glVertex3d(x1, y1, z1);

			if (f == true) {
				glColor3dv(rgb1);
				a2 = ((i + 1)*M_PI * 2) / (s1 / m);
			}
			else {
				glColor3dv(rgb1);
				a2 = (i*M_PI * 2) / (s1 / m);
			}

			//glNormal3d(x2 / R * sin(M_PI * (i + 1) / s1), y2 / R * sin(M_PI * (i + 1) / s1), z2 / R);
			//////glTexCoord2d((i+1) / (s1 / m) / 1024.
			////glTexCoord2d(-sin(a2), y3);
			//glVertex3d(x2, y2, z2);

			glNormal3d(x2 / R * sin(M_PI * (i + 1) / s1), y2 / R * sin(M_PI * (i + 1) / s1), z2 / R);
			//glTexCoord2d(x2 / 1024.0, y2 / 1024.0);
			glVertex3d(x2, y2, z2);
		}
		glEnd();
	}
}
//��������� ����������
void round(float r,int s,int f,double rgb1[],double rgb2[])//������ � ������� ���������,���� 1 �=0,2 �=0,3 z=0
{
	float x=0, y=0, z=0;
	glBegin(GL_TRIANGLE_FAN);

		switch (f) {
		case (1):
			glNormal3d(1, 0, 0);
			break;
		case (2):
			glNormal3d(0, -1, 0);
			break;
		case (3):
			glNormal3d(0, 0, -1);
			break;
		}

	glColor3dv(rgb1);
	glVertex3d(0, 0, 0);
	for (int i = 0; i <= s;i++) {
		switch (f) {
		case (1):	y = r*cos(i*M_PI / 180);
					z = r*sin(i*M_PI / 180);
					break;
		case (2):	x = r*cos(i*M_PI / 180);
					z = r*sin(i*M_PI / 180);
					break;
		case (3):	x = r*cos(i*M_PI / 180);
					y = r*sin(i*M_PI / 180);
					break;
		}
		
		glColor3dv(rgb2);
		glVertex3d(x,y,z);

	}


	glEnd();


}
//��������� ��������� ���� 
void litosphere(double rgb1[], double rgb2[]) 
{
	rgb1[0] = 0.1;
	rgb1[1] = 0.5;
	rgb1[2] = 0.7;

	rgb2[0] = 0.1;
	rgb2[1] = 0.3;
	rgb2[2] = 1;
	sphere(5, 50, 50, 1, 2, rgb1, rgb2, true);//���������
	glPushMatrix();
	glRotated(160, 0, 0, 1);
	sphere(5, 50, 50, 1, 4, rgb1, rgb2, true);//���������
	glRotated(-4, 0, 0, 1);
	round(5, 360, 2, rgb1, rgb2);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-270, 0, 0, 1);
	round(4.99, 360, 2, rgb1, rgb2);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	round(4.99, 360, 3, rgb1, rgb2);//���������� 
	glPopMatrix();
	//round(5, 360, 2, rgb1, rgb2);//����������  ������
	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	sphere(5, 50, 50, 2, 3, rgb1, rgb2, false);// ������ �����
	glPopMatrix();

}
void mant(double rgb1[],double rgb2[]) {//������

	glTranslated(0.1, 0, 0);

	rgb1[0] = 1;
	rgb1[1] = 0.187;
	rgb1[2] = 0.1;

	rgb2[0] = 0.9;
	rgb2[1] = 0.1;
	rgb2[2] = 0.05;
	sphere(4.8, 50, 50, 1, 2, rgb1, rgb2, true);//������� ������
	round(4.8, 360, 1, rgb1, rgb2);
	glPushMatrix();

	glRotated(70, 0, 0, 1);
	round(4.8, 360, 1, rgb1, rgb2);//���������� � ������� ������
	glRotated(94, 0, 0, 1);
	sphere(4.8, 50, 50, 2, 4, rgb1, rgb2,true);//������� ������ 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(0,0,-0.03);
	round(4.8, 360, 3, rgb1, rgb2);//���������� 
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 0.03);
	glRotated(-90, 0, 1, 0);
	sphere(4.8, 50, 50, 1, 2, rgb1, rgb2, true);//����� ������ �����
	glPopMatrix();

}
void magm(double rgb1[], double rgb2[]) {//�����
	glTranslated(0.2, 0.2, 0.2);

	rgb2[0] = 1;
	rgb2[1] = 1;
	rgb2[2] = 0.2;

	rgb1[0] = 0.435;
	rgb1[1] = 0.592;
	rgb1[2] = 0.235;
	sphere(4.3, 50, 50, 1, 2, rgb1, rgb2, true);//�����
	round(4.3, 360, 1, rgb1, rgb2);
	glPushMatrix();

	glRotated(80, 0, 0, 1);
	round(4.3, 360, 1, rgb1, rgb2);//���������� � ������� �����
	glRotated(94, 0, 0, 1);
	sphere(4.3, 50, 50, 2, 4, rgb1, rgb2, true);//�����
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 0, 1, 0);
	sphere(4.3, 50, 50, 1, 2, rgb1, rgb2, true);//����� ������ �����
	round(4.3, 360, 1, rgb1, rgb2);//���������� � ����� ������
	glPopMatrix();

}
void outer_core(double rgb1[], double rgb2[]) {//������� ����

	rgb1[0] = 1;
	rgb1[1] = 1;
	rgb1[2] = 0;

	rgb2[0] = 1;
	rgb2[1] = 0.5;
	rgb2[2] = 0.05;

	glTranslated(0.2, 0.2, 0.2);
	sphere(3.2, 50, 50, 1, 2, rgb1, rgb2, true);
	round(3.2, 360, 1, rgb1, rgb2);
	glPushMatrix();

	glRotated(80, 0, 0, 1);
	round(3.2, 360, 1, rgb1, rgb2);
	glRotated(94, 0, 0, 1);
	sphere(3.2, 50, 50, 2, 4, rgb1, rgb2, true);
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 0, 1, 0);
	sphere(3.2, 50, 50, 1, 2, rgb1, rgb2, true);// ������ �����
	round(3.2, 360, 1, rgb1, rgb2);//���������� �  ������
	glPopMatrix();
}
void core(double rgb1[], double rgb2[]) //����
{

	rgb1[0] = 1;
	rgb1[1] = 0;
	rgb1[2] = 0;

	rgb2[0] = 1;
	rgb2[1] = 0.5;
	rgb2[2] = 0.05;

	glTranslated(0.2, 0.2, 0.2);
	sphere(2, 50, 50, 1, 1, rgb1, rgb2, true);
}
void polocr() {
	double x=0, y=0, z=0;
	int r1 = 5, r2 = 2;
	glBegin(GL_QUAD_STRIP);

	glColor3d(1,1,0);
	for (int i = 0; i <116; i++) {
		glNormal3d(0, 0, -1);

		x = r1*cos(i*M_PI / 180);
		y = r1*sin(i*M_PI / 180);
		glVertex3d(x, y, z);

		x = r2*cos(i*M_PI / 180);
		y = r2*sin(i*M_PI / 180);
		glVertex3d(x, y, z);
	}


	glEnd();
}
void polocr1() {
	double x = 0, y = 0, z = 0;
	int r1 = 5, r2 = 2;
	glBegin(GL_QUAD_STRIP);

	glColor3d(1, 1, 0);
	for (int i = 0; i <90; i++) {
		glNormal3d(-1, 0, 0);

		y = r1*cos(i*M_PI / 180);
		z = r1*sin(i*M_PI / 180);
		glVertex3d(x, y, z);

		y = r2*cos(i*M_PI / 180);
		z = r2*sin(i*M_PI / 180);
		glVertex3d(x, y, z);
	}


	glEnd();
}
//���������� �����
void cut() {

	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };

	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 1;	rgb1[1] = 0;	rgb1[2] = 0;	rgb2[0] = 1;	rgb2[1] = 0.5;	rgb2[2] = 0.05;
	//glTranslated(0, movement*0.02, movement*0.02);
	glTranslated(-movement*0.029, 0, movement*0.02);
	sphere(2, 50, 50, 2, 3, rgb1, rgb2, true);//����
	glPopMatrix();

	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 1;	rgb1[1] = 1;	rgb1[2] = 0;	rgb2[0] = 1;	rgb2[1] = 0.5;	rgb2[2] = 0.05;
	glTranslated(-movement*0.03, 0, movement*0.021);
	sphere(3.2, 50, 50, 2, 3, rgb1, rgb2, true);//������� ����
	glPopMatrix();

	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb2[0] = 1;	rgb2[1] = 1;	rgb2[2] = 0.2;	rgb1[0] = 0.435;	rgb1[1] = 0.592;	rgb1[2] = 0.235;
	glTranslated(-movement*0.032, 0, movement*0.022);
	sphere(4.3, 50, 50, 2, 3, rgb1, rgb2, true);//�����
	glPopMatrix();

	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 1;
	rgb1[1] = 0.187;
	rgb1[2] = 0.1;

	rgb2[0] = 0.9;
	rgb2[1] = 0.1;
	rgb2[2] = 0.05;
	glTranslated(-movement*0.035, 0, movement*0.025);
	sphere(4.8, 50, 50, 2, 3, rgb1, rgb2, true);//������� ������
	glPopMatrix();

	glPushMatrix();
	glRotated(-115, 0, 0, 1); 
	rgb1[0] = 0.1;
	rgb1[1] = 0.5;
	rgb1[2] = 0.7;

	rgb2[0] = 0.1;
	rgb2[1] = 0.3;
	rgb2[2] = 1;
	glTranslated(-movement*0.04, 0, movement*0.03);
	sphere(5, 50, 50, 2, 3, rgb1, rgb2, true);//�������

	if (movement == 250)flag = false;
	if (movement == 0) {
		if (time < 10) { time++; }
		else {
			flag = true;
			time = 0;
		}
	}
	if (flag == true)movement++;
	else if (movement == 0);
	else movement--;


	glPopMatrix();


}
void planet_without_piece() {


	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };
	rgb1[0] = 0.1;
	rgb1[1] = 0.5;
	rgb1[2] = 0.7;

	rgb2[0] = 0.1;
	rgb2[1] = 0.3;
	rgb2[2] = 1;

	glRotated(90, 0, 0, 1);
	litosphere(rgb1, rgb2);
	mant(rgb1, rgb2);//������� ������
	magm(rgb1, rgb2);//�����
	outer_core(rgb1, rgb2);//������� ����
	core(rgb1, rgb2);//���������� ����



}
void earth_without() {
	
	////������� � �������
	//// !!!!!!!!!�������� ��������,���� ��������

	glPushMatrix();
	lightMode = false;
	planet_without_piece();//������� ��� �������
	glTranslated(-0.7,-0.6,-0.58);
	glPushMatrix();
	cut();//������� ,��� ���� ��������� ��� ������� ��� �������� �� �������
	//////������� � ���������� �������
	////

	glPopMatrix();
	glPopMatrix();
}

int memowem = 0;
void merc_without() {

	glPushMatrix();
	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };
	glRotated(90, 0, 0, 1);


	rgb1[0] = 0.02;	rgb1[1] = 0.01;	rgb1[2] = 0.02;
	glBindTexture(GL_TEXTURE_2D, texId[3]);
	sphere(3, 50, 50, 1, 2, rgb1, rgb1, true);//���������
	glPushMatrix();
	glRotated(160, 0, 0, 1);
	sphere(3, 50, 50, 1, 4, rgb1, rgb1, true);//���������
	glRotated(-4, 0, 0, 1);
	round(2.99, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-270, 0, 0, 1);
	round(2.99, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	round(2.99, 360, 3, rgb1, rgb1);//���������� 
	glPopMatrix();
	//round(5, 360, 2, rgb1, rgb2);//����������  ������
	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	sphere(3, 50, 50, 2, 3, rgb1, rgb1, false);// ������ �����
	glPopMatrix();




	glBindTexture(GL_TEXTURE_2D, texId[5]);
	glTranslated(0.1, 0, 0);

	rgb1[0] = 0.8;
	rgb1[1] = 0.1;
	rgb1[2] = 0;

	rgb2[0] = 1;
	rgb2[1] = 0.08;
	rgb2[2] = 0.05;
	sphere(2.8, 50, 50, 1, 2, rgb1, rgb1, true);//������� ������
	round(2.79, 360, 1, rgb2, rgb1);
	glPushMatrix();

	glRotated(70, 0, 0, 1);
	round(2.79, 360, 1, rgb2, rgb1);//���������� � ������� ������
	glRotated(94, 0, 0, 1);
	sphere(2.8, 50, 50, 2, 4, rgb1, rgb1, true);//������� ������ 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(0, 0, -0.03);
	round(2.79, 360, 3, rgb2, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 0.03);
	glRotated(-90, 0, 1, 0);
	sphere(2.8, 50, 50, 1, 2, rgb1, rgb1, true);//����� ������ �����
	glPopMatrix();




	rgb1[0] = 0.9;
	rgb1[1] = 0.8;
	rgb1[2] = 0.08;

	glBindTexture(GL_TEXTURE_2D, texId[2]);

	glTranslated(0.2, 0.2, 0.2);
	sphere(2.2, 50, 50, 1, 1, rgb1, rgb1, true);



	glTranslated(-0.3, -0.2, -0.2);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[2]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.9;	rgb1[1] = 0.8;	rgb1[2] = 0.08;
	glTranslated(-memowem*0.029, 0, memowem*0.02);
	sphere(2.2, 50, 50, 2, 3, rgb1, rgb1, true);//����
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[5]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.8;	rgb1[1] = 0.1;	rgb1[2] = 0;
	glTranslated(-memowem*0.035, 0, memowem*0.025);
	sphere(2.8, 50, 50, 2, 3, rgb1, rgb1, true);//������� ������
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[3]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.02;	rgb1[1] = 0.01;	rgb1[2] = 0.02;
	glTranslated(-memowem*0.04, 0, memowem*0.03);
	sphere(3, 50, 50, 2, 3, rgb1, rgb1, true);//�������

	if (memowem == 250)flag = false;
	if (memowem == 0) {
		if (time < 10) { time++; }
		else {
			flag = true;
			time = 0;
		}
	}
	if (flag == true)memowem++;
	else if (memowem == 0);
	else memowem--;

	glPopMatrix();

	glPopMatrix();
	glPopMatrix();


}
int movementmarce = 0;
void marce_without() {

	glPushMatrix();
	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };
	rgb1[0] = 0.1;
	rgb1[1] = 0.5;
	rgb1[2] = 0.7;

	rgb2[0] = 0.1;
	rgb2[1] = 0.3;
	rgb2[2] = 1;
	glRotated(90, 0, 0, 1);


	rgb1[0] = 0.9;
	rgb1[1] = 0.3;
	rgb1[2] = 0.02;
	glBindTexture(GL_TEXTURE_2D, texId[2]);
	sphere(5.2, 50, 50, 1, 2, rgb1, rgb1, true);//���������
	glPushMatrix();
	glRotated(160, 0, 0, 1);
	sphere(5.2, 50, 50, 1, 4, rgb1, rgb1, true);//���������
	glRotated(-4, 0, 0, 1);
	round(5.19, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-270, 0, 0, 1);
	round(5.19, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	round(5.19, 360, 3, rgb1, rgb1);//���������� 
	glPopMatrix();
	//round(5, 360, 2, rgb1, rgb2);//����������  ������
	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	sphere(5.2, 50, 50, 2, 3, rgb1, rgb1, false);// ������ �����
	glPopMatrix();




	glBindTexture(GL_TEXTURE_2D, texId[4]);
	glTranslated(0.1, 0, 0);

	rgb1[0] = 0.8;
	rgb1[1] = 0.7;
	rgb1[2] = 0;

	rgb2[0] = 1;
	rgb2[1] = 0.3;
	rgb2[2] = 0.05;
	sphere(4.8, 50, 50, 1, 2, rgb1, rgb1, true);//������� ������
	round(4.8, 360, 1, rgb2, rgb1);
	glPushMatrix();

	glRotated(70, 0, 0, 1);
	round(4.8, 360, 1, rgb2, rgb1);//���������� � ������� ������
	glRotated(94, 0, 0, 1);
	sphere(4.8, 50, 50, 2, 4, rgb1, rgb1, true);//������� ������ 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(0, 0, -0.03);
	round(4.8, 360, 3, rgb2, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 0.03);
	glRotated(-90, 0, 1, 0);
	sphere(4.8, 50, 50, 1, 2, rgb1, rgb1, true);//����� ������ �����
	glPopMatrix();




	rgb1[0] = 0.9;
	rgb1[1] = 0.05;
	rgb1[2] = 0.08;

	glBindTexture(GL_TEXTURE_2D, texId[5]);

	glTranslated(0.2, 0.2, 0.2);
	sphere(2, 50, 50, 1, 1, rgb1, rgb1, true);



	glTranslated(-0.3, -0.2, -0.2);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[5]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.9;	rgb1[1] = 0.05;	rgb1[2] = 0.08;
	glTranslated(-movementmarce*0.029, 0, movementmarce*0.02);
	sphere(2, 50, 50, 2, 3, rgb1, rgb1, true);//����
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[4]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.8;	rgb1[1] = 0.7;	rgb1[2] = 0;
	glTranslated(-movementmarce*0.035, 0, movementmarce*0.025);
	sphere(4.8, 50, 50, 2, 3, rgb1, rgb1, true);//������� ������
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[2]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.9;	rgb1[1] = 0.3;	rgb1[2] = 0.02;
	glTranslated(-movementmarce*0.04, 0, movementmarce*0.03);
	sphere(5.2, 50, 50, 2, 3, rgb1, rgb1, true);//�������

	if (movementmarce == 250)flag = false;
	if (movementmarce == 0) {
		if (time < 10) { time++; }
		else {
			flag = true;
			time = 0;
		}
	}
	if (flag == true)movementmarce++;
	else if (movementmarce == 0);
	else movementmarce--;

	glPopMatrix();

	glPopMatrix();
	glPopMatrix();
}
int venoove = 0;
void ven_without() {

	glPushMatrix();
	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };
	glRotated(90, 0, 0, 1);


	rgb1[0] = 0.7882352941176471;	rgb1[1] = 0.392156862745098;	rgb1[2] = 0.2313725490196078;
	glBindTexture(GL_TEXTURE_2D, texId[4]);
	sphere(4.5, 50, 50, 1, 2, rgb1, rgb1, true);//���������
	glPushMatrix();
	glRotated(160, 0, 0, 1);
	sphere(4.5, 50, 50, 1, 4, rgb1, rgb1, true);//���������
	glRotated(-4, 0, 0, 1);
	round(4.49, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-270, 0, 0, 1);
	round(4.49, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	round(4.49, 360, 3, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	sphere(4.5, 50, 50, 2, 3, rgb1, rgb1, false);// ������ �����
	glPopMatrix();






	rgb1[0] = 0.9607843137254902;
	rgb1[1] = 0.9607843137254902;
	rgb1[2] = 0.2627450980392157;

	glBindTexture(GL_TEXTURE_2D, texId[2]);

	glTranslated(0.2, 0.2, 0.2);
	sphere(1.8, 50, 50, 1, 1, rgb1, rgb1, true);



	glTranslated(-0.3, -0.2, -0.2);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[2]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.9607843137254902;	rgb1[1] = 0.9607843137254902;	rgb1[2] = 0.2627450980392157;
	glTranslated(-venoove*0.029, 0, venoove*0.02);
	sphere(1.8, 50, 50, 2, 3, rgb1, rgb1, true);//����
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[4]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.7882352941176471;	rgb1[1] = 0.392156862745098;	rgb1[2] = 0.2313725490196078;
	glTranslated(-venoove*0.04, 0, venoove*0.03);
	sphere(4.5, 50, 50, 2, 3, rgb1, rgb1, true);//�������

	if (venoove == 250)flag = false;
	if (venoove == 0) {
		if (time < 10) { time++; }
		else {
			flag = true;
			time = 0;
		}
	}
	if (flag == true)venoove++;
	else if (venoove == 0);
	else venoove--;

	glPopMatrix();

	glPopMatrix();
	glPopMatrix();


}
int jup = 0;
void jup_without() {



	glPushMatrix();
	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };
	glRotated(90, 0, 0, 1);


	rgb1[0] = 0.662745;
	rgb1[1] = 0.662745;
	rgb1[2] = 0.662745;
	glBindTexture(GL_TEXTURE_2D, texId[6]);
	sphere(10, 50, 50, 1, 2, rgb1, rgb1, true);//���������
	glPushMatrix();
	glRotated(160, 0, 0, 1);
	sphere(10, 50, 50, 1, 4, rgb1, rgb1, true);//���������
	glRotated(-4, 0, 0, 1);
	round(9.99, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-270, 0, 0, 1);
	round(9.99, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	round(9.99, 360, 3, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	sphere(10, 50, 50, 2, 3, rgb1, rgb1, false);// ������ �����
	glPopMatrix();




	glBindTexture(GL_TEXTURE_2D, texId[9]);
	glTranslated(0.1, 0, 0);

	rgb1[0] = 0.0588235294117647;
	rgb1[1] = 0.3215686274509804;
	rgb1[2] = 0.7294117647058824;
	sphere(9, 50, 50, 1, 2, rgb1, rgb1, true);//������� ������
	round(9, 360, 1, rgb2, rgb1);
	glPushMatrix();

	glRotated(70, 0, 0, 1);
	round(9, 360, 1, rgb2, rgb1);//���������� � ������� ������
	glRotated(94, 0, 0, 1);
	sphere(9, 50, 50, 2, 4, rgb1, rgb1, true);//������� ������ 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(0, 0, -0.03);
	round(9, 360, 3, rgb2, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 0.03);
	glRotated(-90, 0, 1, 0);
	sphere(9, 50, 50, 1, 2, rgb1, rgb1, true);
	glPopMatrix();





	rgb1[0] = 0.270588235;	rgb1[1] = 0.270588235;	rgb1[2] = 0.270588235;
	glBindTexture(GL_TEXTURE_2D, texId[3]);

	glTranslated(0.2, 0.2, 0.2);
	sphere(2, 50, 50, 1, 1, rgb1, rgb1, true);



	glTranslated(-0.3, -0.2, -0.2);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[3]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.270588235;	rgb1[1] = 0.270588235;	rgb1[2] = 0.270588235;
	glTranslated(-jup*0.029, 0, jup*0.02);
	sphere(2, 50, 50, 2, 3, rgb1, rgb1, true);//����
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[9]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.0588235294117647;	rgb1[1] = 0.3215686274509804;	rgb1[2] = 0.7294117647058824;
	glTranslated(-jup*0.035, 0, jup*0.025);
	sphere(9, 50, 50, 2, 3, rgb1, rgb1, true);//������� ������
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[6]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.662745;	rgb1[1] = 0.662745;	rgb1[2] = 0.662745;
	glTranslated(-jup*0.04, 0, jup*0.03);
	sphere(10, 50, 50, 2, 3, rgb1, rgb1, true);//�������

	if (jup == 250)flag = false;
	if (jup == 0) {
		if (time < 10) { time++; }
		else {
			flag = true;
			time = 0;
		}
	}
	if (flag == true)jup++;
	else if (jup == 0);
	else jup--;

	glPopMatrix();

	glPopMatrix();
	glPopMatrix();


}
int sut = 0;
void sut_without() {

	glPushMatrix();
	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };
	glRotated(90, 0, 0, 1);


	rgb1[0] = 0.7843137254901961;
	rgb1[1] = 0.6352941176470588;
	rgb1[2] = 0.7843137254901961;
	glBindTexture(GL_TEXTURE_2D, texId[7]);
	sphere(7, 50, 50, 1, 2, rgb1, rgb1, true);//���������
	glPushMatrix();
	glRotated(160, 0, 0, 1);
	sphere(7, 50, 50, 1, 4, rgb1, rgb1, true);//���������
	glRotated(-4, 0, 0, 1);
	round(6.99, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-270, 0, 0, 1);
	round(6.99, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	round(6.99, 360, 3, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	sphere(7, 50, 50, 2, 3, rgb1, rgb1, false);// ������ �����
	glPopMatrix();




	glBindTexture(GL_TEXTURE_2D, texId[8]);
	glTranslated(0.1, 0, 0);

	rgb1[0] = 0.4274509803921569;
	rgb1[1] = 0.2862745098039216;
	rgb1[2] = 0.5686274509803922;
	sphere(5, 50, 50, 1, 2, rgb1, rgb1, true);//������� ������
	round(5, 360, 1, rgb2, rgb1);
	glPushMatrix();

	glRotated(70, 0, 0, 1);
	round(5, 360, 1, rgb2, rgb1);//���������� � ������� ������
	glRotated(94, 0, 0, 1);
	sphere(5, 50, 50, 2, 4, rgb1, rgb1, true);//������� ������ 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(0, 0, -0.03);
	round(5, 360, 3, rgb2, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 0.03);
	glRotated(-90, 0, 1, 0);
	sphere(5, 50, 50, 1, 2, rgb1, rgb1, true);
	glPopMatrix();





	rgb1[0] = 1;	rgb1[1] = 0.6509803921568627;	rgb1[2] = 0.192156862745098;
	glBindTexture(GL_TEXTURE_2D, texId[4]);

	glTranslated(0.2, 0.2, 0.2);
	sphere(3, 50, 50, 1, 1, rgb1, rgb1, true);



	glTranslated(-0.3, -0.2, -0.2);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[4]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	glTranslated(-sut*0.029, 0, sut*0.02);
	sphere(3, 50, 50, 2, 3, rgb1, rgb1, true);//����
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[8]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.4274509803921569;	rgb1[1] = 0.2862745098039216;	rgb1[2] = 0.5686274509803922;
	glTranslated(-sut*0.035, 0, sut*0.025);
	sphere(5, 50, 50, 2, 3, rgb1, rgb1, true);//������� ������
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[7]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.7843137254901961;	rgb1[1] = 0.6352941176470588;	rgb1[2] = 0.7843137254901961;
	glTranslated(-sut*0.04, 0, sut*0.03);
	sphere(7, 50, 50, 2, 3, rgb1, rgb1, true);//�������

	if (sut == 250)flag = false;
	if (sut == 0) {
		if (time < 10) { time++; }
		else {
			flag = true;
			time = 0;
		}
	}
	if (flag == true)sut++;
	else if (sut == 0);
	else sut--;

	glPopMatrix();

	glPopMatrix();
	glPopMatrix();

}
int ur = 0;
void ur_without() {

	glPushMatrix();
	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };
	glRotated(90, 0, 0, 1);


	rgb1[0] = 0;
	rgb1[1] = 0.749;
	rgb1[2] = 1;
	glBindTexture(GL_TEXTURE_2D, texId[8]);
	sphere(6.5, 50, 50, 1, 2, rgb1, rgb1, true);//���������
	glPushMatrix();
	glRotated(160, 0, 0, 1);
	sphere(6.5, 50, 50, 1, 4, rgb1, rgb1, true);//���������
	glRotated(-4, 0, 0, 1);
	round(6.49, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-270, 0, 0, 1);
	round(6.49, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	round(6.49, 360, 3, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	sphere(6.5, 50, 50, 2, 3, rgb1, rgb1, false);// ������ �����
	glPopMatrix();




	glBindTexture(GL_TEXTURE_2D, texId[3]);
	glTranslated(0.1, 0, 0);

	rgb1[0] = 0;
	rgb1[1] = 1;
	rgb1[2] = 1;
	sphere(6, 50, 50, 1, 2, rgb1, rgb1, true);//������� ������
	round(6, 360, 1, rgb2, rgb1);
	glPushMatrix();

	glRotated(70, 0, 0, 1);
	round(6, 360, 1, rgb2, rgb1);//���������� � ������� ������
	glRotated(94, 0, 0, 1);
	sphere(6, 50, 50, 2, 4, rgb1, rgb1, true);//������� ������ 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(0, 0, -0.03);
	round(6, 360, 3, rgb2, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 0.03);
	glRotated(-90, 0, 1, 0);
	sphere(6, 50, 50, 1, 2, rgb1, rgb1, true);
	glPopMatrix();





	rgb1[0] = 0;	rgb1[1] = 0.50196;	rgb1[2] = 1;
	glBindTexture(GL_TEXTURE_2D, texId[9]);

	glTranslated(0.2, 0.2, 0.2);
	sphere(2, 50, 50, 1, 1, rgb1, rgb1, true);



	glTranslated(-0.3, -0.2, -0.2);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[9]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	glTranslated(-ur*0.029, 0, ur*0.02);
	sphere(2, 50, 50, 2, 3, rgb1, rgb1, true);//����
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[3]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0;	rgb1[1] = 1;	rgb1[2] = 1;
	glTranslated(-ur*0.035, 0, ur*0.025);
	sphere(6, 50, 50, 2, 3, rgb1, rgb1, true);//������� ������
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[8]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0;	rgb1[1] = 0.749;	rgb1[2] = 1;
	glTranslated(-ur*0.04, 0, ur*0.03);
	sphere(6.5, 50, 50, 2, 3, rgb1, rgb1, true);//�������

	if (ur == 250)flag = false;
	if (ur == 0) {
		if (time < 10) { time++; }
		else {
			flag = true;
			time = 0;
		}
	}
	if (flag == true)ur++;
	else if (ur == 0);
	else ur--;

	glPopMatrix();

	glPopMatrix();
	glPopMatrix();

}
int nept = 0;
void nept_without() {


	glPushMatrix();
	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };
	glRotated(90, 0, 0, 1);

	rgb1[0] = 0.45098;	rgb1[1] = 0.45098;	rgb1[2] = 0.6;
	glBindTexture(GL_TEXTURE_2D, texId[9]);
	sphere(7, 50, 50, 1, 2, rgb1, rgb1, true);//���������
	glPushMatrix();
	glRotated(160, 0, 0, 1);
	sphere(7, 50, 50, 1, 4, rgb1, rgb1, true);//���������
	glRotated(-4, 0, 0, 1);
	round(6.99, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-270, 0, 0, 1);
	round(6.99, 360, 2, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	round(6.99, 360, 3, rgb1, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glRotated(-180, 0, 1, 0);
	sphere(7, 50, 50, 2, 3, rgb1, rgb1, false);// ������ �����
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[8]);
	glTranslated(0.1, 0, 0);

	rgb1[0] = 0.87;	rgb1[1] = 0.847;	rgb1[2] = 0.890196;
	sphere(5, 50, 50, 1, 2, rgb1, rgb1, true);//������� ������
	round(5, 360, 1, rgb2, rgb1);
	glPushMatrix();

	glRotated(70, 0, 0, 1);
	round(5, 360, 1, rgb2, rgb1);//���������� � ������� ������
	glRotated(94, 0, 0, 1);
	sphere(5, 50, 50, 2, 4, rgb1, rgb1, true);//������� ������ 
	glPopMatrix();
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(0, 0, -0.03);
	round(5, 360, 3, rgb2, rgb1);//���������� 
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 0.03);
	glRotated(-90, 0, 1, 0);
	sphere(5, 50, 50, 1, 2, rgb1, rgb1, true);
	glPopMatrix();



	rgb1[0] = 0.396;	rgb1[1] = 0.262745;	rgb1[2] = 0.1294117647;
	glBindTexture(GL_TEXTURE_2D, texId[2]);

	glTranslated(0.2, 0.2, 0.2);
	sphere(0.5, 50, 50, 1, 1, rgb1, rgb1, true);


	glTranslated(-0.3, -0.2, -0.2);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[2]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	glTranslated(-nept*0.029, 0, nept*0.02);
	sphere(0.5, 50, 50, 2, 3, rgb1, rgb1, true);//����
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[8]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.87;	rgb1[1] = 0.847;	rgb1[2] = 0.890196;
	glTranslated(-nept*0.035, 0, nept*0.025);
	sphere(5, 50, 50, 2, 3, rgb1, rgb1, true);//������� ������
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[9]);
	glPushMatrix();
	glRotated(-115, 0, 0, 1);
	rgb1[0] = 0.45098;	rgb1[1] = 0.45098;	rgb1[2] = 0.6;
	glTranslated(-nept*0.04, 0, nept*0.03);
	sphere(7, 50, 50, 2, 3, rgb1, rgb1, true);//�������

	if (nept == 250)flag = false;
	if (nept == 0) {
		if (time < 10) { time++; }
		else {
			flag = true;
			time = 0;
		}
	}
	if (flag == true)nept++;
	else if (nept == 0);
	else nept--;

	glPopMatrix();

	glPopMatrix();
	glPopMatrix();

}
//������
void cube(double l) {
	//��������� ���������
	GLfloat amb[] = { 0, 0, 0.3, 1. };
	GLfloat dif[] = { 0, 0, 0.3, 1. };
	GLfloat spec[] = { 0, 0, 0.3, 1. };
	GLfloat sh = 0;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);


	glBindTexture(GL_TEXTURE_2D, texId[0]);
	glBegin(GL_QUADS);


	glColor3d(0.1, 0.2, 0.7);
	glNormal3d(-1,0,0);
	glTexCoord2d(1,1);
	glVertex3d(l, l, l);//�������� ������ �� ����
	glTexCoord2d(0, 1);
	glVertex3d(l, l, -l);
	glTexCoord2d(0, 0);
	glVertex3d(l, -l, -l);
	glTexCoord2d(1, 0);
	glVertex3d(l, -l, l);

	glColor3d(0.1, 0.2, 0.7);
	glNormal3d(1, 0, 0);
	glTexCoord2d(0, 0);
	glVertex3d(-l, l, l);//������ ������ �� ����
	glTexCoord2d(1, 0);
	glVertex3d(-l, l, -l);
	glTexCoord2d(1, 1);
	glVertex3d(-l, -l, -l);
	glTexCoord2d(0, 1);
	glVertex3d(-l, -l, l);

	glColor3d(0.1, 0.2, 0.7);
	glNormal3d(0, -1, 0);
	glTexCoord2d(0, 0);
	glVertex3d(-l, l, -l);//�������� ������ �� ������
	glTexCoord2d(1, 0);
	glVertex3d(l, l, -l);
	glTexCoord2d(1, 1);
	glVertex3d(l, l, l);
	glTexCoord2d(0, 1);
	glVertex3d(-l, l, l);


	glColor3d(0.5, 0.3, 0.9);
	glNormal3d(0, 1, 0);
	glTexCoord2d(1, 1);
	glVertex3d(-l, -l, -l);//������ ������ �� ������
	glTexCoord2d(0, 1);
	glVertex3d(l, -l, -l);
	glTexCoord2d(0,0);
	glVertex3d(l, -l, l);
	glTexCoord2d(1, 0);
	glVertex3d(-l, -l, l);

	glColor3d(0.5, 0.3, 0.9);
	glNormal3d(0, 0, -1);
	glTexCoord2d(1, 0);
	glVertex3d(-l, l, l);//�������� ������ �� ���
	glTexCoord2d(1, 1);
	glVertex3d(l, l, l);
	glTexCoord2d(0,1);
	glVertex3d(l, -l, l);
	glTexCoord2d(0, 0);
	glVertex3d(-l, -l, l);

	glColor3d(0.5, 0.3, 0.9);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 1);
	glVertex3d(-l, l, -l);//������ ������ �� ���
	glTexCoord2d(0, 0);
	glVertex3d(l, l, -l);
	glTexCoord2d(1, 0);
	glVertex3d(l, -l, -l);
	glTexCoord2d(1, 1);
	glVertex3d(-l, -l, -l);



	glEnd();


}
void cosmos(float x, float y, float z, float r,double rgb[]) {


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.7, 1. };
	GLfloat dif[] = { 0.4, 0.2, 0.8, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);


	glBindTexture(GL_TEXTURE_2D, texId[0]);
	float a = 1, a2 = 1, mb = 1;
	float y2 = 1024 - cos(a)*cos(a2) * 100 / 1024.0;
	glColor3f(rgb[0], rgb[1], rgb[2]);
	glBegin(GL_QUADS);
	for (int i2 = 0; i2 < 26; i2++) {
		if (i2 > 0) mb *= -1;;
		a2 = (i2*M_PI * 2) / 26;
		for (int i = 0; i < 13; i++) {
			a = (i * M_PI * 2) / 26;
			glTexCoord2d(cos(a)*cos(a2), sin(a2));
			glNormal3f(-cos(a)*cos(a2), -sin(a2), -sin(a)*cos(a2));
			glVertex3f(r*cos(a)*cos(a2) + x, r*sin(a2) + y, r*sin(a)*cos(a2) + z);
			i++;
			a = (i * M_PI * 2) / 26;
			glTexCoord2d(cos(a)*cos(a2), sin(a2));
			glVertex3f(r*cos(a)*cos(a2) + x, r*sin(a2) + y, r*sin(a)*cos(a2) + z);
			i2++;
			a2 = (i2*M_PI * 2) / 26;
			glTexCoord2d(cos(a)*cos(a2), sin(a2));
			glVertex3f(r*cos(a)*cos(a2) + x, r*sin(a2) + y, r*sin(a)*cos(a2) + z);
			i--;
			a = (i * M_PI * 2) / 26;
			glTexCoord2d(cos(a)*cos(a2), sin(a2));
			glVertex3f(r*cos(a)*cos(a2) + x, r*sin(a2) + y, r*sin(a)*cos(a2) + z);
			i2--;
			a2 = (i2*M_PI * 2) / 26;
		}
	}
	glEnd();
}



int sunmove = 0;
void sun() {


	//��������� ���������
	GLfloat amb[] = { 1,0.9, 0.9, 1. };
	GLfloat dif[] = { 1, 0.9, 0.9, 1. };
	GLfloat spec[] = {1, 1,1, 1. };
	GLfloat sh = 0;

	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	glRotated(sunmove*0.2,0,1,0);
	sunmove++;
	double rgb1[] = { 0.97647,0.80784,0.6 };
	double rgb2[] = {0.97647,0.80784,0.6};
	//planet(15,50,50,1,1,rgb2,rgb2,true);
	double t1 = 0, t2 = 0;
	bool f = false;
	double s1 = 80, s2 = 80, m = 1, l = 1,R=30;
	float x1, y1, z1, x2, y2, z2;
	float a = 1, a2 = 1;
	glBindTexture(GL_TEXTURE_2D, texId[2]);
	glColor3dv(rgb1);
	//glColor3d(1,1,1);
	for (int i = 0; i < s1 / m; i++)//��� �� �������� ����� ����� s1 �� 2 ���� ���� ���������,�� 3 ���� ���� ����� � � � 
	{
		t1 = 0;
		t2 += 1024.0 / s1;
		a2 = (i*M_PI * 2) / (s1 / m);
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j <= s2 / l; j++)
		{//��� �� �������� ����� ����� s2 �� 2 ���� ���� ���������,�� 3 ���� ���� ����� � � � 

			x1 = R * sin(-2 * M_PI * j / s2) * sin(M_PI * i / s1);
			x2 = R * sin(-2 * M_PI * j / s2) * sin(M_PI * (i + 1) / s1);
			y1 = R * cos(-2 * M_PI * j / s2) * sin(M_PI * i / s1);
			y2 = R * cos(-2 * M_PI * j / s2) * sin(M_PI * (i + 1) / s1);
			z1 = R * cos(M_PI * i / s1);
			z2 = R * cos(M_PI * (i + 1) / s1);
			if (f == true) {
				;
				a2 = (i*M_PI * 2) / (s1 / m);
			}
			else {
				a2 = ((i + 1)*M_PI * 2) / (s1 / m);
			}
			t1 += 1024.0 / s2;

			glNormal3d(-x1 / R * sin(M_PI * i / s1), -y1 / R * sin(M_PI * i / s1), -z1 / R);

			glTexCoord2d(t1 / 1024.0, t2 / 1024.0);
			//glTexCoord2d(x1 / 1024.0, y1 / 1024.0);
			glVertex3d(x1, y1, z1);


			if (f == true) {
				a2 = ((i + 1)*M_PI * 2) / (s1 / m);
			}
			else {
				a2 = (i*M_PI * 2) / (s1 / m);
			}
			glNormal3d(-x2 / R * sin(M_PI * (i + 1) / s1),-y2 / R * sin(M_PI * (i + 1) / s1), -z2 / R);

			glTexCoord2d(t1 / 1024.0, t2 / 1024.0);
			//glTexCoord2d(x2 / 1024.0, y2 / 1024.0);
			glVertex3d(x2, y2, z2);
		}
		glEnd();
	}

}
int mercmoove = 0;
double mercel = 0;
double po = 0;
int dir = -1;
void mercurii() {
	glBindTexture(GL_TEXTURE_2D, texId[3]);
	double rgb1[] = {0.75294,0.75294,0.75294 };
	glPushMatrix();
	if (camfl == 1)
	{
		vx = 60 * cos(mercmoove*0.7 * M_PI / 180.0);
		vy = 60 * sin(mercmoove*0.7 * M_PI / 180.0);
		vz = 2;
		if (ht == 1){
			camera.camDist = 60; 
			ht = 0;
			}
	}
	glRotated(mercmoove*0.7, 0, 0, 1);
	glTranslated(60, 0, 2);
	glRotated(po*0.7, 1, 0, 1);
	po++;
	glRotated(30, 1, 0, 1);

	if (camfl == 1 && cutP == true) {
		merc_without();
	}
	else {
		planet(3, 30, 30, 1, 1, rgb1, rgb1, false);
	}
	glPopMatrix();
	mercmoove++;
}
int venmoove = 0;
void venera() {
	glBindTexture(GL_TEXTURE_2D, texId[4]);
	double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	if (camfl == 2)
	{
		vx = 75 * cos((venmoove*0.3) * M_PI / 180.0);
		vy = 75 * sin((venmoove*0.3 )* M_PI / 180.0);
		vz =0;
		if (ht == 1) {
			camera.camDist = 60;
			ht = 0;
		}
	}
	glRotated(venmoove*0.3, 0, 0, 1);
	glTranslated(75, 0, 0);
	glRotated(-po*0.7, 0, 0, 1);
	if (camfl == 2 && cutP == true) {
		ven_without();
	}
	else {
		planet(4.5, 30, 30, 1, 1, rgb1, rgb1, true);
	}
	glPopMatrix();
	venmoove++;

}
int moonmove = 70;
void moon() {
	glBindTexture(GL_TEXTURE_2D, texId[3]);
	double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(moonmove*5, 0, 1, 0);
	glTranslated(10, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(1.2, 15, 15, 1, 1, rgb1, rgb1, true);
	glPopMatrix();
	moonmove++;

}
int earthmoove = 0;
void earth()//����� � �����
{
	glBindTexture(GL_TEXTURE_2D, texId[1]);
	double rgb1[] = { 0.1,0.9,0.9 };
	glPushMatrix();
	if (camfl ==3)
	{
		vx = 97 * cos(earthmoove*0.7 * M_PI / 180.0);
		vy = 97 * sin(earthmoove*0.7 * M_PI / 180.0);
		vz = 0;
		if (ht == 1) {
			if (camfl == 3 && cutP == true) {
				camera.camDist = 60;
			}
			else
				camera.camDist = 60;
			ht = 0;
		}
	}
	glRotated(earthmoove*0.7, 0, 0, 1);
	glTranslated(97, 0, 0);
	glRotated(po*0.7, 0, 0, 1);
	glRotated(15,1, 0, 0);
	if (camfl == 3 && cutP == true) {
		earth_without();
		
	}
	else {
		planet(5, 50, 50, 1, 1, rgb1, rgb1, true);
		moon();
	}
	glPopMatrix();
	earthmoove++;

}
int phobosmoove = 15;
void phobos()//������� ������� �����,�������� �������,��� �������� ������ ������ ���� ��������� ����� �������� � �����
{
	glBindTexture(GL_TEXTURE_2D, texId[8]);
	double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(-phobosmoove * 2, 0, 1, 0);
	glTranslated(9, 0, 0);
	glRotated(-45, 1, 0, 0);
	planet(1, 15, 15, 1, 1, rgb1, rgb1, true);
	glPopMatrix();
	phobosmoove++;

}
int deimosmoove = 40;
void deimos() //������� ������� �����,�������� ��������� ������
{
	glBindTexture(GL_TEXTURE_2D, texId[6]);
	double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(257, 1, 0, 0);
	glRotated(deimosmoove * 2, 0, 1, 0);
	glTranslated(13, 0, 0);
	glRotated(-65, 1, 0, 0);
	planet(1.3, 15, 15, 1, 1, rgb1, rgb1, true);
	glPopMatrix();
	deimosmoove++;

}
int marcmove = 13;
void marc()//���� ��� ��������
{	glBindTexture(GL_TEXTURE_2D, texId[5]);
	double rgb1[] = { 1,0,0.1 };
	glPushMatrix();
	if (camfl == 4)
	{
		vx = 127 * cos(-marcmove*0.8 * M_PI / 180.0);
		vy = 127 * sin(-marcmove*0.8 * M_PI / 180.0);
		vz = 0;
		if (ht == 1) {
			if (camfl == 4 && cutP == true) {
				camera.camDist = 60;
			}
			else
				camera.camDist = 60;
			ht = 0;
		}
	}
	glRotated(-marcmove*0.8, 0, 0, 1);
	glTranslated(127, 0, 0);
	glRotated(po*0.4, 0, 1, 1);

	if (camfl == 4 && cutP == true) {
		marce_without();
	}
	else {
		planet(5.2, 30, 30, 1, 1, rgb1, rgb1, true);
		phobos();
		deimos();
	}
	glPopMatrix();
	marcmove++;
}
int cermove = 170;
void cerera() {
	glBindTexture(GL_TEXTURE_2D, texId[4]);
	double rgb1[] = { 0.8,0.9,0.1 };
	glPushMatrix();
	if (camfl == 5)
	{
		vx = 145 * cos(cermove*0.7 * M_PI / 180.0);
		vy = 145 * sin(cermove*0.7 * M_PI / 180.0);
		vz = 0;
		if (ht == 1) {
			camera.camDist = 40;
			ht = 0;
		}
	}
	glRotated(cermove*0.7, 0, 0, 1);
	glTranslated(145, 0, 0);
	glRotated(po*0.4,1, 0, 1);
	glRotated(-35, 1, 0, 0);
	planet(1.2, 15, 15, 1, 1, rgb1, rgb1, true);
	glPopMatrix();
	cermove++;
}
int spmoove = 25;
void jupsp() //�������� ������� ����� �������
{
	glBindTexture(GL_TEXTURE_2D, texId[3]);//metida
	double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(-spmoove * 0.1, 0, 1, 0);
	glTranslated(11, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.2, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[8]);//adrastea
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(-spmoove * 0.8, 0, 1, 0);
	glTranslated(12, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.2, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[6]);//teba
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(spmoove * 2.3, 0, 1, 0);
	glTranslated(14, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.3, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[3]);//io
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(-spmoove * 1.2, 0, 1, 0);
	glTranslated(16, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.9, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[4]);//europ
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(spmoove * 0.9, 0, 1, 0);
	glTranslated(19, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.7, 9, 9, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[3]);//ganimed
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(spmoove * 3, 0, 1, 0);
	glTranslated(22, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(1, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[9]);//kalisto
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(-spmoove * 1.2, 0, 1, 0);
	glTranslated(26, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(1.2, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[7]);//leda
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(spmoove * 0.98, 0, 1, 0);
	glTranslated(28, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.2, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[9]);//gimalia
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(spmoove * 0.47, 0, 1, 0);
	glTranslated(29, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.3, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[5]);//karme
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(-spmoove * 2.2, 0, 1, 0);
	glTranslated(31, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.2, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[6]);//sinope
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(25, 1, 0, 0);
	glRotated(-spmoove * 1.0, 0, 1, 0);
	glTranslated(32, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.3, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	spmoove++;
}
int jupmove = 0;
void jupiter() {//������.....����� �������,����� ���������
	glBindTexture(GL_TEXTURE_2D, texId[6]);
	double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	if (camfl == 6)
	{
		vx = 180 * cos(jupmove*0.6 * M_PI / 180.0);
		vy = 180 * sin(jupmove*0.6 * M_PI / 180.0);
		vz = 0;
		if (ht == 1) {
			camera.camDist = 60;
			ht = 0;
		}
	}
	glRotated(jupmove*0.6, 0, 0, 1);
	glTranslated(180, 0, 0);
	glRotated(po*0.4, 1, 0, 1);
	glRotated(-15, 1, 0, 0);
	if (camfl == 6 && cutP == true) {
		jup_without();
	}
	else {
		planet(10, 40, 40, 1, 1, rgb1, rgb1, true);
		jupsp();
	}

	glPopMatrix();
	jupmove++;

}
int sutspmove = 0;
void sutsp() {
	glBindTexture(GL_TEXTURE_2D, texId[8]);//�����
	double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(15, 1, 0, 0);
	glRotated(-sutspmove * 0.9, 0, 1, 0);
	glTranslated(8, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.2, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[6]);//�����
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(17, 1, 0, 0);
	glRotated(sutspmove * 0.3, 0, 1, 0);
	glTranslated(9, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.2, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[4]);//����
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(19, 1, 0, 0);
	glRotated(sutspmove * 2.3, 0, 1, 0);
	glTranslated(11, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.3, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[3]);//�����
	 //double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(22, 1, 0, 0);
	glRotated(-sutspmove * 4, 0, 1, 0);
	glTranslated(13, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.9, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	sutspmove++;
}
void round1(float r, int s, int f, double rgb1[], double rgb2[])//������ � ������� ���������,���� 1 �=0,2 �=0,3 z=0
{
	glBindTexture(GL_TEXTURE_2D, texId[6]);
	float x = 0, y = 0, z = 0;
	glBegin(GL_TRIANGLE_FAN);

	switch (f) {
	case (1):
		glNormal3d(1, 0, 0);
		break;
	case (2):
		glNormal3d(0, -1, 0);
		break;
	case (3):
		glNormal3d(0, 0, -1);
		break;
	}

	glColor3dv(rgb1);
	glTexCoord2d(512.0/1024.0,0);
	glVertex3d(0, 0, 0);
	for (int i = 0; i <= s; i++) {
		switch (f) {
		case (1):	y = r*cos(i*M_PI / 180);
			z = r*sin(i*M_PI / 180);
			break;
		case (2):	x = r*cos(i*M_PI / 180);
			z = r*sin(i*M_PI / 180);
			break;
		case (3):	x = r*cos(i*M_PI / 180);
			y = r*sin(i*M_PI / 180);
			break;
		}

		glColor3dv(rgb2);
		glTexCoord2d((1024.0/360.0)*i/1024.0, 1);
		glVertex3d(x, y, z);

	}


	glEnd();


}
int satmove = 0;
void saturn() {
	glBindTexture(GL_TEXTURE_2D, texId[7]);
	double rgb1[] = { 0.2,0.598,0.70588 };
	glPushMatrix();
	if (camfl == 7)
	{
		vx = 240 * cos(satmove*0.4 * M_PI / 180.0);
		vy = 240 * sin(satmove*0.4 * M_PI / 180.0);
		vz = 0;
		if (ht == 1) {
			camera.camDist = 60;
			ht = 0;
		}
	}
	glRotated(satmove*0.4,0, 0, 1);
	glTranslated(240, 0, 0);
	glPushMatrix();
	glRotated(-30, 0,1, 0);
	glRotated(180, 1, 0, 0);
	if (camfl == 7 && cutP == true) {
		sut_without();
	}
	else {

		round1(11, 360, 3, rgb1, rgb1);//������...�������� ��������
		glTranslated(0, 0, 0.1);
		round1(14, 360, 3, rgb1, rgb1);//������...�������� ��������
		glTranslated(0, 0, 0.1);
		round1(12, 360, 3, rgb1, rgb1);//������...�������� ��������
		glTranslated(0, 0, 0.1);
		round1(14, 360, 3, rgb1, rgb1);//������...�������� ��������
		glPopMatrix();
		glRotated(po*0.7, 1, 0, 0);
		glRotated(48, 1, 0, 0);
		planet(7, 30, 30, 1, 1, rgb1, rgb1, true);
		sutsp();
	}

	
	glPopMatrix();
	satmove++;
}
int urspmove = 150;
void ursp() {
	double rgb1[] = { 0.960784,0.960784,0.862745 };

	glBindTexture(GL_TEXTURE_2D, texId[3]);//������(������� � ��������)
										   //double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(22, 1, 0, 0);
	glRotated(-urspmove * 4, 0, 1, 0);
	glTranslated(7.5, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.5, 9, 9, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[4]);//��������(������� � ������)
										   //double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(19, 1, 0, 0);
	glRotated(urspmove * 2.3, 0, 1, 0);
	glTranslated(9, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.5, 9, 9, 1, 1, rgb1, rgb1, true);
	glPopMatrix();


	glBindTexture(GL_TEXTURE_2D, texId[7]);//�������(���� �� ����� ������� � ��������)
	
	glPushMatrix();
	glRotated(15, 1, 0, 0);
	glRotated(-urspmove * 0.9, 0, 1, 0);
	glTranslated(10.5, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.8, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[9]);//������� (���� �� ����� ������� � ��������)
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(17, 1, 0, 0);
	glRotated(urspmove * 0.3, 0, 1, 0);
	glTranslated(13, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.8, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();


	glBindTexture(GL_TEXTURE_2D, texId[8]);//������(����� ��������� �� ��������������,�� ������� ��������
										   // ����� ���� ����������,�� �������������� ���������)
	//double rgb1[] = { 0.960784,0.960784,0.862745 };
	glPushMatrix();
	glRotated(22, 1, 0, 0);
	glRotated(-urspmove * 4, 0, 1, 0);
	glTranslated(15, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(0.2, 8, 8, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	urspmove++;

}
int urmove = 40;
void uran() {
	glBindTexture(GL_TEXTURE_2D, texId[8]);
	double rgb1[] = { 0.82352,0.9098039,0.988235};
	glPushMatrix();
	if (camfl == 8)
	{
		vx = 270 * cos(urmove*0.8 * M_PI / 180.0);
		vy = 270 * sin(urmove*0.8 * M_PI / 180.0);
		vz = 0;
		if (ht == 1) {
			camera.camDist = 60;
			ht = 0;
		}
	}
	glRotated(urmove*0.8, 0, 0, 1);
	glTranslated(270, 0, 0);
	glRotated(po*1.1,0, 1,1);
	glRotated(-96, 1, 0, 0);
	if (camfl == 8 && cutP == true) {
		ur_without();
	}
	else {
		planet(6.5, 40, 40, 1, 1, rgb1, rgb1, true);
		ursp();
	}

	glPopMatrix();
	urmove++;

}
int trimove = 0;
void triton()//������,������� �������
{
	double rgb1[] = { 0.960784,0.960784,0.862745 };

	glBindTexture(GL_TEXTURE_2D, texId[3]);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glRotated(-trimove * 4, 0, 1, 0);
	glTranslated(13, 0, 0);
	glRotated(-15, 1, 0, 0);
	planet(1.3, 12, 12, 1, 1, rgb1, rgb1, true);
	glPopMatrix();
	trimove++;
}
int neptmove = 0;
void neptun()//������,� ���� 17 ���������,�� �������� ������ ���� �� �� ����� 95,9% �� ����� ����� ��������� �������
{
	glBindTexture(GL_TEXTURE_2D, texId[9]);
	double rgb1[] = { 0.05882,0.32156862,0.729411 };
	glPushMatrix();
	if (camfl == 9)
	{
		vx = 297 * cos(neptmove*0.6 * M_PI / 180.0);
		vy = 297 * sin(neptmove*0.6 * M_PI / 180.0);
		vz = 0;
		if (ht == 1) {
			camera.camDist = 60;
			ht = 0;
		}
	}

	glRotated(neptmove*0.6, 0, 0, 1);
	glTranslated(297, 0, 0);
	glRotated(po*0.3, 1, 1, 0);
	glRotated(96, 1, 0, 0);
	if (camfl == 9 && cutP == true) {
		nept_without();
	}
	else {
		planet(7, 30, 30, 1, 1, rgb1, rgb1, true);
		triton();
	}

	glPopMatrix();
	neptmove++;
}
int pluspmove = 320;
void plusp() {//�������� �������
	double rgb1[] = { 0.960784,0.960784,0.862745 };

	glBindTexture(GL_TEXTURE_2D, texId[6]);//�����
	glPushMatrix();
	glRotated(-20, 1, 0, 0);
	glRotated(-pluspmove * 3.9, 0, 1, 0);
	glTranslated(3, 0, 0);
	glRotated(90, 1, 0, 0);
	planet(0.3, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();


	glBindTexture(GL_TEXTURE_2D, texId[3]);//�����
	glPushMatrix();
	glRotated(270, 1, 0, 0);
	glRotated(pluspmove * 2, 0, 1, 0);
	glTranslated(3.5, 0, 0);
	glRotated(1, 1, 0, 0);
	planet(0.1, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();


	glBindTexture(GL_TEXTURE_2D, texId[7]);//�����
	glPushMatrix();
	glRotated(310, 1, 0, 0);
	glRotated(pluspmove * 8, 0, 1, 0);
	glTranslated(4, 0, 0);
	glRotated(125, 1, 0, 0);
	planet(0.1, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[9]);//������
	glPushMatrix();
	glRotated(-260, 1, 0, 0);
	glRotated(-pluspmove * 2, 0, 1, 0);
	glTranslated(4.5, 0, 0);
	glRotated(5, 1, 0, 0);
	planet(0.15, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texId[5]);//�����
	glPushMatrix();
	glRotated(30, 1, 0, 0);
	glRotated(pluspmove * 10, 0, 1, 0);
	glTranslated(5, 0, 0);
	glRotated(-95, 1, 0, 0);
	planet(0.12, 10, 10, 1, 1, rgb1, rgb1, true);
	glPopMatrix();
	trimove++;

}
int plumove = 320;
void pluton() {
	glBindTexture(GL_TEXTURE_2D, texId[3]);
	double rgb1[] = { 1,0.9,0.7 };
	glPushMatrix();
	if (camfl == 10)
	{
		vx = 320 * cos(plumove*0.5 * M_PI / 180.0);
		vy = 320 * sin(plumove*0.5 * M_PI / 180.0);
		vz = 0;
		if (ht == 1) {
			camera.camDist = 40;
			ht = 0;
		}
	}

	glRotated(plumove*0.5, 0, 0, 1);
	glTranslated(320, 0, 0);
	glRotated(po*0.7, 1, 0, 1);
	glRotated(79, 1, 0, 0);
	planet(0.8, 10, 10, 1, 1, rgb1, rgb1, true);
	plusp();
	glPopMatrix();
	plumove++;

}
int makmove = 0;
void makemake() {
	glBindTexture(GL_TEXTURE_2D, texId[8]);
	double rgb1[] = { 0.56,0.9,0.7 };
	glPushMatrix();

	if (camfl == 11)
	{
		vx = 327 * cos(makmove*0.4 * M_PI / 180.0);
		vy = 327 * sin(makmove*0.4 * M_PI / 180.0);
		vz = 0;
		if (ht == 1) {
			camera.camDist = 40;
			ht = 0;
		}
	}
	glRotated(makmove*0.4, 0, 0, 1);
	glTranslated(327, 0, 0);
	glRotated(po*0.9, 1, 0, 1);
	glRotated(-85, 1, 0, 0);
	planet(0.6, 15, 15, 1, 1, rgb1, rgb1, true);
	glPopMatrix();
	makmove++;
}

//��������� ��������� �������
void system() {

	glPushMatrix();
	sun();
	glPopMatrix();
	//��������� ���������
	GLfloat amb[] = { 1, 1, 1, 0.2 };
	GLfloat dif[] = { 1, 1, 1, 0.2 };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 0.2 };
	GLfloat sh = 0.1f * 256;
	

	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);
	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);


	mercurii();
	venera();
	earth();
	marc();
	cerera();//��������� � ������ ���������� �������
	jupiter();
	saturn();
	uran();
	neptun();
	pluton();//���������� ���������� ������� ��������� �������
	makemake();//������ �� �������� ���������� ������� ��������� �������
}
void Render(OpenGL *ogl)
{       	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

    //���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


/*
	glBindTexture(GL_TEXTURE_2D, texId[0]);
	circle();
	glBindTexture(GL_TEXTURE_2D, texId[1]);
	ff();    ������ ���������� ������� */

	//double rgb3[] = { 1,1, 1 };
	//cosmos(0,0,0,500,rgb3);
	cube(490);//������
	//���������� ��������
	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	double rgb1[] = { 1, 0.7, 0.2 };
	double rgb2[] = { 0.1, 0.4, 0.8 };

	




	//��������� �������
	//!!!!!�������� � ���� ��������,���� ��������� � ������ ���������

	system();



	//����� ��������� ������ �����, ���� ������� - ���������������, ��� �������� =)
	char c[250];  //������������ ����� ���������
	/*sprintf_s(c, "(T)�������� - %d\n(L)���� - %d\n\n���������� ������:\n"
		"G - ����������� � �������������� ���������,\nG+���+����������� �� ������������ �����\n"
		"R - ���������� ������ � ���� � ��������� ���������\n"
		"F - ����������� ���� � ����� ������", textureMode, lightMode);
	ogl->message = std::string(c);*/




}   //����� ���� �������

