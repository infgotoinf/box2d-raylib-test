#include "include/classes.hpp"


#include "box2d/box2d.h"
#include "box2d/math_functions.h"
#include "raylib.h"
#include <cmath>
#include <memory>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "include/config.hpp"



namespace LBR
{
    void DefineType(b2BodyDef* bodyDef, EntityType type) {
        switch (type) {
        case STATIC:
            bodyDef->type = b2_kinematicBody;
            break;
        default:
            bodyDef->type = b2_dynamicBody;
            // Can make things slow if overused, tho I'm not planing to rely on
            // it that much
            bodyDef->isBullet = true;
        }
    }

    EntityRectangle::EntityRectangle(b2WorldId world_id, EntityBehaviour behaviour, EntityType type, float x, float y, float width, float height, Color color)
            : width(width), height(height)
    {
        this->color = color;
        this->behaviour = behaviour;
        this->type = type;
        this->shape = RECTANGLE;

        b2BodyDef bodyDef = b2DefaultBodyDef();

        bodyDef.position = (b2Vec2){ x, y };

        b2Polygon polygon = b2MakeBox(width * 0.5f, height * 0.5f);

        DefineType(&bodyDef, type);

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


    void EntityRectangle::DrawBorder()
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, (b2Vec2) { -width / 2, -height / 2 });
        b2Vec2 p_border = b2Body_GetWorldPoint(bodyId, (b2Vec2) { -(width + ENTITY_BORDER_SIZE) / 2, -(height + ENTITY_BORDER_SIZE) / 2 });
        b2Rot rotation = b2Body_GetRotation(bodyId);
        float radians = b2Rot_GetAngle(rotation);

