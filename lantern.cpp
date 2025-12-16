#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Animation control
float angle = 0.0f; 

// Geometry setup
const float CENTRAL_SCALE = 4.0f;
const float SATELLITE_SCALE = 1.2f;
const float SPOKE_RADIUS = 5.5f;             
const float SUSPENSION_HEIGHT = 3.0f;        
const float CENTRAL_ATTACH_Y = -CENTRAL_SCALE; 

struct Color {
    float r, g, b;
};

// Color palette
Color pink = {1.0f, 0.0f, 0.5f};
Color yellow = {1.0f, 1.0f, 0.0f};
Color orange = {1.0f, 0.5f, 0.0f};
Color green = {0.0f, 0.8f, 0.4f};
Color blue = {0.2f, 0.2f, 1.0f};
Color teal = {0.0f, 0.8f, 0.8f};
Color red = {1.0f, 0.1f, 0.1f};
Color shine_c = {1.0f, 0.9f, 0.2f}; // Gem/Embellishment color

// Octahedron Vertices (Relative to center (0,0,0) scaled to 1.0)
const float V_TOP[3] = {0.0f, 1.0f, 0.0f};
const float V_BOTTOM[3] = {0.0f, -1.0f, 0.0f};
const float V1[3] = {1.0f, 0.0f, 1.0f};
const float V2[3] = {-1.0f, 0.0f, 1.0f};
const float V3[3] = {-1.0f, 0.0f, -1.0f};
const float V4[3] = {1.0f, 0.0f, -1.0f};

// Macro for triangle faces
#define DRAW_FACE(p1, p2, p3) \
    glVertex3fv(p1); glVertex3fv(p2); glVertex3fv(p3);

// --- Drawing Helper Functions ---

void drawCubeDetail(float size, Color c) {
    glColor3f(c.r, c.g, c.b);
    glutSolidCube(size);
}

void drawStreamers(float length, Color c) {
    glLineWidth(2.0f);
    glColor3f(c.r, c.g, c.b);
    
    float sway_x = sin(angle * 0.1f) * 0.1f;
    float sway_z = cos(angle * 0.1f) * 0.1f;

    glBegin(GL_LINES);
    for(int i = 0; i < 20; i++) {
        float x_off = ((i % 5) / 5.0f * 0.4f) - 0.2f + sway_x;
        float z_off = ((i % 4) / 4.0f * 0.4f) - 0.2f + sway_z;
        
        glVertex3f(x_off, 0.0f, z_off);
        glVertex3f(x_off * 1.5f, -length, z_off * 1.5f);
    }
    glEnd();
}

