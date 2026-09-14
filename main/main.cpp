#include "box2d/box2d.h"
#include "raylib.h"
#include <cstdint>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <assert.h>

#include <vector>



#define WIDTH  640.0
#define HEIGHT 640.0
#define BOX_SIZE 5
#define BOX_DENCITY 25
#define RANDOM_SPREAD 75
#define RANDOM_COLOR (uint8_t)(GetRandomValue(0, 10) * 10 + 155)
#define GROUND_COUNT 60
#define BOX_COUNT 10
#define TEXT_SIZE 24
#define BUTTON_MARGIN 0

enum ShapeProperties : uint8_t {
    X
   ,Y
   ,RADIUS
};

enum Shape : uint8_t {
    RECTANGLE,
    TRIANGLE,
    CIRCLE,
    RECTANGLE_EX,
};

enum EntityType : uint8_t {
    NORMAL,
    KILLING,
    FINISH,
};

// enum : uint8_t {
//     STATIC,
//     MOVING,
//     ROTATING,
// };

struct Entity
{
    b2BodyId bodyId;
    // b2Vec2 extent;
    Color color;
    Shape shape;
    EntityType type;
    int shapeProperties[];

    Entity() {
        color = { RANDOM_COLOR, RANDOM_COLOR, RANDOM_COLOR, 255 };
    }
    // int GetProperty(int property) {
    //     return shapeProperties[property];
    // }
    // bool SetProperty() {
    //     return true;
    // }
    void Draw();
};

void DrawEntity(const Entity* entity)
{
    // The boxes were created centered on the bodies, but raylib draws textures starting at the top left corner.
    // b2Body_GetWorldPoint gets the top left corner of the box accounting for rotation.
    b2Vec2 p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2) { -entity->extent.x, -entity->extent.y });
    b2Rot rotation = b2Body_GetRotation(entity->bodyId);
    float radians = b2Rot_GetAngle(rotation);

    Vector2 ps = {p.x, p.y};

    DrawRectanglePro({p.x, p.y, BOX_SIZE, BOX_SIZE}, {0, 0}, RAD2DEG * radians, entity->color);

    // I used these circles to ensure the coordinates are correct
}

#define DEFAULT_COLOR_STATIC (uint8_t)(200, 200, 200)
#define RECTINGLE_SIZE 25

void GuiButton(const char* label, Vector2 coordinats, float width) {
    GuiButton({ coordinats.x, coordinats.y, width, TEXT_SIZE }, label);
}

struct Button
{
    const char* label;
    bool is_enabled = true;
};

void GuiMenu(Vector2 coordinats, std::vector<Button> buttons, float width) {
    GuiPanel({ coordinats.x, coordinats.y, width, (float)buttons.size() * (TEXT_SIZE + BUTTON_MARGIN) + BUTTON_MARGIN }, nullptr);
    for (int i = 0; i < buttons.size(); ++i)
    {
        if (buttons[i].is_enabled)
            GuiEnable();
        else
            GuiDisable();
        GuiButton(buttons[i].label, { coordinats.x + BUTTON_MARGIN
                                    , coordinats.y + (TEXT_SIZE + BUTTON_MARGIN) * i + BUTTON_MARGIN}
                                  , width - BUTTON_MARGIN * 2);
    }
}



