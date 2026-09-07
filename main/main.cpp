#include <raylib.h>
#include <box2d/box2d.h>
#include <assert.h>
#include <vector>


// //------------------------------------------------------------------------------------
// // Program main entry point
// //------------------------------------------------------------------------------------
// int main(void)
// {
//     // Initialization
//     //--------------------------------------------------------------------------------------
//     const int screenWidth = 800;
//     const int screenHeight = 450;

//     b2WorldDef worldDef = b2DefaultWorldDef();
//     worldDef.gravity = (b2Vec2){0.0f, -10.0f};

//     b2WorldId worldId = b2CreateWorld(&worldDef);

//     b2BodyDef groundBodyDef = b2DefaultBodyDef();
//     groundBodyDef.position = (b2Vec2){0.0f, -10.0f};

//     b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);

//     b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);

//     b2ShapeDef groundShapeDef = b2DefaultShapeDef();
//     b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

//     InitWindow(screenWidth, screenWidth, "Box2d + Raylib");

//     SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

//     // HideCursor();

//     SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
//     //--------------------------------------------------------------------------------------

//     // Main game loop
//     while (!WindowShouldClose())        // Detect window close button or ESC key
//     {
//         //----------------------------------------------------------------------------------
//         // Draw
//         //----------------------------------------------------------------------------------

//         BeginDrawing();

//             ClearBackground(DARKGRAY);

//                 DrawText("Hello_World!", 20, 20, 20, GOLD);

//             EndMode2D();

//         EndDrawing();

//         //----------------------------------------------------------------------------------
//     }

//     // De-Initialization
//     //--------------------------------------------------------------------------------------
//     CloseWindow();        // Close window and OpenGL context
//     //--------------------------------------------------------------------------------------

//     return 0;
// }







#define WIDTH  480.0
#define HEIGHT 480.0
#define BOX_SIZE 10



typedef struct Entity
{
	b2BodyId bodyId;
	b2Vec2 extent;
} Entity;

void DrawEntity(const Entity* entity)
{
	// The boxes were created centered on the bodies, but raylib draws textures starting at the top left corner.
	// b2Body_GetWorldPoint gets the top left corner of the box accounting for rotation.
	b2Vec2 p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2) { -entity->extent.x, -entity->extent.y });
	b2Rot rotation = b2Body_GetRotation(entity->bodyId);
	float radians = b2Rot_GetAngle(rotation);

	Vector2 ps = {p.x, p.y};
	// DrawEx(entity->texture, ps, RAD2DEG * radians, 1.0f, WHITE);
	DrawRectanglePro({p.x, p.y, BOX_SIZE, BOX_SIZE}, {0, 0}, RAD2DEG * radians, MAGENTA);

	// I used these circles to ensure the coordinates are correct
	// DrawCircleV(ps, 5.0f, BLACK);
	// p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2){0.0f, 0.0f});
	// ps = (Vector2){ p.x, p.y };
	// DrawCircleV(ps, 5.0f, BLUE);
	// p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2){ entity->extent.x, entity->extent.y });
	// ps = (Vector2){ p.x, p.y };
	// DrawCircleV(ps, 5.0f, RED);
}

#define GROUND_COUNT 45
#define BOX_COUNT 10

int main(void)
{
	InitWindow(WIDTH, HEIGHT, "box2d-raylib");

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

	Entity groundEntities[GROUND_COUNT] = { 0 };
	for (int i = 0; i < GROUND_COUNT; ++i)
	{
		Entity* entity = groundEntities + i;
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.position = (b2Vec2){ (2.0f * i + 2.0f) * groundExtent.x, (float)HEIGHT - groundExtent.y - 100.0f};

		// I used this rotation to test the world to screen transformation
		//bodyDef.rotation = b2MakeRot(0.25f * b2_pi * i);

		entity->bodyId = b2CreateBody(worldId, &bodyDef);
		entity->extent = groundExtent;
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		b2CreatePolygonShape(entity->bodyId, &shapeDef, &groundPolygon);
	}

	Entity boxEntities[BOX_COUNT] = { 0 };
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

	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_P))
		{
			pause = !pause;
		}
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
		{
    	Entity* entity;
    	b2BodyDef bodyDef = b2DefaultBodyDef();
    	// bodyDef.position = (b2Vec2){ (float)GetMouseX(), (float)GetMouseY()};
    	bodyDef.position = (b2Vec2){ (float)GetMouseX(), (float)GetMouseY()};

    	// I used this rotation to test the world to screen transformation
    	//bodyDef.rotation = b2MakeRot(0.25f * b2_pi * i);

    	entity->bodyId = b2CreateBody(worldId, &bodyDef);
    	entity->extent = groundExtent;
    	b2ShapeDef shapeDef = b2DefaultShapeDef();
    	b2CreatePolygonShape(entity->bodyId, &shapeDef, &groundPolygon);
    	userEntities.push_back(*entity);
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

		for (int i = 0; i < GROUND_COUNT; ++i)
		{
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

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
