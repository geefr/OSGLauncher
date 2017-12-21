# OSGLauncher
Simple program launcher/frontend using OpenSceneGraph

Requirements:
* OpenSceneGraph
* tinyxml2

Usage: ./OSGLauncher <config.xml>

Config is an xml file with one or more <menuentry> elements, see config/osglauncher.xml for an example

Launcher will display first entry in config file to start with
Left and right arrow keys scroll through the list
Return key runs the command for the menu entry
