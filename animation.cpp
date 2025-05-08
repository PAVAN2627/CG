/*
#include <iostream>
#include <math.h>
#include <time.h>
#include <GL/glut.h>

using namespace std;

int y = 0;
int flag = 0;
void init()
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 640, 0, 480);
}
void object1()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2i(y, 220);
    glVertex2i(y + 40, 220);
    glVertex2i(y + 40, 260);
    glVertex2i(y, 260);
    glEnd();
    glutSwapBuffers();
}
void timer(int)
{
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);

    if (flag == 0)
    {
        y = y + 3;
    }
    if (flag == 1)
    {
        y = y - 3;
    }
    if (y == 600)
    {
        flag = 1;
    }
    if (y == 0)
    {
        flag = 0;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Animation");
    init();
    glutDisplayFunc(object1);
    glutTimerFunc(1000, timer, 0);
    glutMainLoop();
    return 0;
}
*/

#include <iostream>
#include <math.h>
#include <time.h>
#include <GL/glut.h>

using namespace std;

// Variables for object position and movement direction
int x_pos = 0;       // Renamed 'y' to 'x_pos' for clarity
int y_pos = 0;       // New variable for vertical position
int x_direction = 1; // 1 for moving right, -1 for moving left
int y_direction = 1; // 1 for moving up, -1 for moving down

// Variables for object color
float r_color = 1.0;
float g_color = 0.0;
float b_color = 0.0;

// Window dimensions (matching gluOrtho2D)
const int window_width = 640;
const int window_height = 480;

// Object size (adjust based on the shape you draw)
const int object_size = 40; // Example size for a square or triangle base

void init()
{
    glClearColor(1.0, 1.0, 1.0, 0.0); // White background
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, window_width, 0, window_height); // Set up 2D orthographic projection
}

void drawObject()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Set the object's color
    glColor3f(r_color, g_color, b_color);

    // Draw a different shape - let's draw a triangle for example
    glBegin(GL_QUADS);
    // Vertices are defined relative to the object's position (x_pos, y_pos)
    glVertex2i(x_pos, y_pos);
    glVertex2i(x_pos + object_size, y_pos);
    glVertex2i(x_pos + object_size / 2, y_pos + object_size);
    glEnd();

    // To draw a square/rectangle again, use GL_QUADS or GL_POLYGON:
    glBegin(GL_QUADS);
    glVertex2i(x_pos, y_pos);
    glVertex2i(x_pos + object_size, y_pos);
    glVertex2i(x_pos + object_size, y_pos + object_size);
    glVertex2i(x_pos, y_pos + object_size);
    glEnd();

    glutSwapBuffers(); // Swap the front and back buffers to display the drawing
}

void timer(int)
{
    glutPostRedisplay();                // Request a redraw of the window
    glutTimerFunc(1000 / 60, timer, 0); // Call timer again after 1/60th of a second (approx 60 FPS)

    // Update object position based on direction
    x_pos += x_direction * 3; // Move horizontally
    y_pos += y_direction * 3; // Move vertically

    // Check for collisions with window edges and reverse direction if needed

    // Collision with right edge
    if (x_pos + object_size > window_width)
    {
        x_direction = -1; // Move left
        // Optional: Adjust position to prevent sticking to the edge
        x_pos = window_width - object_size;
    }
    // Collision with left edge
    if (x_pos < 0)
    {
        x_direction = 1; // Move right
        // Optional: Adjust position to prevent sticking to the edge
        x_pos = 0;
    }
    // Collision with top edge
    if (y_pos + object_size > window_height)
    {
        y_direction = -1; // Move down
        // Optional: Adjust position to prevent sticking to the edge
        y_pos = window_height - object_size;
    }
    // Collision with bottom edge
    if (y_pos < 0)
    {
        y_direction = 1; // Move up
        // Optional: Adjust position to prevent sticking to the edge
        y_pos = 0;
    }

    // Example: Change color based on direction (optional)
    // if (x_direction == 1 && y_direction == 1) glColor3f(1.0, 0.0, 0.0); // Red
    // else if (x_direction == -1 && y_direction == 1) glColor3f(0.0, 1.0, 0.0); // Green
    // else if (x_direction == -1 && y_direction == -1) glColor3f(0.0, 0.0, 1.0); // Blue
    // else if (x_direction == 1 && y_direction == -1) glColor3f(1.0, 1.0, 0.0); // Yellow
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Use double buffering for smoother animation
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Animation");

    init(); // Initialize OpenGL settings

    glutDisplayFunc(drawObject); // Set the display callback function
    glutTimerFunc(0, timer, 0);  // Start the timer immediately

    glutMainLoop(); // Enter the GLUT event processing loop

    return 0;
}
