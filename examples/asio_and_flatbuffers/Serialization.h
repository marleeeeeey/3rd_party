#pragma once
#include <string>

#include "flatbuffers/flatbuffer_builder.h"
#include "solder_generated.h"  // Already includes "flatbuffers/flatbuffers.h"."

// Create flatbuffer with monster data
flatbuffers::FlatBufferBuilder createMonster(const std::string& monsterName);

// Verify that the monster data is valid. Same as in CreadMonster method
void verifyMonster(const MyGame::Sample::Monster* monster);