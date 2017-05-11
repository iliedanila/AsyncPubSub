# PubSub
Asynchronous Publisher - Subscriber library on top of a self-routing node network (C++ 14)

You'll need `boost` to run this. Please download and install the latest version.

There are solution files for `Visual Studio 2017` on Windows, `xcode` on Mac OS, and `codelite` on Linux.

Feel free to submit inquiries about the above software at `danila.ilie@gmail.com`

# Usage example:

`startNode --name broker --accept 7777 --as broker --logger`

`startNode --name publisher --connect-reconnect localhost 7777 --as publisher --publish dummyData --interval 1000`

`startNode --name subscriber --connect-reconnect localhost 7777 --as subscriber --subscription dummyData`
