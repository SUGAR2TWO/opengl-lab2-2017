#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"



bool textureMode = true;
bool lightMode = true;

void ayaya(double x1[], double y1[], double z1[], double vecn[])      
	{
		/*double y[] = { x1[0] - y1[0], x1[1] - y1[1], x1[2] - y1[2] };
		double x[] = { z1[0] - y1[0], z1[1] - y1[1], z1[2] - y1[2] };
		
		vecn[0] = y[1] * x[2] - y[1] * x[2];
		vecn[1] = -x[0] * y[2] + y[0] * x[2];
		vecn[2] = x[0] * y[1] - y[0] * x[2];*/
		double qx, qy, qz, px, py, pz;

	qx = x1[0] - y1[0];
	qy = x1[1] - y1[1];
	qz = x1[2] - y1[2];
	px = z1[0] - y1[0];
	py = z1[1] - y1[1];
	pz = z1[2] - y1[2];
	vecn[0] = py*qz - pz*qy;
	vecn[1] = pz*qx - px*qz;
	vecn[2] = px*qy - py*qx;

		double length = sqrt(pow(vecn[0], 2) + pow(vecn[1], 2) + pow(vecn[2], 2));

		vecn[0] /= length;
		vecn[1] /= length;
		vecn[2] /= length;
	}

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
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
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
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

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
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
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}




//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("KEK.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	GLuint texId;
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
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


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

    //чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  ФИГУРКА!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	float a, a1;
	int i;
	double	/*ax = -4, ay = 1,
			bx = -5, by = 6,
			cx = -2, cy = 4,
			dx = 2, dy = 7,
			ex = 3, ey = 1,
			zn = 0, zv = 5,*/
			cex = 2.5, cey = 4, k;
	double TA[] = {(double)42/512,(double)(512-349)/512}, TA1[] = {(double)142/512,(double)(512-330)/512},
		   TB[] = {(double)15/512,(double)(512-215)/512}, TB1[] = {(double)115/512,(double)(512-195)/512},
		   TVy[] = {(double)503/512,(double)(512-503)/512}, TVy1[] = {(double)393/512,(double)(512-503)/512},
		   T_Vy[] = {(double)503/512,(double)(512-298)/512}, T_Vy1[] = {(double)393/512,(double)(512-298)/512},
		   TVp[] = {(double)143/512,(double)(512-422)/512}, TVp1[] = {(double)143/512,(double)(512-333)/512},
		   T_Vp[] = {(double)334/512,(double)(512-422)/512}, T_Vp1[] = {(double)334/512,(double)(512-333)/512},
		   TB_[] = {(double)170/512,(double)(512-120)/512}, TB1_[] = {(double)119/512,(double)(512-195)/512},
		   TC_[] = {(double)248/512,(double)(512-172)/512}, TC1_[] = {(double)197/512,(double)(512-247)/512},
		   T_A1[] = {(double)145/512,(double)(512-329)/512}, T_B1[] = {(double)117/512,(double)(512-195)/512},
		   T_C1[] = {(double)199/512,(double)(512-250)/512}, T_D1[] = {(double)307/512,(double)(512-169)/512},
		   TD[] = {(double)107/512,(double)(512-72)/512}, TD1[] = {(double)147/512,(double)(512-151)/512},
		   TC[] = {(double)1/512,(double)(512-125)/512}, TC1[] = {(double)41/512,(double)(512-204)/512},
		   T_E1[] = {(double)334/512,(double)(512-331)/512};
	double f1 = 94.5/512,f2 = 239.5/512, f3 = (double)25/512,f4 = (double)(512-329)/512, 
	   f5, f6;
	double A[]={-4,1,0}, A1[]={-4,1,5},
		   B[]={-5,6,0}, B1[]={-5,6,5},
		   C[]={-2,4,0}, C1[]={-2,4,5},
		   D[]={2,7,0}, D1[]={2,7,5},
		   E[]={3,1,0}, E1[]={3,1,5},
		   Vp[]= {cos( 0) * 3.5f-0.5f, sin( 0 ) * 1.5f+1, 0}, Vp1[] = {cos( 0) * 3.5f-0.5f, sin( 0 ) * 1.5f+1, 5},
		   _Vp[] = {cos( 0) * 3.5f-0.5f, sin( 0 ) * 1.5f+1, 0}, _Vp1[] = {cos( 0) * 3.5f-0.5f, sin( 0 ) * 1.5f+1, 5},
		   Vy[] = {cos( -0.0358 ) * 3.04f+2.5f, sin( -0.0358 ) * 3.05f+4, 0}, Vy1[] = {cos( -0.0358 ) * 3.04f+2.5f, sin( -0.0358 ) * 3.05f+4, 5},
		  _Vy[] = {cos( -0.0353 ) * 3.04f+2.5f, sin( -0.0353 ) * 3.05f+4, 0},_Vy1[] = {cos( -0.0353 ) * 3.04f+2.5f, sin( -0.0353 ) * 3.05f+4, 5},
		   vecn[3], mx, my, vp[3];

	

