#include "stdafx.h"
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"

#include <math.h>

// Hang qui uoc trang thai di chuyen cua tay va chan
const char BACKWARD_STATE = 0;
const char FORWARD_STATE = 1;

const char LEFT = 0;
const char RIGHT = 1;

char legStates[2];
char armStates[2];

float legAngles[2];
float armAngles[2];

float angle;

// Các hướng di chuyển có thể
const char MOVE_LEFT = 0;
const char MOVE_RIGHT = 1;
const char MOVE_UP = 3;
const char MOVE_DOWN = 4;

// Góc xoay robot tương ứng khi sử dụng các phím điều khiển
const int TURN_LEFT = -90;
const int TURN_RIGHT = 90;
const int TURN_UP = 180;
const int TURN_DOWN = 0;

const int GROUND_STZE = 200; // Kích thước của mặt sàn 
const float FLOOR_HEIGHT = -2; // Độ cao của mặt sàn 
const int CHECK_STZE = 5; // Kích thước của mỗi ô vuông

int moveDirection; // Hướng di chuyển hiện tại 
float moveX; // Vị trí di chuyển đến theo trục x 
float moveZ; // Vị trí di chuyển đên theo trục z
float theta; // Góc xoay của camera (tính tọa độ x và z)
float y; // Tọa độ y của camera
float dTheta; // Mức tăng/giảm theta khi điều khiển
float dy; // Mức tăng/giảm y khi điều khiển

//Khai báo thêm các hàm : 
void DrawGround();
void SettingCamera(float theta, float y);
void Keyboard(unsigned char key, int, int);
void Special(int key, int, int);

void DrawCube(float xPos, float yPos, float zPos);
void DrawArm(float xPos, float yPos, float zPos);
void DrawHead(float xPos, float yPos, float zPos);
void DrawTorso(float xPos, float yPos, float zPos);
void DrawLeg(float xPos, float yPos, float zPos);
void DrawRobot(float xPos, float yPos, float zPos);
void Prepare();
void Display();
void Init();
void Reshape(int Width, int Height);
void Idle();

