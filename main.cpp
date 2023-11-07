#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <GL/glu.h>
#include "helpFile.hpp"
using namespace sf;

float angleX, angleY; // Углы поворота камеры

class Map
{
public:
    int maxX, maxY, maxZ;
    int minX, minY, minZ;

    Map(int x0, int y0, int z0) 
    {
        maxX = x0; maxY = y0; maxZ = z0;
        minX = 0; minY = 0; minZ = 0;
    }
};

class Player
{
public:
    float x, y, z;
    float dx, dy, dz;
    float w, h, d;  // width, height, depth 
    bool onGround;
    float speed;
    bool needJump;
   

    Player(float x0, float y0, float z0)
    {
        x = x0; y = y0; z = z0;
        dx = 0; dy = 0; dz = 0;
        w = 5; h = 10; d = 5; speed = 5;
        onGround = false;
        needJump = false;
        
    }

    void update(float time, Map map)
    {
        needJump = false;
        if (!onGround) dy -= 1.5 * time;
        onGround = 0;

        x += dx * time;
        collision(dx, 0, 0, map);
        y += dy * time;
        collision(0, dy, 0, map);
        z += dz * time;
        collision(0, 0, dz, map);

        dx = dz = 0;


    }

    void collision(float Dx, float Dy, float Dz, Map map)
    {
        float minX = map.minX; // Минимальная X-координата карты
        float minY = map.minY; // Минимальная Y-координата карты
        float minZ = map.minZ; // Минимальная Z-координата карты
        float maxX = map.maxX * size; // Максимальная X-координата карты
        float maxY = map.maxY * size; // Максимальная Y-координата карты
        float maxZ = map.maxZ * size; // Максимальная Z-координата карты

        // Проверьте, не выходит ли персонаж за границы карты
        if (x - w + Dx < minX) {
            x = minX + w;
 
        }
        if (x + w + Dx > maxX) {
            x = maxX - w;

        }
        if (y - h + Dy < minY) {
            y = minY + h;
            onGround = true;
            dy = 0;
        }
        if (z - d + Dz < minZ) {
            z = minZ + d;

        }
        if (z + d + Dz > maxZ) {
            z = maxZ - d;
        }

        for (int X = (x - w) / size; X < (x + w) / size; X++)
            for (int Y = (y - h) / size; Y < (y + h) / size; Y++)
                for (int Z = (z - d) / size; Z < (z + d) / size; Z++)
                    if (check(X, Y, Z)) {
                        if (Dx > 0) {
                            x = X * size - w; needJump = true;
                        }
                        if (Dx < 0) {
                            x = X * size + size + w; needJump = true;
                        }
                        if (Dy > 0)  y = Y * size - h;
                        if (Dy < 0) { y = Y * size + size + h; onGround = true; dy = 0; }
                        if (Dz > 0) { z = Z * size - d; needJump = true; }
                        if (Dz < 0) { z = Z * size + size + d; needJump = true; }
                    }
    }

    void keyboard()
    {
        if (Keyboard::isKeyPressed(Keyboard::Space)) if (onGround) { onGround = false; dy = 12; };

        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            dx = -sin(angleX / 180 * PI) * speed;
            dz = -cos(angleX / 180 * PI) * speed;
            if (needJump) {
                onGround = false; dy = 12;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            dx = sin(angleX / 180 * PI) * speed;
            dz = cos(angleX / 180 * PI) * speed;
            if (needJump) {
                onGround = false; dy = 12;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            dx = sin((angleX + 90) / 180 * PI) * speed;
            dz = cos((angleX + 90) / 180 * PI) * speed;
            if (needJump) {
                onGround = false; dy = 12;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            dx = sin((angleX - 90) / 180 * PI) * speed;
            dz = cos((angleX - 90) / 180 * PI) * speed;
            if (needJump) {
                onGround = false; dy = 12;
            }
        }
    }

};



int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);

    // activate the window
    window.setActive(true);

    // load resources, initialize the OpenGL states, ...

    ///////текстуры///////
    Texture t;
    t.loadFromFile("resources/cursor.png");
    Sprite s(t); s.setOrigin(8, 8); s.setPosition(400, 300);


