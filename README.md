# Drone Simulator - Assignment 2

A **distributed drone simulation system** where two computers communicate via DDS (Data Distribution Service).  
- **Operator Computer**: Controls the drone and displays the simulation.  
- **Generator Computer**: Generates targets/obstacles and shares them over the network.  

---

## Table of Contents
1. [System Overview](#system-overview)
2. [Requirements](#requirements)
3. [Installation](#installation)
4. [Building the Project](#building-the-project)
5. [Running the Application](#running-the-application)
6. [Keyboard Controls](#keyboard-controls)
7. [DDS Configuration](#dds-configuration)
8. [Architecture](#architecture)
9. [Troubleshooting](#troubleshooting)
10. [FAQ](#faq)

---

## System Overview
### Key Features for Assignment 2
- **Distributed Simulation**:  
  - Operator and Generator run on separate machines in the same LAN.  
  - Communication via DDS (CycloneDDS implementation).  
- **Dynamic Content**:  
  - Generator creates random targets (green numbers) and obstacles (orange dots).  
  - Operator receives real-time updates and displays them.  
- **Physics Engine**:  
  - Drone movement follows 2-DOF dynamics with inertia and viscous friction.  

---

## Requirements
### Hardware
- 2 x Linux machines (Ubuntu 22.04 recommended) on the same network.  
- Minimum screen resolution: 1024x768 (for ncurses interface).  

### Software
- `gcc`, `make`, `git`  
- `ncurses` library  
- CycloneDDS (DDS middleware)  


### Install dependencies on both machines:
sudo apt-get update
sudo apt-get install build-essential libncurses-dev cyclonedds-dev

### Installation
### Clone the repository (on both machines):
```bash
git clone https://github.com/abdulhayee181/ARP-Drone-second-Assignment.git
cd drone-simulator

## Building the Project

### Compile the code:
```bash 
make clean && make

This builds:

- **`server`**: Manages DDS communication and pipe routing.
- **`drone`**: Handles drone dynamics.
- **`interface`**: Renders the ncurses window.
- **`watchdog`**: Monitors system health.

# Running the Application
## Step 1: Start the Generator Computer
On the Generator Machine:

```bash
./build/main generator

What Happens:

- Generates random targets and obstacles every 5 seconds.

- Publishes data to DDS topics:

- **TargetList: Positions of targets.

- **ObstacleList: Positions of obstacles.

# Step 2: Start the Operator Computer
On the Operator Machine:

```bash
./build/main operator

What Happens:

- Launches an ncurses window with:

- **Drone (blue +).

- **Targets (green numbers, e.g., 1, 2).

- **Obstacles (orange *).

-Subscribes to DDS topics:

- **Receives targets/obstacles from the Generator.

- **Publishes drone state (DroneState topic) for collision detection.

# Keyboard Controls (Operator Only)
Key	Action
W	Move Up
A	Move Left
S	Move Down
D	Move Right
X	Stop Drone
Q	Quit Application
# DDS Configuration
## Topics
Topic_Name	Data Type	Publisher	Subscriber
TargetList	TargetList	Generator	Operator
ObstacleList	ObstacleList	Generator	Operator
DroneState	DroneState	Operator	Generator
Network Setup
Verify Network Connectivity:

```bash
ping <generator-ip>  # Replace with the actual IP

Configure CycloneDDS:

Create a file cyclonedds_config.xml:

xml
Copy
<CycloneDDS>
  <Domain>
    <General>
      <NetworkInterfaceAddress>auto</NetworkInterfaceAddress>
      <AllowMulticast>true</AllowMulticast>
    </General>
  </Domain>
</CycloneDDS>
Run HTML
Run the application with:

```bash
export CYCLONEDDS_URI=file://$(pwd)/cyclonedds_config.xml
./build/main operator  # or generator

# Architecture
## Component Diagram
plaintext
Copy
+---------------------+          DDS Topics           +---------------------+
|   Operator Machine  |<---------------------------->|  Generator Machine  |
+---------------------+                               +---------------------+
| - Drone Dynamics    |                               | - Targets Generator |
| - Keyboard Input    |                               | - Obstacles Generator|
| - Ncurses Interface |                               | - DDS Publisher     |
| - DDS Subscriber    |                               +---------------------+
+---------------------+
# Data Flow
- Operator sends drone position updates via DroneState.

- Generator sends targets/obstacles via TargetList and ObstacleList.

- Operator renders received data in the ncurses window.

# Troubleshooting
## Common Issues
### DDS Communication Failure:

- Ensure both machines are on the same LAN.

- Check firewall rules (allow UDP port 7400).

- Enable debug logs:

```bash
export CYCLONEDDS_DEBUG=1 && ./build/main operator
### Build Errors:

- Verify dependencies:

```bash
sudo apt-get install build-essential libncurses-dev cyclonedds-dev

- Rebuild:

```bash
make clean && make

### Ncurses Window Not Rendering:

- Install ncurses:

```bash
sudo apt-get install libncurses-dev

##FAQ
### Q: Can I run both roles on a single machine?
A: Yes! Use two terminals:

```bash
# Terminal 1 (Generator):
./build/main generator

# Terminal 2 (Operator):
./build/main operator

### Q: How do I adjust the simulation speed?
A: Modify SIMULATION_TIMESTEP_MS in constants.h:

```bash
#define SIMULATION_TIMESTEP_MS 100  // 100ms = 10Hz

### Q: Why aren’t targets/obstacles appearing on the Operator?
A:

- Ensure the Generator is running.

- Check DDS configuration and network connectivity.

### Note:
- For Assignment 1 (single-machine mode), run:

```bash
./build/main