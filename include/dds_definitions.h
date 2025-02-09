// dds_definitions.h
#ifndef DDS_DEFINITIONS_H
#define DDS_DEFINITIONS_H

#include <stdint.h>

// Define roles
#define ROLE_OPERATOR 0
#define ROLE_GENERATOR 1

// Topics for DDS communication
typedef struct DroneState {
    float x, y;       // Position
    float vx, vy;     // Velocity
    int role;         // Role (Operator/Generator)
} DroneState;

typedef struct Target {
    float x, y;
} Target;

typedef struct TargetList {
    Target targets[10];
    int count;
} TargetList;

typedef struct Obstacle {
    float x, y;
} Obstacle;

typedef struct ObstacleList {
    Obstacle obstacles[20];
    int count;
} ObstacleList;

typedef struct ControlCommand {
    char key;         // Keyboard input (e.g., 'w', 's', 'a', 'd')
} ControlCommand;

#endif