glBegin(GL_TRIANGLE_STRIP);
glColor3f( 1.0f,0.0f,1.0f);
k=0;

glNormal3f(0,0,-1);
glTexCoord2dv(T_D1);
	glVertex3dv(D);
	for( i = 0; i <= 20; i++ )
			   {
                   a = (float)i/50.0f*2.0f*3.1415f;
				   f5 = (double)239.5/512 + (double)94.5/512*cos(a);
				   f6 = (double)(512-329)/512 + (double)25/512*sin(a); 
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 0);
				   glTexCoord2dv(T_D1);
				   glVertex3dv(D);
               }
	
glEnd();

glBegin(GL_TRIANGLE_STRIP);
glColor3f( 1.0f,0.0f,1.0f);
glNormal3f(0,0,-1);
	
	for( i = 20; i <= 25; i++ )
			   {
				   
                   a = (float)i/50.0f*2.0f*3.1415f;
				   f5 = (double)239.5/512 + (double)94.5/512*cos(a);
				   f6 = (double)(512-329)/512 + (double)25/512*sin(a);
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 0);
				   glTexCoord2dv(T_B1);
				   glVertex3dv(B);
               }
	
glEnd();



glBegin(GL_TRIANGLE_STRIP);
glColor3f( 1.0f,0.0f,1.0f);
glNormal3f(0,0,-1);
glTexCoord2dv(T_B1);
	glVertex3dv(B);	
				   
                   a = (float)20/50.0f*2.0f*3.1415f;
				   f5 = (double)239.5/512 + (double)94.5/512*cos(a);
				   f6 = (double)(512-329)/512 + (double)25/512*sin(a); 
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 0);
				   glTexCoord2dv(T_C1);
				   glVertex3dv(C);
				   glTexCoord2dv(T_D1);
				   glVertex3dv(D);
				   a = (float)20/50.0f*2.0f*3.1415f;
				   f5 = (double)239.5/512 + (double)94.5/512*cos(a);
				   f6 = (double)(512-329)/512 + (double)25/512*sin(a); 
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 0);
               
	
glEnd();

glBegin(GL_TRIANGLE_STRIP);
glColor3f( 1.0f,0.0f,1.0f);
glNormal3f(0,0,-1);
glTexCoord2d((double)0.62566,(double)0.51346);
glVertex3f(cex,cey,0);
	 for( i = -81; i <= 100; i++ )
			   {
                   a1 = i/360.0f*2.0f*3.1415f;
				   f5 = (double)0.62566 + (double)0.16*cos(a1);
				   f6 = (double)0.51346 + (double)0.16*sin(a1); 
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a1 ) * 3.04f+2.5f, sin( a1 ) * 3.05f+4, 0);		
				   glTexCoord2d((double)0.62566,(double)0.51346);
				   glVertex3f(cex,cey,0);
               }
glEnd();


glBegin(GL_TRIANGLE_STRIP);
glColor3f( 1.0f,0.0f,1.0f);
k=0;

glNormal3f(0,0,1);
glTexCoord2dv(T_D1);
	glVertex3dv(D1);
	for( i = 0; i <= 20; i++ )
			   {
                   a = (float)i/50.0f*2.0f*3.1415f;
				   f5 = (double)239.5/512 + (double)94.5/512*cos(a);
				   f6 = (double)(512-329)/512 + (double)25/512*sin(a); 
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 5);
				   glTexCoord2dv(T_D1);
				   glVertex3dv(D1);
               }
	
