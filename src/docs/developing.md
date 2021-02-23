# Developing the server #
[TOC]

## Dependencies ##

This project has 4 dependencies

- [json for modern C++](https://github.com/nlohmann/json) for JSON support (header only)
- [jsonrpcpp](https://github.com/badaix/jsonrpcpp) for JSON-RPC support (header only)
- [boost](https://www.boost.org)
- [pugixml](https://pugixml.org) (header only)

----------------

## How to build ##

This project was created on Windows 64bit and is not tested to compile/work on other systems but will probably work after some configuration, as all dependencies and the project's sources are cross-platform.

[CMake](https://cmake.org) is used to build the project together with the [mingw-w64](http://mingw-w64.org/doku.php/start) toolchain.

You can either build the server using the [VSCode CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) or manually.

### Manual install ###

~~~~~~~~~~~~~~~~~~~~~~~
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make.exe
~~~~~~~~~~~~~~~~~~~~~~~

### Install using CMake Tools ###

1. Open the repository as a workspace in VSCode.
2. Open CMakeLists.txt in the root directory.
3. Save, CMake Tools should now configure the build files.
Alternatively, configure the project manually.
4. Press F7 or click on build in the bottom row.

If errors occur, make sure to select the build target "XML_GridViewServer".

-----------------

## How to run / debug ##

The server itself does pretty much nothing on its own. It tries to connect to 127.0.0.1 at a given port and closes if it can't connect.
The server is supposed to connect to the corresponding VSCode Client Extension available [here](https://github.com/JonasRock/XML_GridView).

Aforementioned client extension will instantiate the server process on its own, making it really hard to debug, but can be configured to wait for an external process to connect instead.
This allows us to run the server ourselves manually with a debugger.
To configure the client to wait for an exteral process is currently only possible through commenting out

~~~~~~~~~~~~~~~~~~~~~~~~~ts
//extension.ts: createServerWithSocket():

exec = child_process.spawn(executablePath)
~~~~~~~~~~~~~~~~~~~~~~~~~

in the clients extension.ts file

Now we can start a VSCode instance with the extension enabled (F5), open a XML file using the Grid View (so the extension starts) and then start the server to connect to the extension.
In the Debug Log of the VSCode instance thats owning the Extension Development Host (The one you started the extension with) should now log what port number VSCode will listen to. Either provide the port as an argument to the server or enter it manually after starting.

-----------------

## Structure / Architecture ##

The Server strucute is based on [this project](https://github.com/JonasRock/ARXML_LanguageServer), with a couple of differences, so its documentation should transfer for the most part.

The communication protocol used here NOT the Language Server Protocol, just plain JSON-RPC, with messages delimited by "\r\n\r\n"

The backend parser is pugixml instead of a custom solution.

### Structure ###

- xmlServer::IOHandler: Manages the connection to the socket, as well as reading and writing
- xmlServer::XmlService: Contains main routine and all callbacks
- xmlServer::MessageParer: Manages parsing of messages and management of corresponding callbacks
- xmlServer::XmlParser: Contains the pugixml xml parser, responsible for the parsed data.

### Startup ###

The communication between server and client is using sockets. The client owns the connection and starts the server who connects to the client.

### Request Handling ###

The server waits until a message is sent to it, parses the message, calculates the responses and sends them back. This loop is synchronous, the server can only process the next message after the responses to the first one are sent back. Multiple responses from the server to the client are supported

For every request/notification we can receive based on our initialization, we register a callback function in the lsp::LanguageService that owns the lsp::MessageParser that handles the requests.

The lsp::MessageParser provides the arguments for the request to the callback, that processes the request, formulates a result and passes it back to the parser, that serializes it and passes it back to the main to be sent back out to the client via socket.

The data flow can be visualized as such:

![](src/docs/img/xmlServerRequestSequence.png)

### Request to the Client ###

The server can also request information from the client. To do this, you can add additional request to be sent back together with the original response in another callback. This works the same way as for the [other server linked above](https://github.com/JonasRock/ARXML_LanguageServer)

## Clientside Extension ##

The clientside Extension is documented [here](https://github.com/JonasRock/XML_GridView)