// dds_helper.c
#include "dds_definitions.h"
#include <cyclonedds/dds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dds_entity_t participant;
dds_entity_t topic_drone, topic_targets, topic_obstacles, topic_commands;
dds_entity_t writer_drone, writer_targets, writer_obstacles;
dds_entity_t reader_commands;

void dds_init(int role) {
    participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
    
    // Create topics
    topic_drone = dds_create_topic(participant, &DroneState_desc, "DroneState");
    topic_targets = dds_create_topic(participant, &TargetList_desc, "TargetList");
    topic_obstacles = dds_create_topic(participant, &ObstacleList_desc, "ObstacleList");
    topic_commands = dds_create_topic(participant, &ControlCommand_desc, "ControlCommands");

    // Create readers/writers based on role
    if (role == ROLE_OPERATOR) {
        writer_drone = dds_create_writer(participant, topic_drone);
        reader_commands = dds_create_reader(participant, topic_commands);
    } else {
        writer_targets = dds_create_writer(participant, topic_targets);
        writer_obstacles = dds_create_writer(participant, topic_obstacles);
    }
}

void dds_publish_drone_state(float x, float y, float vx, float vy) {
    DroneState state = {x, y, vx, vy, ROLE_OPERATOR};
    dds_write(writer_drone, &state);
}

void dds_publish_targets(TargetList *targets) {
    dds_write(writer_targets, targets);
}

void dds_publish_obstacles(ObstacleList *obstacles) {
    dds_write(writer_obstacles, obstacles);
}

ControlCommand dds_read_commands() {
    ControlCommand cmd;
    dds_sample_info_t info;
    if (dds_take(reader_commands, &cmd, &info, 1, 1) > 0) {
        return cmd;
    }
    return (ControlCommand){.key = '\0'}; // No command
}