# ARXML GridView

[TOC]

This extension provides a structured alternative view for XML files

## Prerequesites ##

The Gridview-Server used for parsing the XML information

For bigger files (>50MB) VSCode disables extensions. To get around this, the extension [Large file support for extensions](https://marketplace.visualstudio.com/items?itemName=mbehr1.vsc-lfs) is needed.

--------------

## Features ##

- **Grid view**: Shows the structure of XML Documents in a clear and concise manner
- **Go to Position** Jump to the corresponding element in the text view by using the context menu

## Common issues ##

### Extension does not work on big files ###

For files bigger than around 50mb, Visual Studio Code disables all extensions automatically.
The current workaround for this is to use the VSCode extension
[Large file support for extensions](https://marketplace.visualstudio.com/items?itemName=mbehr1.vsc-lfs).
Install it and use to command (Ctrl+Shift+P) **open large file...**
This can currently only open the file in readonly mode.

## Developing the server ##

[Read Here](src/docs/Developing.md)