    GLuint skybox[6];
    skybox[0] = LoadTexture("resources/skybox4/front.png");
    skybox[1] = LoadTexture("resources/skybox4/back.png");
    skybox[2] = LoadTexture("resources/skybox4/left.png");
    skybox[3] = LoadTexture("resources/skybox4/right.png");
    skybox[4] = LoadTexture("resources/skybox4/bottom.png");
    skybox[5] = LoadTexture("resources/skybox4/top.png");


    GLuint box[6];
    box[0] = LoadTexture("resources/textures/sand.jpg");
    box[1] = LoadTexture("resources/textures/sand.jpg");    
    box[2] = LoadTexture("resources/textures/sand.jpg"); 
    box[3] = LoadTexture("resources/textures/sand.jpg"); 
    box[4] = LoadTexture("resources/textures/sand.jpg"); 
    box[5] = LoadTexture("resources/textures/sand.jpg");
    //////////////////////////

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClearDepth(1.f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.f, 1.f, 1.f, 2000.f);
    glEnable(GL_TEXTURE_2D);

    ShowCursor(FALSE);

    ////карта высот////
    Image im;  im.loadFromFile("resources/heightmap.png");

    for (int x = 0; x < 256; x++)
        for (int z = 0; z < 256; z++)
        {
            int c = im.getPixel(x, z).r / 15;
            for (int y = 0; y < c; y++)
                if (y > c - 3) mass[x][y][z] = 1;
        }
    Clock clock;
    // run the main loop
    bool running = true;

    bool mLeft = 0, mRight = 0; // mouse buttons

    Map map(100, 60, 100);
    Player p(100, 200, 100);
   

    while (window.isOpen())
    {

        float time = clock.getElapsedTime().asMilliseconds();
        clock.restart();
        time = time / 50;
        if (time > 3) time = 3;

        // handle events
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if ((event.type == Event::KeyPressed) && (event.key.code == Keyboard::Escape))
                window.close();

            if (event.type == Event::MouseButtonPressed)
            {
                if (event.key.code == Mouse::Right) mRight = 1;
                if (event.key.code == Mouse::Left)  mLeft = 1;
            }
        }

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        p.keyboard();
        p.update(time, map);

        ////-----------------------
        POINT mousexy;
        GetCursorPos(&mousexy);
        int xt = window.getPosition().x + 400;
        int yt = window.getPosition().y + 300;

        angleX += (xt - mousexy.x) / 4; //4 — чувствительность 
        angleY += (yt - mousexy.y) / 4;

        if (angleY < -89.0) { angleY = -89.0; }
        if (angleY > 89.0) { angleY = 89.0; }

        SetCursorPos(xt, yt);
        ////----------------------



        if (mRight || mLeft)
        {
            float x = p.x;
            float y = p.y + p.h / 2;
            float z = p.z;

            int X, Y, Z, oldX, oldY, oldZ;
            int dist = 0;
            while (dist < 120)  // радиус действия
            {
                dist++;

                x += -sin(angleX / 180 * PI);    X = x / size;
                y += tan(angleY / 180 * PI);    Y = y / size;
                z += -cos(angleX / 180 * PI);    Z = z / size;

                if (check(X, Y, Z))
                    if (mLeft) { mass[X][Y][Z] = 0; break; }
                    else { mass[oldX][oldY][oldZ] = 1; break; }

                oldX = X; oldY = Y; oldZ = Z;
            }
        }

        mLeft = mRight = 0;


        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(p.x, p.y + p.h / 2, p.z, p.x - sin(angleX / 180 * PI), p.y + p.h / 2 + (tan(angleY / 180 * PI)), p.z - cos(angleX / 180 * PI), 0, 1, 0);

        glTranslatef(p.x, p.y, p.z);
        createBox(skybox, 1000);
        glTranslatef(-p.x, -p.y, -p.z);


        for (int x = map.minX; x < map.maxX; x++)
            for (int y = map.minY; y < map.maxY; y++)
                for (int z = map.minZ; z < map.maxZ; z++)
                {
                    if (!mass[x][y][z]) continue;
                    glTranslatef(x * size + size / 2, y * size + size / 2, z * size + size / 2);
                    createBox(box, size / 2);
                    glTranslatef(-x * size - size / 2, -y * size - size / 2, -z * size - size / 2);
                }

        window.pushGLStates();
        window.draw(s);      //рисуем курсор
        window.popGLStates();

        window.display();
    }

    // release resources...

    return 0;
}