glEnd();

glBegin(GL_TRIANGLE_STRIP);
glColor3f( 1.0f,0.0f,1.0f);
glNormal3f(0,0,1);
	
	for( i = 20; i <= 25; i++ )
			   {
				   
                   a = (float)i/50.0f*2.0f*3.1415f;
				   f5 = (double)239.5/512 + (double)94.5/512*cos(a);
				   f6 = (double)(512-329)/512 + (double)25/512*sin(a);
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 5);
				   glTexCoord2dv(T_B1);
				   glVertex3dv(B1);
               }
	
glEnd();



glBegin(GL_TRIANGLE_STRIP);
glColor3f( 1.0f,0.0f,1.0f);
glNormal3f(0,0,1);
glTexCoord2dv(T_B1);
	glVertex3dv(B1);	
				   
                   a = (float)20/50.0f*2.0f*3.1415f;
				   f5 = (double)239.5/512 + (double)94.5/512*cos(a);
				   f6 = (double)(512-329)/512 + (double)25/512*sin(a); 
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 5);
				   glTexCoord2dv(T_C1);
				   glVertex3dv(C1);
				   glTexCoord2dv(T_D1);
				   glVertex3dv(D1);
				   a = (float)20/50.0f*2.0f*3.1415f;
				   f5 = (double)239.5/512 + (double)94.5/512*cos(a);
				   f6 = (double)(512-329)/512 + (double)25/512*sin(a); 
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 5);
               
	
glEnd();

glBegin(GL_TRIANGLE_STRIP);
glColor3f( 1.0f,0.0f,1.0f);
glNormal3f(0,0,1);
glTexCoord2d((double)0.62566,(double)0.51346);
glVertex3f(cex,cey,5);
	 for( i = -81; i <= 100; i++ )
			   {
                   a1 = i/360.0f*2.0f*3.1415f;
				   f5 = (double)0.62566 + (double)0.16*cos(a1);
				   f6 = (double)0.51346 + (double)0.16*sin(a1); 
				   glTexCoord2d(f5,f6);
				   glVertex3f( cos( a1 ) * 3.04f+2.5f, sin( a1 ) * 3.05f+4, 5);		
				   glTexCoord2d((double)0.62566,(double)0.51346);
				   glVertex3f(cex,cey,5);
               }
glEnd();
//////////////////////////////////////////////////////////////////////////////////////////////////
glBegin(GL_QUADS);
glColor3f( 1.0f,1.0f,0.0f);
ayaya(C1, C, B, vecn);
glNormal3dv(vecn);
glTexCoord2dv(TC_);
	glVertex3dv(C);
glTexCoord2dv(TC1_);
	glVertex3dv(C1);
glTexCoord2dv(TB1_);
	glVertex3dv(B1);
glTexCoord2dv(TB_);
	glVertex3dv(B);
glEnd();
//////////////////////////////////////////////////////////////////////////////////////////////////
glBegin(GL_QUADS);
glColor3f( 0.0f,1.0f,0.0f);
ayaya(D1,D,C,vecn);
glNormal3dv(vecn);
glTexCoord2dv(TD);
	glVertex3dv(D);
glTexCoord2dv(TD1);
	glVertex3dv(D1);
glTexCoord2dv(TC1);
	glVertex3dv(C1);
glTexCoord2dv(TC);
	glVertex3dv(C);
glEnd();
////////////////////////////////////////////////////////////////////////////////////////////////
glBegin(GL_QUADS);
glColor3f( 1.0f,0.0f,0.0f);
ayaya(B1, B, A, vecn);
glNormal3dv(vecn);
glTexCoord2dv(TB);
    glVertex3dv(B);
glTexCoord2dv(TB1);
	glVertex3dv(B1);
glTexCoord2dv(TA1);
	glVertex3dv(A1);
glTexCoord2dv(TA);
	glVertex3dv(A);