int main(void)
{
    InitWindow(WIDTH, HEIGHT, "box2d-raylib");

    // GuiLoadStyle("resources/style.rgs");
    GuiSetStyle(BUTTON, TEXT_PADDING, 8);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, 0);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 1);

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

    SetTargetFPS(60);

    // 128 pixels per meter is a appropriate for this scene. The boxes are 128 pixels wide.
    float lengthUnitsPerMeter = 128.0f;
    b2SetLengthUnitsPerMeter(lengthUnitsPerMeter);

    b2WorldDef worldDef = b2DefaultWorldDef();

    // Realistic gravity is achieved by multiplying gravity by the length unit.
    worldDef.gravity.y = 9.8f * lengthUnitsPerMeter;
    b2WorldId worldId = b2CreateWorld(&worldDef);

    b2Vec2 groundExtent = { 0.5f * BOX_SIZE, 0.5f * BOX_SIZE };
    b2Vec2 boxExtent = { 0.5f * BOX_SIZE, 0.5f * BOX_SIZE };

    // These polygons are centered on the origin and when they are added to a body they
    // will be centered on the body position.
    b2Polygon groundPolygon = b2MakeBox(groundExtent.x, groundExtent.y);
    b2Polygon boxPolygon = b2MakeBox(boxExtent.x, boxExtent.y);

    Entity groundEntities[GROUND_COUNT] = { Entity() };
    for (int i = 0; i < GROUND_COUNT; ++i)
    {
        Entity* entity = groundEntities + i;
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = (b2Vec2){ groundExtent.x * 2 * i + 100.0f, (float)HEIGHT - groundExtent.y - 10.0f};

        // I used this rotation to test the world to screen transformation
        //bodyDef.rotation = b2MakeRot(0.25f * b2_pi * i);

        bodyDef.type = b2_kinematicBody;
        entity->bodyId = b2CreateBody(worldId, &bodyDef);
        entity->extent = groundExtent;
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(entity->bodyId, &shapeDef, &groundPolygon);
    }

    Entity boxEntities[BOX_COUNT] = { Entity() };
    int boxIndex = 0;
    for (int i = 0; i < 4; ++i)
    {
        float y = HEIGHT - groundExtent.y - 100.0f - (2.5f * i + 2.0f) * boxExtent.y - 20.0f;

        for (int j = i; j < 4; ++j)
        {
            float x = 0.5f * WIDTH + (3.0f * j - i - 3.0f) * boxExtent.x;
            assert(boxIndex < BOX_COUNT);

            Entity* entity = boxEntities + boxIndex;
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = (b2Vec2){ x, y };
            entity->bodyId = b2CreateBody(worldId, &bodyDef);
            entity->extent = boxExtent;
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            b2CreatePolygonShape(entity->bodyId, &shapeDef, &boxPolygon);

            boxIndex += 1;
        }
    }
    std::vector<Entity> userEntities;

    bool pause = false;


    Vector2 menu_coordinats;
    bool show_menu = false;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_P))
        {
            pause = !pause;
        }
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            b2Vec2 mouse_pos = { (float)GetMouseX(), (float)GetMouseY() };

            for (int i = 0; i < BOX_DENCITY; ++i)
            {
                float y = mouse_pos.y - 0.5 * boxExtent.y + GetRandomValue(-RANDOM_SPREAD, RANDOM_SPREAD);
                float x = mouse_pos.x - 0.5 * boxExtent.x + GetRandomValue(-RANDOM_SPREAD, RANDOM_SPREAD);

                Entity entity = Entity();
                b2BodyDef bodyDef = b2DefaultBodyDef();
                bodyDef.type = b2_dynamicBody;
                bodyDef.position = (b2Vec2){ x, y };
                entity.bodyId = b2CreateBody(worldId, &bodyDef);
                entity.extent = boxExtent;
                b2ShapeDef shapeDef = b2DefaultShapeDef();
                b2CreatePolygonShape(entity.bodyId, &shapeDef, &boxPolygon);

                userEntities.push_back(entity);
            }
        }
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            menu_coordinats =  { (float)GetMouseX(), (float)GetMouseY() };

            show_menu = true;
        }
        else if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            show_menu = false;
        }

        if (pause == false)
        {
            float deltaTime = GetFrameTime();
            b2World_Step(worldId, deltaTime, 4);
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        const char* message = "Hello Box2D!";
        int fontSize = 36;
        int textWidth = MeasureText("Hello Box2D!", fontSize);
        DrawText(message, (WIDTH - textWidth) / 2, 50, fontSize, LIGHTGRAY);

        static float step = 0;
        static bool da = false;
        step++;
        for (int i = 0; i < GROUND_COUNT; ++i)
        {
            Entity* cur_ent = groundEntities + i;
            if (step < 20) {
                if (da == false)
                    b2Body_SetLinearVelocity( cur_ent->bodyId, { 0.0f, -1000.0f } );
                else
                    b2Body_SetLinearVelocity( cur_ent->bodyId, { 0.0f, 1000.0f } );
            }
            else {
                da = !da;
                step = 0;
            }
            // static float timeStep = 1.0f / 60.0f;
            // b2Transform target = { { (float)i * BOX_SIZE, step }, 0 };
            // b2Body_SetTargetTransform( cur_ent->bodyId, target, timeStep);

            // b2Body_SetType( m_platformId, b2_kinematicBody );
            // b2Body_SetAngularVelocity( m_platformId, 0.0f );

            // b2Body_SetType( m_secondAttachmentId, b2_kinematicBody );
            // b2Body_SetLinearVelocity( m_secondAttachmentId, b2Vec2_zero );
            // b2Body_SetAngularVelocity( m_secondAttachmentId, 0.0f );

            // b2Body_SetType( m_secondPayloadId, b2_kinematicBody );
            // b2Body_SetType( m_touchingBodyId, b2_kinematicBody );
            // b2Body_SetType( m_floatingBodyId, b2_kinematicBody );


            // b2Vec2 p = b2Body_GetWorldPoint(cur_ent->bodyId, (b2Vec2) { -cur_ent->extent.x, -cur_ent->extent.y });

            // Vector2 ps = {p.x, p.y};
            DrawEntity(groundEntities + i);
        }

        for (int i = 0; i < BOX_COUNT; ++i)
        {
            DrawEntity(boxEntities + i);
        }
        for (auto e : userEntities)
        {
            DrawEntity(&e);
        }

        if (show_menu) {
            std::vector<Button> buttons {
                { "Paste", 0 }
               ,{ "Rectangle" }
               ,{ "Triangle" }
               ,{ "Circle" }
            };
            GuiMenu(menu_coordinats, buttons, TEXT_SIZE * 8);
                // GuiGroupBox({menu_coordinats.x, menu_coordinats.y, 60, 60}, "lol");
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
