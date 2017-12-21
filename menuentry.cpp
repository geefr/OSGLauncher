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

#include "menuentry.h"

#include <osg/Texture2D>
#include <osg/Geometry>
#include <osgDB/ReadFile>

MenuEntry::MenuEntry( const tinyxml2::XMLElement* xmlEntry )
{
  // We're looking for an <image> and a <command>
  // TODO: Error handling
  const tinyxml2::XMLElement* xmlImage{ xmlEntry->FirstChildElement("image") };
  const tinyxml2::XMLElement* xmlCommand{ xmlEntry->FirstChildElement("command") };
  if( xmlImage )
  {
    const char* xmlImageText{ xmlImage->GetText() };
    if( xmlImageText )
    {
      m_image = xmlImageText;
    }
  }
  if( xmlCommand )
  {
    const char* xmlCommandText{ xmlCommand->GetText() };
    if( xmlCommandText )
    {
      m_command = xmlCommandText;
    }
  }
}

MenuEntry::MenuEntry(const std::string& image, const std::string& command)
  : m_image( image )
  , m_command( command )
{

}

MenuEntry::~MenuEntry()
{

}

osg::ref_ptr<osg::Geode> MenuEntry::osgGeode()
{
  if( m_osgGeode )
  {
    return m_osgGeode;
  }

  osg::ref_ptr<osg::Vec3Array> vertices( new osg::Vec3Array() );
  vertices->push_back( osg::Vec3(-0.5, 0.0, -0.5) );
  vertices->push_back( osg::Vec3( 0.5, 0.0, -0.5) );
  vertices->push_back( osg::Vec3( 0.5, 0.0,  0.5) );
  vertices->push_back( osg::Vec3(-0.5, 0.0,  0.5) );
  vertices->push_back( osg::Vec3(-0.5, 0.0, -0.5) );
  vertices->push_back( osg::Vec3( 0.5, 0.0,  0.5) );

  osg::ref_ptr<osg::Vec3Array> normals( new osg::Vec3Array() );
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );

  osg::ref_ptr<osg::Vec2Array> texCoords( new osg::Vec2Array );
  texCoords->push_back( osg::Vec2( 0.0, 0.0 ) );
  texCoords->push_back( osg::Vec2( 1.0, 0.0 ) );
  texCoords->push_back( osg::Vec2( 1.0, 1.0 ) );
  texCoords->push_back( osg::Vec2( 0.0, 1.0 ) );
  texCoords->push_back( osg::Vec2( 0.0, 0.0 ) );
  texCoords->push_back( osg::Vec2( 1.0, 1.0 ) );

  osg::ref_ptr<osg::Geometry> quad( new osg::Geometry );
  quad->setVertexArray( vertices );
  quad->setNormalArray( normals );
  quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
  quad->setTexCoordArray( 0, texCoords );
  quad->addPrimitiveSet( new osg::DrawArrays( GL_TRIANGLES, 0, 6 ) );

  osg::ref_ptr<osg::Texture2D> texture( new osg::Texture2D() );
  osg::ref_ptr<osg::Image> image( osgDB::readImageFile(m_image) );
  texture->setImage( image );

  m_osgGeode = new osg::Geode();
  m_osgGeode->addDrawable( quad );
  m_osgGeode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);

  return m_osgGeode;
}