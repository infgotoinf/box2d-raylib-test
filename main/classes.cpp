#include "include/classes.hpp"


#include "box2d/box2d.h"
#include "box2d/types.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "include/config.hpp"



namespace LBR
{
    EntityRectangle::EntityRectangle(b2WorldId world_id, EntityBehaviour behaviour, EntityType type, float x, float y, float width, float height, Color color)
            : x(x), y(y), width(width), height(height)
    {
        this->color = color;
        this->behaviour = behaviour;
        this->type = type;
        this->shape = RECTANGLE;

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = (b2Vec2){ this->x, this->y };

        b2Polygon polygon = b2MakeBox(width * 0.5f, height * 0.5f);

        switch (type) {
        case STATIC:
            bodyDef.type = b2_kinematicBody;
            break;
        default:
            bodyDef.type = b2_dynamicBody;
        }

        bodyId = b2CreateBody(world_id, &bodyDef);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
    }


    void EntityRectangle::Draw()
    {
        // The boxes were created centered on the bodies, but raylib draws textures starting at the top left corner.
        // b2Body_GetWorldPoint gets the top left corner of the box accounting for rotation.
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, (b2Vec2) { -width / 2, -height / 2 });
        b2Rot rotation = b2Body_GetRotation(bodyId);
        float radians = b2Rot_GetAngle(rotation);

        DrawRectanglePro({p.x, p.y, width, height}, {0, 0}, RAD2DEG * radians, color);
    }


    void EntityRectangle::ChangeCoordinats(float x, float y)
    {
        this->x = x;
        this->y = y;
    }


    World::World()
    {
        // 128 pixels per meter is a appropriate for this scene. The boxes are 128 pixels wide.
        float lengthUnitsPerMeter = 128.0f;
        b2SetLengthUnitsPerMeter(lengthUnitsPerMeter);

        b2WorldDef worldDef = b2DefaultWorldDef();

        // Realistic gravity is achieved by multiplying gravity by the length unit.
        worldDef.gravity.y = 9.8f * lengthUnitsPerMeter;
        world_id = b2CreateWorld(&worldDef);
    }


    World::~World() {
        b2DestroyWorld(world_id);
    }


    World* World::GetIstance()
    {
        if (instance == nullptr) {
            instance = new World();
        }
        return instance;
    }


    void World::CreateWindow()
    {
        SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lucky Ball Race");

        GuiSetStyle(BUTTON, TEXT_PADDING, 8);
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, 0);
        GuiSetStyle(BUTTON, BORDER_WIDTH, 1);

        SetTargetFPS(60);
    }


    void World::CopyEntity(const float x, const float y)
    {
        copy_buffer->ChangeCoordinats(x, y);
    }


    void World::PasteEntity(const float x, const float y)
    {
        copy_buffer->ChangeCoordinats(x, y);
    }


    void World::SpawnRectangle(const float x, const float y)
    {
        entities.push_back(
                std::make_unique<EntityRectangle>(
                          world_id
                        , LBR::NORMAL
                        , LBR::STATIC
                        , x
                        , y
                        , DEFAULT_RECTANGLE_WIDTH
                        , DEFAULT_RECTANGLE_HEIGHT
        ));
    }


    void World::DrawEntities()
    {
        for (auto &entity : entities)
        {
            switch (entity->shape)
            {
            case RECTANGLE:
                dynamic_cast<EntityRectangle*>(entity.get())->Draw();
                break;
            case TRIANGLE:
                break;
            case CIRCLE:
                break;
            default:
                exit(1);
            }
        }
    }


    bool World::GuiMenu(const std::vector<Button> buttons, const float x, const float y, const float width)
    {
        GuiPanel({ x, y, width, (float)buttons.size() * (GUI_TEXT_SIZE + GUI_BUTTON_MARGIN) + GUI_BUTTON_MARGIN }, nullptr);

        bool button_was_clicked = false;
        for (int i = 0; i < buttons.size(); ++i)
        {
            Rectangle button_collision = { x + GUI_BUTTON_MARGIN
                                         , y + (GUI_TEXT_SIZE + GUI_BUTTON_MARGIN) * i + GUI_BUTTON_MARGIN
                                         , width - GUI_BUTTON_MARGIN * 2
                                         , GUI_TEXT_SIZE };
            if (buttons[i].is_enabled)
                GuiEnable();
            else
                GuiDisable();

            if (GuiButton({ x + GUI_BUTTON_MARGIN
                          , y + (GUI_TEXT_SIZE + GUI_BUTTON_MARGIN) * i + GUI_BUTTON_MARGIN
                          , width - GUI_BUTTON_MARGIN * 2
                          , GUI_TEXT_SIZE }
                          , buttons[i].label))
            {
                (this->*buttons[i].action)(x, y);
                button_was_clicked = true;
            }
        }
        return button_was_clicked;
    }
}
