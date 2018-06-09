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

    std::shared_ptr<MenuEntry> menuEntry( new MenuEntry(entry, configXML) );
    entries->emplace_back( menuEntry );

    tinyxml2::XMLElement* sibling{ entry->NextSiblingElement("menuentry") };
    while( sibling != nullptr )
    {
      std::shared_ptr<MenuEntry> menuEntry( new MenuEntry(sibling, configXML) );
      entries->emplace_back( menuEntry );
      sibling = sibling->NextSiblingElement("menuentry");
    }
  }

  // OSG setup
  osgViewer::Viewer viewer;
  osg::Group* root = new osg::Group();

  osg::ref_ptr<InputHandler> inputHandler( new InputHandler(this, entries) );

  viewer.setSceneData( root );
  //viewer.setUpViewInWindow(30, 30, 800, 600);

  // Setup scene graph
  // For now just load everything at once as I'm only planning on a few menu entries
  // TODO: Worry about paging in entries to avoid hogging memory
  double entryPos = 0.0;
  double entryPosDelta = 1.2;
  for( auto entry : *(entries.get()) )
  {
    osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform();
    transform->setPosition( osg::Vec3d(entryPos, 0.0, 0.0) );
    entryPos += entryPosDelta;
    transform->addChild( entry->osgGeode() );
    root->addChild( transform );
  }

  viewer.addEventHandler(inputHandler);
  viewer.realize();

  // Limit framerate to 60fps at least otherwise we're just wasting power/to heat the GPU up
  auto minFrameTime = 1.0 / 60.0;

  // Main program loop
  while( !viewer.done() )
  {
    auto startTick = osg::Timer::instance()->tick();

    auto currentIndex = inputHandler->currentIndex();
    viewer.getCamera()->setViewMatrixAsLookAt(
          osg::Vec3d(currentIndex * entryPosDelta, -3.0, 0.0),
          osg::Vec3(currentIndex * entryPosDelta, 0.0, 0.0),
          osg::Z_AXIS );

    std::shared_ptr<MenuEntry> currentEntry( entries->operator[](currentIndex));

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
      // Some events can be stuck in the queue while applications are quitting
      viewer.getEventQueue()->clear();
    }

    auto endTick = osg::Timer::instance()->tick();
    auto frameTime = osg::Timer::instance()->delta_s(startTick, endTick);
    if( frameTime < minFrameTime )
    {
      OpenThreads::Thread::microSleep( 1e6 * (minFrameTime - frameTime) );
    }
  }
  return 0;
}
