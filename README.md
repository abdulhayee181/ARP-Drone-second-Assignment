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

```bash
# Install dependencies on both machines:
sudo apt-get update
sudo apt-get install build-essential libncurses-dev cyclonedds-dev