void drawLantern(float x, float y, float z, float scale, Color c, float rotation_offset) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    glRotatef(angle + rotation_offset, 0.0f, 1.0f, 0.0f); 
    glScalef(scale, scale, scale);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // --- Octahedron Faces (Lantern Body) ---
    glColor4f(c.r, c.g, c.b, 0.8f); 
    glBegin(GL_TRIANGLES);
    
    // Top Pyramid
    DRAW_FACE(V_TOP, V2, V1);
    DRAW_FACE(V_TOP, V3, V2);
    DRAW_FACE(V_TOP, V4, V3);
    DRAW_FACE(V_TOP, V1, V4);

    // Bottom Pyramid
    DRAW_FACE(V_BOTTOM, V1, V2);
    DRAW_FACE(V_BOTTOM, V2, V3);
    DRAW_FACE(V_BOTTOM, V3, V4);
    DRAW_FACE(V_BOTTOM, V4, V1);
    
    glEnd();
    
    // --- Inner Panel Design (Simplified triangles) ---
    Color detail_c = (c.r > 0.5f && c.g > 0.5f) ? red : yellow; 
    glColor4f(detail_c.r, detail_c.g, detail_c.b, 0.9f);
    
    glBegin(GL_TRIANGLES);
        // Face 1 (Top)
        glVertex3f(0.0f, 0.75f, 0.0f); glVertex3f(0.3f, 0.3f, 0.1f); glVertex3f(0.1f, 0.3f, 0.3f); 
        // Face 2 (Top)
        glVertex3f(0.0f, 0.75f, 0.0f); glVertex3f(-0.3f, 0.3f, 0.1f); glVertex3f(-0.1f, 0.3f, 0.3f); 
    glEnd();

    // --- Wireframe ---
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    
    glBegin(GL_LINES);
    // All 12 edges
    glVertex3fv(V_TOP); glVertex3fv(V1);
    glVertex3fv(V_TOP); glVertex3fv(V2);
    glVertex3fv(V_TOP); glVertex3fv(V3);
    glVertex3fv(V_TOP); glVertex3fv(V4);
    glVertex3fv(V_BOTTOM); glVertex3fv(V1);
    glVertex3fv(V_BOTTOM); glVertex3fv(V2);
    glVertex3fv(V_BOTTOM); glVertex3fv(V3);
    glVertex3fv(V_BOTTOM); glVertex3fv(V4);
    glVertex3fv(V2); glVertex3fv(V1);
    glVertex3fv(V1); glVertex3fv(V4);
    glVertex3fv(V4); glVertex3fv(V3);
    glVertex3fv(V3); glVertex3fv(V2);
    glEnd();
    
    glDisable(GL_BLEND);
    
    // --- Vertex Embellishments (Gems/Cubes) ---
    float detail_size = 0.15f / scale; 

    // Equator Cubes
    glPushMatrix(); glTranslatef(V1[0], V1[1], V1[2]); drawCubeDetail(detail_size, shine_c); glPopMatrix();
    glPushMatrix(); glTranslatef(V2[0], V2[1], V2[2]); drawCubeDetail(detail_size, shine_c); glPopMatrix();
    glPushMatrix(); glTranslatef(V3[0], V3[1], V3[2]); drawCubeDetail(detail_size, shine_c); glPopMatrix();
    glPushMatrix(); glTranslatef(V4[0], V4[1], V4[2]); drawCubeDetail(detail_size, shine_c); glPopMatrix();

    // Top/Bottom Spheres
    glPushMatrix(); 
        glTranslatef(V_TOP[0], V_TOP[1], V_TOP[2]); 
        glColor3f(shine_c.r, shine_c.g, shine_c.b);
        glutSolidSphere(detail_size * 1.5f, 10, 10);
    glPopMatrix();
    
    glPushMatrix(); 
        glTranslatef(V_BOTTOM[0], V_BOTTOM[1], V_BOTTOM[2]); 
        glColor3f(shine_c.r, shine_c.g, shine_c.b);
        glutSolidSphere(detail_size * 1.5f, 10, 10);
    glPopMatrix();

    // --- Streamers ---
    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f);
    drawStreamers(4.0f / scale, c);
    glPopMatrix();
    
    float sideTailLength = 2.5f / scale;
    glPushMatrix(); glTranslatef(V1[0], V1[1], V1[2]); drawStreamers(sideTailLength, c); glPopMatrix();
    glPushMatrix(); glTranslatef(V2[0], V2[1], V2[2]); drawStreamers(sideTailLength, c); glPopMatrix();
    glPushMatrix(); glTranslatef(V3[0], V3[1], V3[2]); drawStreamers(sideTailLength, c); glPopMatrix();
    glPushMatrix(); glTranslatef(V4[0], V4[1], V4[2]); drawStreamers(sideTailLength, c); glPopMatrix();
    
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Camera centering
    const float TOP_Y = CENTRAL_SCALE + SUSPENSION_HEIGHT;
    const float BOTTOM_Y = CENTRAL_ATTACH_Y;
    const float VERTICAL_CENTER = (TOP_Y + BOTTOM_Y) / 2.0f; 
    
    gluLookAt(0.0, VERTICAL_CENTER, 28.0, 
              0.0, VERTICAL_CENTER, 0.0,  
              0.0, 1.0, 0.0);      

    // Suspension Rod
    float top_attach_y = CENTRAL_SCALE;
    float ceiling_y = top_attach_y + SUSPENSION_HEIGHT; 
    
    glLineWidth(5.0f);
    glColor3f(0.0f, 0.0f, 0.0f); 
    glBegin(GL_LINES);
    glVertex3f(0.0f, ceiling_y, 0.0f); 
    glVertex3f(0.0f, top_attach_y, 0.0f);
    glEnd();

    glRotatef(angle, 0.0f, 1.0f, 0.0f); 

    // Central Lantern
    drawLantern(0.0f, 0.0f, 0.0f, CENTRAL_SCALE, yellow, 0.0f);

    // Satellites
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f); 

    std::vector<Color> satellite_colors = {pink, orange, green, blue, teal, red};
    int num_satellites = satellite_colors.size();
    
    float central_attach_x = 0.0f;
    float central_attach_y = CENTRAL_ATTACH_Y; 
    float central_attach_z = 0.0f;
    
    for (int i = 0; i < num_satellites; ++i) {
        float initial_offset = (float)i * (360.0f / num_satellites);
        float orbit_rad = initial_offset * (float)M_PI / 180.0f; 
        
        float x = central_attach_x + SPOKE_RADIUS * cos(orbit_rad);
        float z = central_attach_z + SPOKE_RADIUS * sin(orbit_rad);
        float y = central_attach_y; 

        // Spoke
        glBegin(GL_LINES);
        glVertex3f(central_attach_x, central_attach_y, central_attach_z);
        glVertex3f(x, y, z);
        glEnd();

        // Satellite Lantern
        drawLantern(x, y, z, SATELLITE_SCALE, satellite_colors[i], i * 30.0f);
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 150.0f); 
    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    angle += 0.5f;
    if (angle > 360.0f) angle -= 360.0f;
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1024, 768); 
    glutCreateWindow("3D vesak lantern");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE); 
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    srand(123);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}