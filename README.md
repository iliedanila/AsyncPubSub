# PubSub
Asynchronous Publisher - Subscriber library on top of a self-routing node network (C++20)

This allows to create a dynamic network of publishers and subscribers (you can add/remove nodes while the system is running, add/remove subscriptions or start/stop certain publishers).

# Requirements:
* CMake 3.25 or newer and a C++20 compiler
* boost 1.84 or newer - install with `sudo apt install libboost-all-dev` on Ubuntu 26.04 or newer (24.04 ships 1.83) and `brew install boost` on MacOS

Feel free to submit inquiries about the above software at `danila.ilie@gmail.com`

# Build:
`./build.sh` (the `release` preset, into `build/release/`)

# Test:
`ctest --preset release`, or `cmake --workflow --preset debug` to configure, build and test a debug build

# Usage example:

`startNode --name broker --accept 7777 --as broker --logger`

`startNode --name publisher --connect-reconnect localhost 7777 --as publisher --publish dummyData --interval 1000`

`startNode --name subscriber --connect-reconnect localhost 7777 --as subscriber --subscription dummyData`
