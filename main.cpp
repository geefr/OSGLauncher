/**
Copyright (c) 2017, Gareth Francis
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>

#include <osgGA/TrackballManipulator>
#include <osgGA/NodeTrackerManipulator>

#include <tinyxml2.h>

#include "main.h"
#include "menuentry.h"
#include "inputhandler.h"

#include <memory>

int main(int argc, const char** argv)
{
  Main m;
  return m.run(argc, argv);
}

Main::Main()
  : m_enterPressed{ false }
{

}

int Main::run(int argc, const char** argv)
{
  if( argc < 2 )
  {
    std::cerr << "Usage: ./OSGLauncher <osglauncher.xml>" << std::endl;
    return 1;
  }

  std::shared_ptr<std::vector<std::shared_ptr<MenuEntry>>> entries( new std::vector<std::shared_ptr<MenuEntry>>() );

  {
    const char* configXML{ argv[1] };
    tinyxml2::XMLDocument doc;

    if( doc.LoadFile( configXML ) != tinyxml2::XML_SUCCESS )
    {
      std::cerr << "Error: Invalid configuration file provided" << std::endl;
      return 1;
    }

    // We're looking for:
    // - menu
    // - menuentry (one or more)
    tinyxml2::XMLElement* entry{ doc.FirstChildElement("menuentry") };
    if( entry == nullptr )
    {
      std::cerr << "Error: No <menuentry> in configuration file" << std::endl;
      return 1;
    }

    std::shared_ptr<MenuEntry> menuEntry( new MenuEntry(entry) );
    entries->emplace_back( menuEntry );

    tinyxml2::XMLElement* sibling{ entry->NextSiblingElement("menuentry") };
    while( sibling != nullptr )
    {
      std::shared_ptr<MenuEntry> menuEntry( new MenuEntry(sibling) );
      entries->emplace_back( menuEntry );
      sibling = sibling->NextSiblingElement("menuentry");
    }
  }

  // OSG setup
  osgViewer::Viewer viewer;
  osg::Group* root = new osg::Group();

  osg::ref_ptr<InputHandler> inputHandler( new InputHandler(this, entries) );

  viewer.setSceneData( root );
  //viewer.setCameraManipulator( new osgGA::TrackballManipulator() );
  viewer.getCamera()->setViewMatrixAsLookAt(
        osg::Vec3(0.0, -3.0, 0.0), osg::Vec3(), osg::Z_AXIS );
  viewer.getCamera()->setAllowEventFocus( false );
  //viewer.setUpViewInWindow(30, 30, 800, 600);
  viewer.addEventHandler(inputHandler);
  viewer.realize();

  // Main program loop
  while( !viewer.done() )
  {
    // No this isn't efficient
    root->removeChildren(0, root->getNumChildren());

    unsigned int currentIndex{ inputHandler->currentIndex() };

    std::shared_ptr<MenuEntry> currentEntry( entries->operator[](currentIndex));
    root->addChild( currentEntry->osgGeode() );

    viewer.frame();

    if( m_enterPressed )
    {
      // Launch the entry
      // For most commands we'll wait until it finishes
      // Some things like gvim automatically fork again to run as a separate process and not interrupt the
      // calling shell. In this case the viewer will keep running and use system resources unecesarrily
      // TODO: Shutdown completely until the command finishes
      int result{ system(currentEntry->command().c_str()) };
      if( result != 0 )
      {
        std::cerr << "Command failed (" << result << "): " << currentEntry->command();
      }
      m_enterPressed = false;
    }
  }
  return 0;
}
