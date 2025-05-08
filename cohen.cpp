#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <cmath> // For fabs

using namespace std;

// Structure to represent a 2D point
struct Point
{
    double x, y;
};

// Clipping Window (Viewport) coordinates
const int wxmin = 200, wxmax = 500, wymin = 100, wymax = 350;

// Original and clipped polygon vertices
vector<Point> original_polygon;
vector<Point> clipped_polygon;

// Mouse interaction state
bool drawing_polygon = false; // Flag to indicate if the user is currently drawing the polygon

void init()
{
    glClearColor(1.0, 1.0, 1.0, 0.0); // White background
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 640, 0, 480);   // Set up 2D orthogonal projection
    glClear(GL_COLOR_BUFFER_BIT); // Clear the buffer
}

// Function to draw the polygon
void drawPolygon(const vector<Point> &polygon, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (const auto &p : polygon)
    {
        glVertex2d(p.x, p.y);
    }
    glEnd();
    glFlush();
}

// Function to draw the clipping window
void drawClippingWindow()
{
    glColor3f(0.0, 1.0, 0.0); // Green color for the window
    glBegin(GL_LINE_LOOP);
    glVertex2i(wxmin, wymin);
    glVertex2i(wxmax, wymin);
    glVertex2i(wxmax, wymax);
    glVertex2i(wxmin, wymax);
    glEnd();
    glFlush();
}

// Helper function to check if a point is inside the clipping window for a specific edge
// edge: 0-left, 1-right, 2-bottom, 3-top
bool isInside(Point p, int edge)
{
    switch (edge)
    {
    case 0:
        return p.x >= wxmin; // Left edge
    case 1:
        return p.x <= wxmax; // Right edge
    case 2:
        return p.y >= wymin; // Bottom edge
    case 3:
        return p.y <= wymax; // Top edge
    }
    return false; // Should not happen
}

// Helper function to compute the intersection point of a line segment (p1, p2) with a clipping edge
// edge: 0-left, 1-right, 2-bottom, 3-top
Point computeIntersection(Point p1, Point p2, int edge)
{
    Point ip; // Intersection point
    double m; // Slope

    if (p2.x - p1.x == 0)
    {             // Vertical line segment
        m = 1e10; // Effectively infinite slope
    }
    else
    {
        m = (p2.y - p1.y) / (p2.x - p1.x);
    }

    switch (edge)
    {
    case 0: // Left edge (x = wxmin)
        ip.x = wxmin;
        ip.y = p1.y + (wxmin - p1.x) * m;
        break;
    case 1: // Right edge (x = wxmax)
        ip.x = wxmax;
        ip.y = p1.y + (wxmax - p1.x) * m;
        break;
    case 2: // Bottom edge (y = wymin)
        ip.y = wymin;
        // Handle horizontal line segment
        if (p2.y - p1.y == 0)
        {
            ip.x = p1.x;
        }
        else
        {
            ip.x = p1.x + (wymin - p1.y) / m;
        }
        break;
    case 3: // Top edge (y = wymax)
        ip.y = wymax;
        // Handle horizontal line segment
        if (p2.y - p1.y == 0)
        {
            ip.x = p1.x;
        }
        else
        {
            ip.x = p1.x + (wymax - p1.y) / m;
        }
        break;
    }
    return ip;
}

// Sutherland-Hodgman Polygon Clipping function
vector<Point> sutherlandHodgman(const vector<Point> &input_polygon, int edge)
{
    vector<Point> output_polygon;
    int num_vertices = input_polygon.size();

    if (num_vertices == 0)
    {
        return output_polygon; // Return empty if input is empty
    }

    Point p1 = input_polygon.back(); // Start with the last vertex

    for (int i = 0; i < num_vertices; ++i)
    {
        Point p2 = input_polygon[i];

        bool p1_inside = isInside(p1, edge);
        bool p2_inside = isInside(p2, edge);

        if (p1_inside && p2_inside)
        {
            // Case 1: Both points inside - add p2 to output
            output_polygon.push_back(p2);
        }
        else if (p1_inside && !p2_inside)
        {
            // Case 2: p1 inside, p2 outside - add intersection point
            output_polygon.push_back(computeIntersection(p1, p2, edge));
        }
        else if (!p1_inside && p2_inside)
        {
            // Case 3: p1 outside, p2 inside - add intersection point and p2
            output_polygon.push_back(computeIntersection(p1, p2, edge));
            output_polygon.push_back(p2);
        }
        // Case 4: Both points outside - add nothing

        p1 = p2; // Move to the next edge
    }
    return output_polygon;
}

