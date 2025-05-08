#include <iostream>
#include <math.h>
#include <GL/glut.h>

using namespace std;

float R = 0, G = 0, B = 0;

void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 640, 0, 480);
}

bool areColorsEqual(float* color1, float* color2, float tolerance = 0.001) {
    return fabs(color1[0] - color2[0]) < tolerance &&
           fabs(color1[1] - color2[1]) < tolerance &&
           fabs(color1[2] - color2[2]) < tolerance;
}


void boundaryFill(int x, int y, float* fillColor, float* bc) {
    float color[3];

    if (x < 0 || x >= 640 || y < 0 || y >= 480) {
        return;
    }

    glReadPixels(x, y, 1.0, 1.0, GL_RGB, GL_FLOAT, color);

    // Check if the current pixel color is NOT the boundary color AND NOT the fill color
    if (!areColorsEqual(color, bc) && !areColorsEqual(color, fillColor)) {
        glColor3f(fillColor[0], fillColor[1], fillColor[2]);
        glBegin(GL_POINTS);
        glVertex2i(x, y);
        glEnd();
        glFlush();

        boundaryFill(x + 1, y, fillColor, bc);
        boundaryFill(x - 1, y, fillColor, bc);
        boundaryFill(x, y + 1, fillColor, bc);
        boundaryFill(x, y - 1, fillColor, bc);
    }
}

void mouse(int btn, int state, int x, int y) {
    y = 480 - y;

    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // The boundary color is red (1,0,0) as drawn by B_Draw
        float bcol[] = {1.0, 0.0, 0.0};
        // The new fill color comes from the menu selection
        float newCol[] = {R, G, B};

        // Directly call boundaryFill
        boundaryFill(x, y, newCol, bcol);
    }
}

void B_Draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 0, 0); // Set color to Red for the boundary
    glBegin(GL_LINE_LOOP); // Draw the triangle boundary

    glVertex2i(150, 100);
    glVertex2i(300, 300);
    glVertex2i(450, 100);
    glEnd();
    glFlush();
}

void goMenu(int value) {
    switch (value) {
        case 1:
            R = 0, G = 1, B = 0; // Green
            break;
        case 2:
            R = 1, G = 1, B = 0; // Yellow
            break;
        case 3:
            R = 1, G = 0, B = 1; // Pink/Magenta
            break;
    }
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Boundary Fill Demo");

    init();

    glutCreateMenu(goMenu);
    glutAddMenuEntry("Color 1 Green", 1);
    glutAddMenuEntry("Color 2 Yellow", 2);
    glutAddMenuEntry("Color 3 Pink", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // Directly set the display function to B_Draw for Boundary Fill
    glutDisplayFunc(B_Draw);

    glutMouseFunc(mouse);

    glutMainLoop();

    return 0;
}
