#include "server.h"
#include "constants.h"
#include "util.h"
#include "dds_definitions.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

using namespace eprosima::fastdds::dds;

// DDS Global Entities
DomainParticipant* participant = nullptr;
Publisher* publisher = nullptr;
Subscriber* subscriber = nullptr;
DataWriter* writer_drone = nullptr;
DataWriter* writer_targets = nullptr;
DataWriter* writer_obstacles = nullptr;
DataReader* reader_commands = nullptr;
Topic* topic_drone = nullptr;
Topic* topic_targets = nullptr;
Topic* topic_obstacles = nullptr;
Topic* topic_commands = nullptr;
TypeSupport type_drone, type_targets, type_obstacles, type_commands;

// Role (0=Operator, 1=Generator)
int role;

// Pipes (declared as extern in server.h)
extern int km_server[2], server_drone[2], interface_server[2], drone_server[2];
extern int server_interface[2], server_obstacles[2], obstacles_server[2];
extern int server_targets[2], targets_server[2];

// ================== DDS INITIALIZATION ================== //
void dds_init() {
    // Create Participant
    participant = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
    
    // Register Types
    type_drone.register_type(participant);
    type_targets.register_type(participant);
    type_obstacles.register_type(participant);
    type_commands.register_type(participant);

    // Create Topics
    topic_drone = participant->create_topic("DroneState", type_drone.get_type_name(), TOPIC_QOS_DEFAULT);
    topic_targets = participant->create_topic("TargetList", type_targets.get_type_name(), TOPIC_QOS_DEFAULT);
    topic_obstacles = participant->create_topic("ObstacleList", type_obstacles.get_type_name(), TOPIC_QOS_DEFAULT);
    topic_commands = participant->create_topic("ControlCommands", type_commands.get_type_name(), TOPIC_QOS_DEFAULT);

    // Create Publisher/Subscriber Based on Role
    if (role == ROLE_OPERATOR) {
        publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        writer_drone = publisher->create_datawriter(topic_drone, DATAWRITER_QOS_DEFAULT);
        subscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
        reader_commands = subscriber->create_datareader(topic_commands, DATAREADER_QOS_DEFAULT);
    } else {
        publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT);
        writer_targets = publisher->create_datawriter(topic_targets, DATAWRITER_QOS_DEFAULT);
        writer_obstacles = publisher->create_datawriter(topic_obstacles, DATAWRITER_QOS_DEFAULT);
    }
}

// ================== PIPE/DDS BRIDGE FUNCTIONS ================== //
void forward_commands_to_drone() {
    ControlCommand cmd;
    SampleInfo info;
    if (reader_commands->take_next_sample(&cmd, &info) == ReturnCode_t::RETCODE_OK && info.valid_data) {
        char cmd_str[2] = {cmd.key, '\0'};
        write_to_pipe(server_drone[1], cmd_str);  // Send to drone.c
    }
}

void publish_drone_state() {
    char buffer[MSG_LEN];
    ssize_t bytes = read(drone_server[0], buffer, MSG_LEN);  // Read from drone.c
    if (bytes > 0) {
        DroneState state;
        sscanf(buffer, "D:%f,%f,%f,%f", &state.x, &state.y, &state.vx, &state.vy);
        writer_drone->write(&state);  // Publish via DDS
    }
}

void publish_targets_obstacles() {
    TargetList targets;
    ObstacleList obstacles;
    
    // Read from targets.c
    char targets_buffer[MSG_LEN];
    if (read(targets_server[0], targets_buffer, MSG_LEN) > 0) {
        parse_targets(targets_buffer, &targets);
        writer_targets->write(&targets);
    }

    // Read from obstacles.c
    char obstacles_buffer[MSG_LEN];
    if (read(obstacles_server[0], obstacles_buffer, MSG_LEN) > 0) {
        parse_obstacles(obstacles_buffer, &obstacles);
        writer_obstacles->write(&obstacles);
    }
}

// ================== MAIN SERVER LOOP ================== //
void server_loop() {
    fd_set read_fds;
    struct timeval timeout = {0, 100000};  // 100ms

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(drone_server[0], &read_fds);        // Drone updates
        FD_SET(targets_server[0], &read_fds);      // Targets updates
        FD_SET(obstacles_server[0], &read_fds);    // Obstacles updates

        select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);

        // Handle DDS communication
        if (role == ROLE_OPERATOR) {
            publish_drone_state();
            forward_commands_to_drone();
        } else {
            publish_targets_obstacles();
        }
    }
}

// ================== MAIN FUNCTION ================== //
int main(int argc, char* argv[]) {
    // Parse role from arguments (passed by main.c)
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <role>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    role = (strcmp(argv[1], "operator") == 0) ? ROLE_OPERATOR : ROLE_GENERATOR;

    // Initialize DDS
    dds_init();

    // Start server loop
    server_loop();

    // Cleanup
    DomainParticipantFactory::get_instance()->delete_participant(participant);
    return 0;
}