// Function to perform the full clipping process against all four edges
void clipPolygon()
{
    if (original_polygon.empty())
    {
        cout << "No polygon to clip. Draw a polygon first." << endl;
        return;
    }

    // Start with the original polygon as input
    vector<Point> temp_polygon = original_polygon;

    // Clip against each edge in sequence
    temp_polygon = sutherlandHodgman(temp_polygon, 0); // Clip against Left
    temp_polygon = sutherlandHodgman(temp_polygon, 1); // Clip against Right
    temp_polygon = sutherlandHodgman(temp_polygon, 2); // Clip against Bottom
    temp_polygon = sutherlandHodgman(temp_polygon, 3); // Clip against Top

    clipped_polygon = temp_polygon; // Store the final clipped polygon

    // Request redraw to show the clipped polygon
    glutPostRedisplay();
}

// Display callback function
void display()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the buffer

    // Draw the clipping window
    drawClippingWindow();

    // Draw the original polygon if not clipping
    if (!drawing_polygon && clipped_polygon.empty() && !original_polygon.empty())
    {
        drawPolygon(original_polygon, 0.2, 0.2, 1.0); // Blue color for original
    }
    // Draw the polygon being drawn
    else if (drawing_polygon && !original_polygon.empty())
    {
        drawPolygon(original_polygon, 0.5, 0.5, 0.5); // Gray color while drawing
    }
    // Draw the clipped polygon if available
    else if (!clipped_polygon.empty())
    {
        drawPolygon(clipped_polygon, 1.0, 0.0, 0.0); // Red color for clipped
    }

    glFlush(); // Ensure drawing commands are executed
}

// Mouse callback function
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (drawing_polygon)
        {
            // Add the clicked point to the original polygon vertices
            original_polygon.push_back({(double)x, (double)(480 - y)});
            cout << "Added point: (" << x << ", " << (480 - y) << ")" << endl;
            glutPostRedisplay(); // Redraw to show the point/polygon outline
        }
    }
}

// Keyboard callback function
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'd': // Press 'd' to finish drawing the polygon
    case 'D':
        if (drawing_polygon && original_polygon.size() > 2)
        {
            drawing_polygon = false;
            clipped_polygon.clear(); // Clear any previous clipped polygon
            cout << "Finished drawing polygon with " << original_polygon.size() << " vertices." << endl;
            glutPostRedisplay(); // Redraw the complete original polygon
        }
        else if (drawing_polygon && original_polygon.size() <= 2)
        {
            cout << "Polygon must have at least 3 vertices." << endl;
        }
        break;
    case 'c': // Press 'c' to clip the polygon
    case 'C':
        if (!drawing_polygon)
        {
            clipPolygon();
        }
        else
        {
            cout << "Finish drawing the polygon before clipping (press 'd')." << endl;
        }
        break;
    case 'r': // Press 'r' to reset
    case 'R':
        original_polygon.clear();
        clipped_polygon.clear();
        drawing_polygon = false;
        cout << "Resetting. Click to start drawing a new polygon." << endl;
        glutPostRedisplay(); // Clear the screen and redraw the window
        break;
    case 'q': // Press 'q' to quit
    case 'Q':
        exit(0);
        break;
    }
}

// Menu callback function
void goMenu(int value)
{
    switch (value)
    {
    case 1: // Start Drawing Polygon
        original_polygon.clear();
        clipped_polygon.clear();
        drawing_polygon = true;
        cout << "Starting to draw a new polygon. Left-click to add vertices." << endl;
        glutPostRedisplay(); // Clear the screen
        break;
    case 2: // Clip Polygon
        if (!drawing_polygon)
        {
            clipPolygon();
        }
        else
        {
            cout << "Finish drawing the polygon before clipping (press 'd')." << endl;
        }
        break;
    case 3: // Reset
        original_polygon.clear();
        clipped_polygon.clear();
        drawing_polygon = false;
        cout << "Resetting. Click to start drawing a new polygon." << endl;
        glutPostRedisplay(); // Clear the screen and redraw the window
        break;
    case 4: // Exit
        exit(0);
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Sutherland-Hodgman Polygon Clipping");

    init(); // Initialize OpenGL settings

    glutDisplayFunc(display);   // Set the display callback function
    glutMouseFunc(mouse);       // Set the mouse callback function
    glutKeyboardFunc(keyboard); // Set the keyboard callback function

    // Create a right-click menu
    glutCreateMenu(goMenu);
    glutAddMenuEntry("Start Drawing Polygon", 1);
    glutAddMenuEntry("Clip Polygon", 2);
    glutAddMenuEntry("Reset", 3);
    glutAddMenuEntry("Exit", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON); // Attach menu to right mouse button

    cout << "Left-click to add polygon vertices." << endl;
    cout << "Press 'd' to finish drawing the polygon." << endl;
    cout << "Press 'c' to clip the polygon." << endl;
    cout << "Press 'r' to reset." << endl;
    cout << "Press 'q' to quit." << endl;
    cout << "Right-click for menu options." << endl;

    glutMainLoop(); // Enter the GLUT event processing loop

    return 0;
}
