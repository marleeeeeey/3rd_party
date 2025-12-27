#include "Serialization.h"

#include <iostream>

flatbuffers::FlatBufferBuilder createMonster(const std::string& monsterName) {
  using namespace MyGame::Sample;

  // Build up a serialized buffer algorithmically:
  flatbuffers::FlatBufferBuilder builder;

  // First, lets serialize some weapons for the Monster: A 'sword' and an 'axe'.
  auto weapon_one_name = builder.CreateString("Sword");
  short weapon_one_damage = 3;

  auto weapon_two_name = builder.CreateString("Axe");
  short weapon_two_damage = 5;

  // Use the `CreateWeapon` shortcut to create Weapons with all fields set.
  auto sword = CreateWeapon(builder, weapon_one_name, weapon_one_damage);
  auto axe = CreateWeapon(builder, weapon_two_name, weapon_two_damage);

  // Create a FlatBuffer's `vector` from the `std::vector`.
  std::vector<flatbuffers::Offset<Weapon>> weapons_vector;
  weapons_vector.push_back(sword);
  weapons_vector.push_back(axe);
  auto weapons = builder.CreateVector(weapons_vector);

  // Second, serialize the rest of the objects needed by the Monster.
  auto position = Vec3(1.0f, 2.0f, 3.0f);

  auto name = builder.CreateString(monsterName);

  unsigned char inv_data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto inventory = builder.CreateVector(inv_data, 10);

  // Shortcut for creating monster with all fields set:
  auto orc = CreateMonster(builder,
                           &position,         // pos
                           150,               // mana
                           80,                // hp
                           name,              // name
                           inventory,         // inventory
                           Color_Red,         // color
                           weapons,           // weapons
                           Equipment_Weapon,  // equipped_type
                           axe.Union()        // path
  );

  builder.Finish(orc);  // Serialize the root of the object.
  return builder;
}

void verifyMonster(const MyGame::Sample::Monster* monster) {
  using namespace MyGame::Sample;

  assert(monster != nullptr);

  // Get and test some scalar types from the FlatBuffer.
  assert(monster->hp() == 80);
  assert(monster->mana() == 150);  // default
  // assert(monster->name()->str() == "MyMonster");

  // Get and test a field of the FlatBuffer's `struct`.
  auto pos = monster->pos();
  assert(pos);
  assert(pos->z() == 3.0f);
  (void)pos;

  // Get a test an element from the `inventory` FlatBuffer's `vector`.
  auto inv = monster->inventory();
  assert(inv);
  assert(inv->Get(9) == 9);
  (void)inv;

  // Get and test the `weapons` FlatBuffers's `vector`.
  std::string expected_weapon_names[] = {"Sword", "Axe"};
  short expected_weapon_damages[] = {3, 5};
  auto weps = monster->weapons();
  for (unsigned int i = 0; i < weps->size(); i++) {
    assert(weps->Get(i)->name()->str() == expected_weapon_names[i]);
    assert(weps->Get(i)->damage() == expected_weapon_damages[i]);
  }
  (void)expected_weapon_names;
  (void)expected_weapon_damages;

  // Get and test the `Equipment` union (`equipped` field).
  assert(monster->equipped_type() == Equipment_Weapon);
  auto equipped = static_cast<const Weapon*>(monster->equipped());
  assert(equipped->name()->str() == "Axe");
  assert(equipped->damage() == 5);
  (void)equipped;

  std::cout << "The FlatBuffer was successfully created and verified" << std::endl;
}
