#pragma once

#include "dungeon.h"

#include <stdexcept>
#include <set>
#include <iostream>

void OpenDoor(Door* door, std::vector<std::string>& found_keys) {
    for (size_t i = 0; i < found_keys.size(); ++i) {
        door->TryOpen(found_keys[i]);
    }
}

void AddKeys(Room* room, std::vector<std::string>& found_keys) {
    for (size_t i = 0; i < room->NumKeys(); ++i) {
        found_keys.push_back(room->GetKey(i));
    }
}

Room* MyFindFinalRoom(Room* room, std::vector<std::string>& found_keys, std::set<Door*>& used_doors,
                      std::set<Room*>& visited_rooms) {
    visited_rooms.insert(room);
    if (room->IsFinal()) {
        return room;
    }
    AddKeys(room, found_keys);
    for (size_t i = 0; i < room->NumDoors(); ++i) {
        const auto door_to_other_room = room->GetDoor(i);
        if (!used_doors.contains(door_to_other_room)) {
            used_doors.insert(door_to_other_room);
            OpenDoor(door_to_other_room, found_keys);
            if (!door_to_other_room->IsOpen()) {
                continue;
            }
            auto next_room = door_to_other_room->GoThrough();
            if (visited_rooms.contains(next_room)) {
                continue;
            }
            if (auto final_room =
                    (MyFindFinalRoom(next_room, found_keys, used_doors, visited_rooms))) {
                return final_room;
            }
        }
    }
    return nullptr;
}

Room* FindFinalRoom(Room* room) {
    std::set<Door*> used_doors;
    std::set<Room*> visited_rooms;
    std::vector<std::string> found_keys;
    auto exit_room = MyFindFinalRoom(room, found_keys, used_doors, visited_rooms);
    // while (!used_doors.empty()) {
    //     delete *(--used_doors.end());
    //     used_doors.erase(--used_doors.end());
    // }
    return exit_room;
}
