#include "include/classes.hpp"


#include "box2d/box2d.h"
#include "box2d/id.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <memory>
#include <vector>

#include "include/config.hpp"



namespace LBR
{
    Entity::~Entity()
    {
        b2DestroyBody(bodyId);
    }


    void Entity::ChangeCoordinats(const float x, const float y)
    {
        b2Body_SetTransform(bodyId, { x, y }, b2Body_GetRotation(bodyId));
    }


    void DefineType(b2BodyDef* bodyDef, const EntityType type) {
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

        b2BodyDef bodyDef = b2DefaultBodyDef();

        bodyDef.position = (b2Vec2){ x, y };

        b2Polygon polygon = b2MakeBox(width * 0.5f, height * 0.5f);

        DefineType(&bodyDef, type);

        bodyId = b2CreateBody(world_id, &bodyDef);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
    }


    std::unique_ptr<Entity> EntityRectangle::Clone() const
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, { 0, 0 });
        return std::make_unique<EntityRectangle>(
            b2Body_GetWorld(bodyId), behaviour, type, p.x, p.y, width, height, color
        );
    }


    bool EntityRectangle::IsHovered(Vector2 mouse_pos)
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, (b2Vec2) { -width / 2, -height / 2 });
        return CheckCollisionPointRec(mouse_pos, {p.x, p.y, width, height});
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


    void EntityRectangle::DrawOutline(Color outline_color)
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, (b2Vec2) { -(width - ENTITY_OUTLINE_SIZE) / 2, -(height - ENTITY_OUTLINE_SIZE) / 2 });
        b2Vec2 p_border = b2Body_GetWorldPoint(bodyId, (b2Vec2) { -(width + ENTITY_OUTLINE_SIZE) / 2, -(height + ENTITY_OUTLINE_SIZE) / 2 });
        b2Rot rotation = b2Body_GetRotation(bodyId);
        float radians = b2Rot_GetAngle(rotation);

        DrawRectanglePro({p_border.x, p_border.y, width + ENTITY_OUTLINE_SIZE, height + ENTITY_OUTLINE_SIZE}, {0, 0}, RAD2DEG * radians, outline_color);
        DrawRectanglePro({p.x, p.y, width - ENTITY_OUTLINE_SIZE, height - ENTITY_OUTLINE_SIZE}, {0, 0}, RAD2DEG * radians, color);
    }


    EntityTriangle::EntityTriangle(b2WorldId world_id, EntityBehaviour behaviour, EntityType type, Vector2 v1, Vector2 v2, Vector2 v3, Color color)
            : v2({v2.x - v1.x, v2.y - v1.y}), v3({v3.x - v1.x, v3.y - v1.y})
    {
        this->color = color;
        this->behaviour = behaviour;
        this->type = type;
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


    std::unique_ptr<Entity> EntityTriangle::Clone() const
    {
        b2Vec2 p1 = b2Body_GetWorldPoint(bodyId, { 0, 0 });
        b2Vec2 p2 = b2Body_GetWorldPoint(bodyId, { v2.x, v2.y });
        b2Vec2 p3 = b2Body_GetWorldPoint(bodyId, { v3.x, v3.y });
        return std::make_unique<EntityTriangle>(
            b2Body_GetWorld(bodyId), behaviour, type, Vector2{ p1.x, p1.y }, Vector2{ p2.x, p2.y }, Vector2{ p3.x, p3.y }, color
        );
    }


    bool EntityTriangle::IsHovered(Vector2 mouse_pos)
    {
        b2Vec2 p1 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });
        b2Vec2 p2 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v2.x, v2.y });
        b2Vec2 p3 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v3.x, v3.y });
        return CheckCollisionPointTriangle(mouse_pos, { p1.x, p1.y }, { p2.x, p2.y }, { p3.x, p3.y });
    }


    bool PointsAreCounterclockwise(b2Vec2 v1, b2Vec2 v2, b2Vec2 v3)
    {
        // Math https://www.baeldung.com/cs/list-polygon-points-clockwise#1-area-of-a-triangle
        return !(((v2.x - v1.x) * (v3.y - v1.y) - (v3.x - v1.x) * (v2.y * v1.y)) * 0.5f);
    }


    void EntityTriangle::Draw()
    {
        b2Vec2 p1 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });
        b2Vec2 p2 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v2.x, v2.y });
        b2Vec2 p3 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v3.x, v3.y });

        // We check only one time since cases there points
        // are counterclockwise are:
        //
        //      3     1->2->3
        //    /  \    2->3->1
        //  1 --- 2   3->1->2
        //
        //      2     1->3->2
        //    /  \    3->2->1
        //  1 --- 3   2->1->3
        if (PointsAreCounterclockwise(p1, p2, p3))
            DrawTriangle({p1.x, p1.y}, {p2.x, p2.y}, {p3.x, p3.y}, color);
        else
            DrawTriangle({p1.x, p1.y}, {p3.x, p3.y}, {p2.x, p2.y}, color);
    }


    void EntityTriangle::DrawOutline(Color outline_color)
    {
        b2Vec2 p1 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });
        b2Vec2 p2 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v2.x, v2.y });
        b2Vec2 p3 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { v3.x, v3.y });

        // Lines from center to points. Using Pythagoras formula.
        float c_p1 = std::sqrt(centroid.x * centroid.x + 0 * 0);
        float c_p2 = std::sqrt(centroid.x * centroid.x + v2.y * v2.y);
        float c_p3 = std::sqrt(centroid.x * centroid.x + v3.y * v3.y);

        // Lines from center to borders.
        float c_b1 = (c_p1 + ENTITY_OUTLINE_SIZE * 2) / c_p1;
        float c_b2 = (c_p2 + ENTITY_OUTLINE_SIZE * 2) / c_p2;
        float c_b3 = (c_p3 + ENTITY_OUTLINE_SIZE * 2) / c_p3;

        b2Vec2 b1 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { (0    - centroid.x) * c_b1 + centroid.x
                                                          , (0    - centroid.y) * c_b1 + centroid.y });
        b2Vec2 b2 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { (v2.x - centroid.x) * c_b2 + centroid.x
                                                          , (v2.y - centroid.y) * c_b2 + centroid.y });
        b2Vec2 b3 = b2Body_GetWorldPoint(bodyId, (b2Vec2) { (v3.x - centroid.x) * c_b3 + centroid.x
                                                          , (v3.y - centroid.y) * c_b3 + centroid.y });

        if (PointsAreCounterclockwise(p1, p2, p3))
        {
            DrawTriangle({b1.x, b1.y}, {b2.x, b2.y}, {b3.x, b3.y}, outline_color);
            DrawTriangle({p1.x, p1.y}, {p2.x, p2.y}, {p3.x, p3.y}, color);
        }
        else
        {
            DrawTriangle({b1.x, b1.y}, {b3.x, b3.y}, {b2.x, b2.y}, outline_color);
            DrawTriangle({p1.x, p1.y}, {p3.x, p3.y}, {p2.x, p2.y}, color);
        }
    }


    EntityCircle::EntityCircle(b2WorldId world_id, EntityBehaviour behaviour, EntityType type, float x, float y, float radius, Color color)
            : radius(radius)
    {
        this->color = color;
        this->behaviour = behaviour;
        this->type = type;

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = (b2Vec2){ x, y };

        b2Circle circle{{0,0}, radius};

        DefineType(&bodyDef, type);

        bodyId = b2CreateBody(world_id, &bodyDef);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreateCircleShape(bodyId, &shapeDef, &circle);
    }


    std::unique_ptr<Entity> EntityCircle::Clone() const
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, { 0, 0 });
        return std::make_unique<EntityCircle>(
            b2Body_GetWorld(bodyId), behaviour, type, p.x, p.y, radius, color
        );
    }


    bool EntityCircle::IsHovered(Vector2 mouse_pos)
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });
        return CheckCollisionPointCircle(mouse_pos, { p.x, p.y }, radius);
    }


    void EntityCircle::Draw()
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });

        // For some reason without ADDITIONAL_CIRCLE_SIZE physical body size of a
        // circle seems to be a little bigger that what's rendered.
        constexpr static float ADDITIONAL_CIRCLE_SIZE = 0.2f;
        DrawCircle(p.x, p.y, radius + ADDITIONAL_CIRCLE_SIZE, color);
    }


    void EntityCircle::DrawOutline(Color outline_color)
    {
        b2Vec2 p = b2Body_GetWorldPoint(bodyId, (b2Vec2) { 0, 0 });

        constexpr static float ADDITIONAL_CIRCLE_SIZE = 0.2f;
        DrawCircle(p.x, p.y, radius + ADDITIONAL_CIRCLE_SIZE + ENTITY_OUTLINE_SIZE / 2, outline_color);
        DrawCircle(p.x, p.y, radius + ADDITIONAL_CIRCLE_SIZE - ENTITY_OUTLINE_SIZE / 2, color);
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


    void World::CopyEntities(const float x, const float y)
    {
        copy_buffer.clear();
        for (const auto &entity : selected_entities)
        {
            copy_buffer.push_back(entity->Clone());
            b2Body_Disable(copy_buffer.back()->bodyId);
        }
    }


    void World::PasteEntities(const float x, const float y)
    {
        std::list<Entity*> new_selected_entities;
        b2Vec2 middle_of_entities = { 0, 0 };
        for (const auto &entity : copy_buffer)
        {
            new_selected_entities.push_back(entity.get());
            middle_of_entities += b2Body_GetWorldPoint(entity->bodyId, (b2Vec2) { 0, 0 });
        }

        selected_entities.clear();
        middle_of_entities.x /= new_selected_entities.size();
        middle_of_entities.y /= new_selected_entities.size();
        for (const auto &entity : new_selected_entities)
        {
            b2Vec2 entity_middle = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2) { 0, 0 });
            b2Vec2 new_entity_middle = { entity_middle.x - middle_of_entities.x + x
                                       , entity_middle.y - middle_of_entities.y + y };

            entity->ChangeCoordinats(new_entity_middle.x, new_entity_middle.y);
            entities.push_back(entity->Clone());
        }
    }


    void World::DeleteEntities(const float x, const float y)
    {
        for (const auto &entity : selected_entities)
        {
            entities.remove_if([entity](const std::unique_ptr<Entity>& ptr) {
                return ptr.get() == entity;
            });
        }
        selected_entities.clear();
    }


    void World::SpawnRectangle(const float x, const float y)
    {
        entities.push_back(
                std::make_unique<EntityRectangle>(
                         world_id
                        ,LBR::NORMAL
                        ,LBR::STATIC
                        ,x
                        ,y
                        ,DEFAULT_RECTANGLE_WIDTH
                        ,DEFAULT_RECTANGLE_HEIGHT
        ));
    }


    void World::SpawnTriangle(const float x, const float y)
    {
        entities.push_back(
                std::make_unique<EntityTriangle>(
                         world_id
                        ,LBR::NORMAL
                        ,LBR::STATIC
                        ,Vector2 {x - DEFAULT_RECTANGLE_WIDTH / 2, y + DEFAULT_RECTANGLE_HEIGHT}
                        ,Vector2 {x, y}
                        ,Vector2 {x + DEFAULT_RECTANGLE_WIDTH / 2, y + DEFAULT_RECTANGLE_HEIGHT}
        ));
    }


    void World::SpawnCircle(const float x, const float y)
    {
        entities.push_back(
                std::make_unique<EntityCircle>(
                         world_id
                        ,LBR::NORMAL
                        ,LBR::STATIC
                        ,x
                        ,y
                        ,DEFAULT_CIRCLE_RADIUS
        ));
    }


    void World::DetermineHoveredEntity()
    {
        Vector2 mouse_pos = GetMousePosition();
        if (CheckCollisionPointRec(mouse_pos, menu_collision))
        {
            hovered_entity = nullptr;
        }
        else if (hovered_entity == nullptr || not hovered_entity->IsHovered(mouse_pos))
        {
            hovered_entity = nullptr;
            for (const auto &entity : entities)
                if (entity->IsHovered(mouse_pos))
                    hovered_entity = entity.get();
        }
    }


    void World::MoveSelectedEntities() {

    }


    void World::DrawEntities()
    {
        for (const auto &entity : entities)
            entity->Draw();

        for (const auto &entity : selected_entities)
            entity->DrawOutline(COLOR_ENTITY_SELECTED);

        if (hovered_entity != nullptr)
            hovered_entity->DrawOutline(COLOR_ENTITY_HOVERED);
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
                menu_collision = { 0, 0, 0, 0 };
            }
        }
        return button_was_clicked;
    }
}
