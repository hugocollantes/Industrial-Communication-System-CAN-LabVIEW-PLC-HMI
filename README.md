# Industrial Communication System: CAN → LabVIEW → PLC → HMI

## Overview
This project implements a complete industrial communication architecture that emulates a vehicle diagnostic system using CAN Bus, LabVIEW, TCP/IP, Modbus TCP, and a Siemens PLC with HMI visualization.

The system is designed from a system-engineering perspective, integrating embedded electronics, industrial networks, and software platforms commonly used in automotive and industrial automation environments.

---

## System Architecture
The implemented architecture follows a multi-layer industrial communication approach:

- CAN Bus (OBD-II based)
  - Node N2 (ECU Emulator) responds to diagnostic PID requests
  - Node N1 (Master / Diagnostic Tool) requests and receives data

- USB communication
  - Transfers CAN data from Arduino to PC

- LabVIEW
  - Real-time data acquisition
  - Data processing and scaling
  - TCP server implementation

- Ethernet network
  - TCP communication
  - Modbus TCP communication with PLC

- PLC (Siemens S7-1200 – TIA Portal)
  - Receives and processes diagnostic data

- HMI
  - Binary visualization
  - System status monitoring

---

## Implemented CAN PIDs
The following OBD-II PIDs were implemented and validated:

- PID 10 – single-byte variable
- PID 20 – two-byte variable

Data values are simulated with realistic behavior and transmitted cyclically through the complete communication chain.

---

## Key Features
- CAN Bus communication using Arduino-based nodes
- OBD-II diagnostic request/response protocol
- Real-time visualization in LabVIEW
- TCP encapsulation of CAN data
- Modbus TCP communication with industrial PLC
- Industrial HMI monitoring
- Parallel Ethernet communication channels

---

## Technologies Used
- Arduino with CAN Bus Shield
- CAN Bus (OBD-II protocol)
- LabVIEW
- TCP/IP
- Modbus TCP
- Siemens TIA Portal (S7-1200 PLC)
- Industrial HMI

---

## Project Context
This project was developed as part of an Industrial Communications course and focuses on realistic industrial system integration rather than isolated simulations.

It emphasizes:
- Embedded systems
- Industrial networking
- System-level engineering
- Hardware–software integration

---

## Author
Hugo Collantes  
Bachelor’s degree in Industrial Engineering (Electronics specialization)