glEnd();
/////////////////////////////////////////////////////////////////////////////////////////////////
glColor3f(0.0f,1.0f,1.5f);
glBegin( GL_QUAD_STRIP);
k=0;
	for(i=-81;i<=100;i++)
    {   
		ayaya(Vy1, Vy, _Vy, vecn);
		glNormal3dv(vecn);
			   {
                   a1 = i/360.0f*2.0f*3.1415f;
				   glTexCoord2d(TVy[0],TVy[1]+k);
				   glVertex3f( cos( a1 ) * 3.04f+2.5f, sin( a1 ) * 3.05f+4, 0);
				   glTexCoord2d(TVy1[0],TVy1[1]+k);
				   glVertex3f( cos( a1 ) * 3.04f+2.5f, sin( a1 ) * 3.05f+4, 5);
			   Vy[0] = cos( a1 ) * 3.04f+2.5f;
			   Vy[1] = sin( a1 ) * 3.05f+4;
			   Vy[2] = 0;
			   Vy1[0] = cos( a1 ) * 3.04f+2.5f;
			   Vy1[1] = sin( a1 ) * 3.05f+4;
			   Vy1[2] = 5;
			   int j=i+1;
			   
			   a1=j/360.0f*2.0f*3.1415f;
			   
			   _Vy[0] = cos( a1 ) * 3.04f+2.5f;
			   _Vy[1] = sin( a1 ) * 3.05f+4;
			   _Vy[2] = 0;
			   }
			/*TVy[] = {(double)503/512,(double)(512-503)/512}, TVy1[] = {(double)393/512,(double)(512-503)/512},
		   T_Vy[] = {(double)503/512,(double)(512-298)/512}, T_Vy1[] = {(double)393/512,(double)(512-298)/512};*/
			   k+=(double)205/181/512;
	}
    glEnd();
/////////////////////////////////////////////////////////////////////////////////////////////////
glColor3f(0.0f,1.0f,0.0f);
glBegin( GL_QUAD_STRIP);
k=0;
for(i=0;i<=25;i++)
    {   
		ayaya(_Vp, Vp, Vp1, vecn);
		glNormal3dv(vecn);
			   {
                   a = i/50.0f*2.0f*3.1415f;
				   glTexCoord2d(TVp[0]+k,TVp[1]);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 0);
				   glTexCoord2d(TVp1[0]+k,TVp1[1]);
				   glVertex3f( cos( a ) * 3.5f-0.5f, sin( a ) * 1.5f+1, 5);
			   Vp[0] = cos( a ) * 3.5f-0.5f;
			   Vp[1] = sin( a ) * 1.5f+1;
			   Vp[2] = 0;
			   Vp1[0] = cos( a ) * 3.5f-0.5f;
			   Vp1[1] = sin( a ) * 1.5f+1;
			   Vp1[2] = 5;
			   int j=i+1;
			   
			   a=j/50.0f*2.0f*3.1415f;
			   
			   _Vp[0] = cos( a ) * 3.5f-0.5f;
			   _Vp[1] = sin( a ) * 1.5f+1;
			   _Vp[2] = 0;
			   }
			   /*TVp[] = {(double)143/512,(double)(512-422)/512}, TVp1[] = {(double)143/512,(double)(512-333)/512},
		   T_Vp[] = {(double)334/512,(double)(512-422)/512}, T_Vp1[] = {(double)334/512,(double)(512-333)/512};*/
			    k+=(double)191/25/512;
	}

    glEnd();




//ФИГУРКА СТОП!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*
	//Начало рисования квадратика станкина
	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	
	
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();
	//конец рисования квадратика станкина
    */
	
	//текст сообщения вверху слева, если надоест - закоментировать, или заменить =)
	/*char c[250];  //максимальная длина сообщения
	sprintf_s(c, "(T)Текстуры - %d\n(L)Свет - %d\n\nУправление светом:\n"
		"G - перемещение в горизонтальной плоскости,\nG+ЛКМ+перемещение по вертикальной линии\n"
		"R - установить камеру и свет в начальное положение\n"
		"F - переместить свет в точку камеры", textureMode, lightMode);
	ogl->message = std::string(c);*/




}   //конец тела функции