void DrawCube(float xPos, float yPos, float zPos) {
	glPushMatrix();
		glTranslatef(xPos, yPos, zPos);
		glBegin(GL_QUADS);
			// Ve mat tren
			glNormal3d(0, 1, 0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, 0.0f, 0.0f);
		
			// Ve mat truoc
			glNormal3d(0, 0, 1);
			glVertex3f(-1.0f, 0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);
			glVertex3f(0.0f, -1.0f, 0.0f);

			// Ve mat ben phai
			glNormal3d(1, 0, 0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, -1.0f, 0.0f);
			glVertex3f(0.0f, -1.0f, -1.0f);
			glVertex3f(0.0f, 0.0f, -1.0f); 
		
			// Ve mat ben trai
			glNormal3d(-1, 0, 0); 
			glVertex3f(-1.0f, 0.0f, 0.0f); 
			glVertex3f(-1.0f, 0.0f, -1.0f); 
			glVertex3f(-1.0f, -1.0f, -1.0f); 
			glVertex3f(-1.0f, -1.0f, 0.0f); 
		
			// Ve mat duoi
			glNormal3d(0, -1, 0); 
			glVertex3f(0.0f, 0.0f, 0.0f); 
			glVertex3f(0.0f, -1.0f, -1.0f); 
			glVertex3f(-1.0f, -1.0f, -1.0f); 
			glVertex3f(-1.0f, -1.0f, 0.0f); 
		
			// Ve mat sau
			glNormal3d(0, 0, -1);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(-1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(0.0f, -1.0f, -1.0f);
		glEnd();
	glPopMatrix(); // Quay lai trang thai bien doi da luu
}

//Hàm drawGround đê vẽ mặt sàn.
void DrawGround() {
	int x, z;
	int counter = 0;

	for (x = -GROUND_STZE; x < GROUND_STZE; x += CHECK_STZE) {
		for (z = -GROUND_STZE; z < GROUND_STZE; z += CHECK_STZE) {
			if (counter % 2 == 0)
				glColor3f(0.2, 0.2, 1.0); // Tô màu xanh 
			else
				glColor3f(1.0, 1.0, 1.0); // To màu trắng
			
			glBegin(GL_QUADS);
				glNormal3d(0, 1, 0);
				glVertex3f(x, FLOOR_HEIGHT, z);
				glVertex3f(x, FLOOR_HEIGHT, z + CHECK_STZE);
				glVertex3f(x + CHECK_STZE, FLOOR_HEIGHT, z + CHECK_STZE);
				glVertex3f(x + CHECK_STZE, FLOOR_HEIGHT, z);
			glEnd();
			counter++;
		}
		counter++;
	}
}

void SettingCamera(float theta, float y) {
	gluLookAt(50 * sin(theta), y, 50 * cos(theta),
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);
}

void Keyboard(unsigned char key, int, int) {
	switch (key)
	{
		case 'a': theta -= dTheta; break;
		case 'd': theta += dTheta; break;
		case 'w': y += dy; break;
		case 's': if (y > dy) y -= dy; break;
	}
	glutPostRedisplay();
}

void Special(int key, int, int) {
	switch (key) {
	case GLUT_KEY_LEFT:
		moveDirection = MOVE_LEFT;
		angle = TURN_LEFT;
		break;
	case GLUT_KEY_RIGHT:
		moveDirection = MOVE_RIGHT;
		angle = TURN_RIGHT;
		break;
	case GLUT_KEY_UP:
		moveDirection = MOVE_UP;
		angle = TURN_UP;
		break;
	case GLUT_KEY_DOWN:
		moveDirection = MOVE_DOWN;
		angle = TURN_DOWN;
		break;
	}
	glutPostRedisplay();
}

// Ham DrawArm de ve canh tay(hinh hop chu nhat) tai vi tri chi dinh.
void DrawArm(float xPos, float yPos, float zPos) {
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f); // To mau do
	glTranslatef(xPos, yPos, zPos);
	glScalef(1.0f, 4.0f, 1.0f); // Kich thuoc 1x4x1
	DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

// Ham DrawHead de ve dau(hinh hop chu nhat) tai vi tri chi dinh.
void DrawHead(float xPos, float yPos, float zPos) {
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f); // To mau trang
	glTranslatef(xPos, yPos, zPos);
	glScalef(2.0f, 2.0f, 2.0f); // Kich thuoc 2x2x2
	DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

// Ham DrawTorso de ve than (hinh hop chu nhat) tai vi tri chi dinh.
void DrawTorso(float xPos, float yPos, float zPos) {
	glPushMatrix();
		glColor3f(0.0f, 0.0f, 1.0f); // To mau xanh
		glTranslatef(xPos, yPos, zPos);
		glScalef(3.0f, 5.0f, 2.0f); // Kich thuoc 3x5x2
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

// Ham DrawLeg de ve chan (hinh hop chu nhat) tai vi tri chi dinh.
void DrawLeg(float xPos, float yPos, float zPos) {
	glPushMatrix();
		glColor3f(1.0f, 1.0f, 0.0f); // To mau vang
		glTranslatef(xPos, yPos, zPos);
		glScalef(1.0f, 5.0f, 1.0f); // Kich thuoc 1x5x1 
		DrawCube(0.0f, 0.0f, 0.0f); 
	glPopMatrix();
}

void DrawRobot(float xPos, float yPos, float zPos) {
	glPushMatrix();
		glTranslatef(xPos, yPos, zPos); // Toa do ve robot
	
		// Ve phan dau va phan than
		DrawHead(1.0f, 2.0f, 0.0f);
		DrawTorso(1.5f, 0.0f, 0.0f);

		// Di chuyen canh tay trai va xoay canh tay de tao hieu ung "dang buoc di"
		glPushMatrix();
			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(armAngles[LEFT], 1.0f, 0.0f, 0.0f);
			DrawArm(2.5f, 0.0f, -0.5f);
		glPopMatrix();

		//Di chuyen canh tay phai va xoay canh tay de tao hieu ung "dang buoc di"
		glPushMatrix();
			glTranslatef(0.0f, -0.5f, 0.0f); 
			glRotatef(armAngles[RIGHT], 1.0f, 0.0f, 0.0f);
			DrawArm(-1.5f, 0.0f, -0.5f);
		glPopMatrix();

		// Di chuyen chan trai va xoay chan de tao hieu ung "dang buoc"
		glPushMatrix();
			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(legAngles[LEFT], 1.0f, 0.0f, 0.0f);
			DrawLeg(-0.5f, -5.0f, -0.5f);
		glPopMatrix();
	
		//Di chuyen chan phai va xoay chan de tao hieu ung "dang buoc"
		glPushMatrix();
			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(legAngles[RIGHT], 1.0f, 0.0f, 0.0f);
			DrawLeg(1.5f, -5.0f, -0.5f);
		glPopMatrix();
	glPopMatrix();
}

void Display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity(); // reset modelview matrix
	SettingCamera(theta, y);

	//angle = angle + 0.05f; // Tăng góc xoay của robot 
	//if (angle >= 360.0f) // Nếu xoay đủ vòng, reset góc xoay 
	//	angle = 0.0f;

	glPushMatrix();
		glTranslatef(moveX, 8.0f, moveZ);
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
		Prepare();
		DrawRobot(0.0f, 0.0f, 0.0f); // Vẽ robot
	glPopMatrix();
	
	glPushMatrix();
		DrawGround();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

//void Prepare()
//{ 
//	/* Nếu tay/chân đang di chuyển về phía trước thì tăng góc xoay, ngược lại (di chuyển về phía sau) thì giảm góc xoay. */ 
//	for (char side = 0; side < 2; side++) {
//		// Góc xoay cho tay 
//		if (armStates[side] == FORWARD_STATE)
//			armAngles[side] += 0.1f;
//		else 
//			armAngles[side] -= 0.1f; 
//		// Thay đổi trạng thái nếu góc xoay vượt quá giá trị cho phép 
//		if (armAngles[side] >= 15.0f) 
//			armStates[side] = BACKWARD_STATE; 
//		else if (armAngles[side] <= -15.0f) 
//			armStates[side] = FORWARD_STATE;
//		// Góc xoay cho chân 
//		if (legStates[side] == FORWARD_STATE) 
//			legAngles[side] += 0.1f;
//		else 
//			legAngles[side] -= 0.1f; // Thay đổi trạng thái nếu góc xoay vượt quá giá trị cho phép 
//		if (legAngles[side] >= 15.0f) 
//			legStates[side] = BACKWARD_STATE; 
//		else if (legAngles[side] <= -15.0f) 
//			legStates[side] = FORWARD_STATE; 
//	}
//}

void Prepare() {
	/* Nếu tay/chân đang di chuyển về phía trước thì tăng góc xoay, ngược lại (di chuyển về phía sau) thì giảm góc xoay. */
	for (char side = 0; side < 2; side++) {
		// Góc xoay cho tay 
		if (armStates[side] == FORWARD_STATE)
			armAngles[side] += 0.1f;
		else
			armAngles[side] -= 0.1f;
		// Thay đổi trạng thái nếu góc xoay vượt quá giá trị cho phép 
		if (armAngles[side] >= 15.0f)
			armStates[side] = BACKWARD_STATE;
		else if (armAngles[side] <= -15.0f)
			armStates[side] = FORWARD_STATE;
		// Góc xoay cho chân
		if (legStates[side] == FORWARD_STATE)
			legAngles[side] += 0.1f;
		else
			legAngles[side] -= 0.1f;
		// Thay đổi trạng thái nếu góc xoay vượt quá giá trị cho phép 
		if (legAngles[side] >= 15.0f)
			legStates[side] = BACKWARD_STATE;
		else if (legAngles[side] <= -15.0f)
			legStates[side] = FORWARD_STATE;
	}

	switch (moveDirection) {
		case MOVE_LEFT:
			moveX = moveX - 0.0005f; break;
		case MOVE_RIGHT:
			moveX = moveX + 0.015f; break;
		case MOVE_UP:
			moveZ = moveZ - 0.015f; break;
		case MOVE_DOWN:
			moveZ = moveZ + 0.015f; break;
	}
}

void Init()
{
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Màu ánh sáng ambient
	GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
	// Màu ánh sáng diffuse
	GLfloat diffuseLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// Vị trí nguồn sáng 
	GLfloat lightPos[] = { 25.9f, 25.0f, 25.0f, 0.0}; 
	// Hướng chiếu sáng
	GLfloat spotDir[] = { 0.0, 0.0, 0.0, 0.0 };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); 
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	// Gán giá trị mặc định ban đầu cho robot angle = 0.0f;
	armAngles[LEFT] = 0.0;
	armAngles[RIGHT] = 0.0;
	legAngles[LEFT] = 0.0;
	legAngles[RIGHT] = 0.0;
	armStates[LEFT] = FORWARD_STATE;
	armStates[RIGHT] = BACKWARD_STATE;
	legStates[LEFT] = FORWARD_STATE;
	legStates[RIGHT] = BACKWARD_STATE;

	moveDirection = MOVE_LEFT;
	angle = TURN_LEFT;
	moveX = 0.0f;
	moveZ = 0.0f;

	// Gan gia tri mac dinh ban dau cho camera
	theta = 0.0f;
	y = 5.0f;
	dTheta = 0.04f;
	dy = 1.0f;
}

void Reshape(int Width, int Height)
{
	glViewport(0, 0, (GLsizei)Width, (GLsizei)Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)Width / (GLfloat)Height, 1.0, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(5.0, 5.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void Idle() {
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Robot");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);
	glutSpecialFunc(Special);
	glutKeyboardFunc(Keyboard);
	Init();
	glutMainLoop();
	return 0;
}