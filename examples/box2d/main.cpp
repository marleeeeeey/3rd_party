#include <iostream>
#include <box2d/box2d.h>

int main()
{
    std::cout << "Hello box2d" << std::endl;
    std::cout << "Example based on https://github.com/erincatto/box2d/blob/main/docs/migration.md" << std::endl;

    { // Creating a world

        b2Vec2 gravity = {0.0f, -10.0f};
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = gravity;
        b2WorldId worldId = b2CreateWorld(&worldDef);

        { // Creating a body

            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = b2Vec2{0.0f, 4.0f};
            b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);


            { // Creating a shape

                b2Polygon box = b2MakeBox(1.0f, 1.0f);

                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.density = 1.0f;
                // shapeDef.friction = 0.3f;

                b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);

                // ---

                bool updateBodyMass = true;
                b2DestroyShape(shapeId, updateBodyMass);
                shapeId = b2_nullShapeId;
            }


            { // Chains

                b2Vec2 points[5] = {
                    {-8.0f, 6.0f},
                    {-8.0f, 20.0f},
                    {8.0f, 20.0f},
                    {8.0f, 6.0f},
                    {0.0f, -2.0f}
                };

                b2ChainDef chainDef = b2DefaultChainDef();
                chainDef.points = points;
                chainDef.count = 5;
                // chainDef.loop = true;
                b2ChainId chainId = b2CreateChain(bodyId, &chainDef);

            }

            { // Creating a joint

                // My code to create ground body. Copied from bodyId creation.
                b2BodyDef bodyDef = b2DefaultBodyDef();
                bodyDef.type = b2_dynamicBody;
                bodyDef.position = b2Vec2{0.0f, 4.0f};
                b2BodyId groundId = b2CreateBody(worldId, &bodyDef);

                // Original code from the official example.
                b2Vec2 pivot = {-10.0f, 20.5f};
                b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
                jointDef.bodyIdA = groundId;
                jointDef.bodyIdB = bodyId;
                jointDef.localAnchorA = b2Body_GetLocalPoint(jointDef.bodyIdA, pivot);
                jointDef.localAnchorB = b2Body_GetLocalPoint(jointDef.bodyIdB, pivot);
                jointDef.motorSpeed = 1.0f;
                jointDef.maxMotorTorque = 100.0f;
                jointDef.enableMotor = true;
                jointDef.lowerAngle = -0.25f * B2_PI;
                jointDef.upperAngle = 0.5f * B2_PI;
                jointDef.enableLimit = true;
                b2JointId jointId = b2CreateRevoluteJoint(worldId, &jointDef);

            }

            // ---

            b2DestroyBody(bodyId);
            bodyId = b2_nullBodyId;
        }

        // ---

        b2DestroyWorld(worldId);
        worldId = b2_nullWorldId;
    }
}