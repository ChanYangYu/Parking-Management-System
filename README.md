# **Parking-Management-System**

> Parking-Management-System provides administrator monitoring and simulation programs.
> 

## Installation

```bash
# GCC update
sudo apt-get install -y build-essential

# GL library install
sudo apt-get install freeglut3-dev libglu1-mesa-dev mesa-common-dev
```

### Usage

- Build
    
    ```bash
    # 32-bit architecture
    make 32 
    
    # 64-bit architecture
    make 64
    ```
    
- Run Server
    
    ```bash
    # pwd = Parking-Management-System/
    cd server
    ./server
    ```
    
- Run Manager
    
    ```bash
    # pwd = Parking-Management-System/
    cd manager
    ./manager
    ```
    
- Run Client
    
    ```bash
    # pwd = Parking-Management-System/
    cd client
    ./client
    ```
    

## Directory

```bash
├── client
│   ├── bin
│   ├── calculatetime.cpp
│   ├── checkkey.cpp
│   ├── client.cpp
│   ├── include
│   ├── Makefile
│   ├── mobject
│   ├── textures
│   └── userclient.cpp
│
├── manager
│   ├── include
│   ├── Makefile
│   ├── manager.c
│   └── manager_title
│
├──server
│   ├── include
│   ├── _linkedList.c
│   ├── Makefile
│   ├── _parson.c
│   ├── server.c
│   ├── serverFunc.c
│   └── users.json
│
├── README.md
└── Makefile
```

## E**xecution-Environment**

```bash
Ubuntu 20.04.4 LTS

Linux version 5.13.0-30-generic
(buildd@lcy02-amd64-003)
(gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0,
GNU ld (GNU Binutils for Ubuntu) 2.34)
```