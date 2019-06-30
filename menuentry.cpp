/**
Copyright (c) 2019, Gareth Francis
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
#include "settings.h"

#include <osg/Texture2D>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgText/Text>

MenuEntry::MenuEntry( const tinyxml2::XMLElement* xmlEntry, std::string xmlFile )
{
  // We're looking for an <image> and a <command>
  // TODO: Error handling
  const tinyxml2::XMLElement* xmlImage{ xmlEntry->FirstChildElement("image") };
  const tinyxml2::XMLElement* xmlCommand{ xmlEntry->FirstChildElement("command") };
  const tinyxml2::XMLElement* xmlName{ xmlEntry->FirstChildElement("name") };
  if( xmlImage )
  {
    const char* xmlImageText{ xmlImage->GetText() };
    if( xmlImageText )
    {
      std::string imgFilename( xmlImageText );
#ifndef MSC_VER
      if( imgFilename.compare(0, 1, "/", 1) != 0 )
      {
        // Path isn't absolute, assume relative to the config file
        auto lastSlashXmlDir = xmlFile.find_last_of('/');
        if( lastSlashXmlDir == std::string::npos )
        {
          // Config must be in pwd
          // Paths left relative to pwd
        }
        else
        {
          m_image = xmlFile.substr(0, lastSlashXmlDir);
          m_image.append("/");
          m_image.append( xmlImageText );
        }
      }
#else
# error "Windows not currently supported"
#endif
      else
      {
        m_image = imgFilename;
      }
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
  if( xmlName )
  {
    const char* xmlNameText{ xmlName->GetText() };
    if( xmlNameText )
    {
      m_name = xmlNameText;
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

osg::ref_ptr<osg::Group> MenuEntry::osgGroup()
{
  if( m_osgGroup )
  {
    return m_osgGroup;
  }

  m_osgGroup = new osg::Group();

  // Geode to display textured quad
  {
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

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode = new osg::Geode();
    geode->addDrawable( quad );
    geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);

    m_osgGroup->addChild(geode);
  }

  // 3D text displaying entry name
  if( !m_name.empty() )
  {
    osg::ref_ptr<osgText::Text> text = new osgText::Text();
    text->setFont(Settings::instance().font());
    text->setCharacterSize( 0.15f );
    text->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    text->setAxisAlignment( osgText::TextBase::XZ_PLANE );
    text->setPosition( osg::Vec3(0.0f, 0.0f, -0.75f) );
    text->setText( m_name );
    text->setAlignment( osgText::TextBase::CENTER_BOTTOM );
    osg::ref_ptr<osg::Geode> textGeode = new osg::Geode();
    textGeode->addDrawable(text);

    m_osgGroup->addChild(textGeode);
  }

  return m_osgGroup;
}