        DrawRectanglePro({p_border.x, p_border.y, width + ENTITY_BORDER_SIZE / 2, height + ENTITY_BORDER_SIZE / 2}, {0, 0}, RAD2DEG * radians, COLOR_ENTITY_BORDER);
        DrawRectanglePro({p.x, p.y, width - ENTITY_BORDER_SIZE / 2, height - ENTITY_BORDER_SIZE / 2}, {0, 0}, RAD2DEG * radians, color);
    }


    void EntityRectangle::ChangeCoordinats(float x, float y)
    {
        // this->x = x;
        // this->y = y;
    }



    EntityTriangle::EntityTriangle(b2WorldId world_id, EntityBehaviour behaviour, EntityType type, Vector2 v1, Vector2 v2, Vector2 v3, Color color)
            : v2({v2.x - v1.x, v2.y - v1.y}), v3({v3.x - v1.x, v3.y - v1.y})
    {
        this->color = color;
        this->behaviour = behaviour;
        this->type = type;
        this->shape = TRIANGLE;
        this->centroid = { (this->v2.x + this->v3.x)/3, (this->v2.y + this->v3.y)/3 };

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = (b2Vec2){ v1.x, v1.y };


        b2Vec2 points[] = {{0, 0}, {this->v2.x, this->v2.y}, {this->v3.x, this->v3.y}};
        b2Hull hull = b2ComputeHull(points, 3);
        float radius = 0.0f;
        b2Polygon polygon = b2MakePolygon(&hull, radius);

        DefineType(&bodyDef, type);

        bodyId = b2CreateBody(world_id, &bodyDef);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
    }


    void EntityTriangle::Draw()
    {
        b2Vec2 p1 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });
        b2Vec2 p2 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v2.x, v2.y });
        b2Vec2 p3 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v3.x, v3.y });

        DrawTriangle({p1.x, p1.y}, {p2.x, p2.y}, {p3.x, p3.y}, color);
        DrawTriangle({p2.x, p2.y}, {p1.x, p1.y}, {p3.x, p3.y}, color);
        DrawTriangle({p3.x, p3.y}, {p1.x, p1.y}, {p2.x, p2.y}, color);
    }


    void EntityTriangle::DrawBorder()
    {
        b2Vec2 p1 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });
        b2Vec2 p2 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v2.x, v2.y });
        b2Vec2 p3 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v3.x, v3.y });

        // Lines from center to points. Using Pythagoras formula.
        float c_p1 = std::sqrt(centroid.x * centroid.x + 0 * 0);
        float c_p2 = std::sqrt(centroid.x * centroid.x + v2.y * v2.y);
        float c_p3 = std::sqrt(centroid.x * centroid.x + v3.y * v3.y);

        // Lines from center to borders.
        float c_b1 = (c_p1 + ENTITY_BORDER_SIZE) / c_p1;
        float c_b2 = (c_p2 + ENTITY_BORDER_SIZE) / c_p2;
        float c_b3 = (c_p3 + ENTITY_BORDER_SIZE) / c_p3;

        b2Vec2 b1 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { (0    - centroid.x) * c_b1, (0    - centroid.y) * c_b1 });
        b2Vec2 b2 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { (v2.x - centroid.x) * c_b2, (v2.y - centroid.y) * c_b2 });
        b2Vec2 b3 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { (v3.x - centroid.x) * c_b3, (v3.y - centroid.y) * c_b3 });

        DrawTriangle({b1.x, b1.y}, {b2.x, b2.y}, {b3.x, b3.y}, COLOR_ENTITY_BORDER);
        DrawTriangle({b2.x, b2.y}, {b1.x, b1.y}, {b3.x, b3.y}, COLOR_ENTITY_BORDER);
        DrawTriangle({b3.x, b3.y}, {b1.x, b1.y}, {b2.x, b2.y}, COLOR_ENTITY_BORDER);

        DrawTriangle({p1.x, p1.y}, {p2.x, p2.y}, {p3.x, p3.y}, color);
        DrawTriangle({p2.x, p2.y}, {p1.x, p1.y}, {p3.x, p3.y}, color);
        DrawTriangle({p3.x, p3.y}, {p1.x, p1.y}, {p2.x, p2.y}, color);
    }


    void EntityTriangle::ChangeCoordinats(float x, float y)
    {
        // this->v1 = {x, y};
    }


    EntityCircle::EntityCircle(b2WorldId world_id, EntityBehaviour behaviour, EntityType type, float x, float y, float radius, Color color)
            : radius(radius)
    {
        this->color = color;
        this->behaviour = behaviour;
        this->type = type;
        this->shape = CIRCLE;

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = (b2Vec2){ x, y };

        b2Circle circle{{0,0}, radius};

        DefineType(&bodyDef, type);

        bodyId = b2CreateBody(world_id, &bodyDef);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreateCircleShape(bodyId, &shapeDef, &circle);
    }


    void EntityCircle::Draw()
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });

        // For some reason without ADDITIONAL_CIRCLE_SIZE physical body size of a
        // circle seems to be a little bigger that what's rendered.
        constexpr static float ADDITIONAL_CIRCLE_SIZE = 0.2f;
        DrawCircle(p.x, p.y, radius + ADDITIONAL_CIRCLE_SIZE, color);
    }


    void EntityCircle::DrawBorder()
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });

        constexpr static float ADDITIONAL_CIRCLE_SIZE = 0.2f;
        DrawCircle(p.x, p.y, radius + ADDITIONAL_CIRCLE_SIZE + ENTITY_BORDER_SIZE / 2, COLOR_ENTITY_BORDER);
        DrawCircle(p.x, p.y, radius + ADDITIONAL_CIRCLE_SIZE - ENTITY_BORDER_SIZE / 2, color);
        // DrawCircleLines(p.x, p.y, radius + ADDITIONAL_CIRCLE_SIZE, COLOR_ENTITY_BORDER);
    }


    void EntityCircle::ChangeCoordinats(float x, float y)
    {
        // this->x = x;
        // this->y = y;
    }


    World::World()
    {
        b2SetLengthUnitsPerMeter(PIXELS_PER_METER);

        b2WorldDef worldDef = b2DefaultWorldDef();

        // Realistic gravity is achieved by multiplying gravity by the length unit.
        worldDef.gravity.y = 9.8f * PIXELS_PER_METER;
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
        SetConfigFlags(
                FLAG_VSYNC_HINT
                | FLAG_WINDOW_HIGHDPI
                // | FLAG_MSAA_4X_HINT // This flag ruins rendering of small circles for some reason
        );

        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lucky Ball Race");

        GuiSetStyle(BUTTON, TEXT_PADDING, 8);
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, 0);
        GuiSetStyle(BUTTON, BORDER_WIDTH, 1);

        SetTargetFPS(FPS);
    }


    void World::CopyEntity(const float x, const float y)
    {
        // copy_buffer->ChangeCoordinats(x, y);
    }


    void World::PasteEntity(const float x, const float y)
    {
        // copy_buffer->ChangeCoordinats(x, y);
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


    void World::SpawnTriangle(const float x, const float y)
    {
        entities.push_back(
                std::make_unique<EntityTriangle>(
                          world_id
                        , LBR::NORMAL
                        , LBR::STATIC
                        , Vector2 {x - DEFAULT_RECTANGLE_WIDTH / 2, y + DEFAULT_RECTANGLE_HEIGHT}
                        , Vector2 {x, y}
                        , Vector2 {x + DEFAULT_RECTANGLE_WIDTH / 2, y + DEFAULT_RECTANGLE_HEIGHT}
        ));
    }


    void World::SpawnCircle(const float x, const float y)
    {
        entities.push_back(
                std::make_unique<EntityCircle>(
                          world_id
                        , LBR::NORMAL
                        , LBR::STATIC
                        , x
                        , y
                        , DEFAULT_CIRCLE_RADIUS
        ));
    }


    void World::DetermineHoveredEntity()
    {
        Vector2 mouse_pos = GetMousePosition();
        hovered_entity = nullptr;
        for (auto &entity : entities)
        {
            bool is_hovered = false;
            switch (entity->shape)
            {
            case RECTANGLE:
                {
                    EntityRectangle *entity_rect = dynamic_cast<EntityRectangle*>(entity.get());
                    b2Vec2 p = b2Body_GetWorldPoint(entity_rect->bodyId, (b2Vec2) { -entity_rect->width / 2, -entity_rect->height / 2 });
                    is_hovered = CheckCollisionPointRec(mouse_pos, {p.x, p.y, entity_rect->width, entity_rect->height});
                }
                break;
            case TRIANGLE:
                {
                    EntityTriangle *entity_tri = dynamic_cast<EntityTriangle*>(entity.get());
                    b2Vec2 p1 = b2Body_GetWorldPoint(entity_tri->bodyId, (b2Vec2) { 0, 0 });
                    b2Vec2 p2 = b2Body_GetWorldPoint(entity_tri->bodyId, (b2Vec2) { entity_tri->v2.x, entity_tri->v2.y });
                    b2Vec2 p3 = b2Body_GetWorldPoint(entity_tri->bodyId, (b2Vec2) { entity_tri->v3.x, entity_tri->v3.y });
                    is_hovered = CheckCollisionPointTriangle(mouse_pos, { p1.x, p1.y }, { p2.x, p2.y }, { p3.x, p3.y });
                }
                break;
            case CIRCLE:
                {
                    EntityCircle *entity_circle = dynamic_cast<EntityCircle*>(entity.get());
                    b2Vec2 p = b2Body_GetWorldPoint(entity_circle->bodyId, (b2Vec2) { 0, 0 });
                    is_hovered = CheckCollisionPointCircle(mouse_pos, { p.x, p.y }, entity_circle->radius);
                }
                break;
            default:
                break;
            }
            if (is_hovered)
                hovered_entity = &entity;
        }
    }


    void DrawEntityByShape(std::unique_ptr<Entity> *entity, bool draw_border)
    {
        switch (entity->get()->shape)
        {
        case RECTANGLE:
            if (draw_border)
                dynamic_cast<EntityRectangle*>(entity->get())->DrawBorder();
            else
                dynamic_cast<EntityRectangle*>(entity->get())->Draw();
            break;
        case TRIANGLE:
            if (draw_border)
                dynamic_cast<EntityTriangle*>(entity->get())->DrawBorder();
            else
                dynamic_cast<EntityTriangle*>(entity->get())->Draw();
            break;
        case CIRCLE:
            if (draw_border)
                dynamic_cast<EntityCircle*>(entity->get())->DrawBorder();
            else
                dynamic_cast<EntityCircle*>(entity->get())->Draw();
            break;
        default:
            exit(1);
        }
    }

    void World::DrawEntities()
    {
        for (auto &entity : entities)
        {
            DrawEntityByShape(&entity, false);
        }
        if (hovered_entity != nullptr)
            DrawEntityByShape(hovered_entity, true);
    }


    bool World::GuiMenu(const std::vector<Button> buttons, const float x, const float y, const float width)
    {
        menu_collision = { x, y, width, (float)buttons.size() * (GUI_TEXT_SIZE + GUI_BUTTON_MARGIN) + GUI_BUTTON_MARGIN };
        GuiPanel(menu_collision, nullptr);

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
