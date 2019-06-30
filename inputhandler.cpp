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

#include "inputhandler.h"

#include <iostream>

InputHandler::InputHandler( Main* main, std::shared_ptr< std::vector< std::shared_ptr<MenuEntry> > > entries )
  : m_main( main )
  , m_entries( entries )
  , m_currentIndex{ 0 }
{

}

InputHandler::~InputHandler()
{

}

bool InputHandler::handle( const osgGA::GUIEventAdapter& ea, [[maybe_unused]] osgGA::GUIActionAdapter& aa )
{
  switch( ea.getEventType() )
  {
    case osgGA::GUIEventAdapter::KEYDOWN:
      switch( ea.getKey() )
      {
        case osgGA::GUIEventAdapter::KEY_Right:
        if( m_currentIndex < m_entries->size() - 1 ) ++m_currentIndex;
          //std::cerr << "Info: Selected " << m_currentIndex + 1 << " of " << m_entries->size() << std::endl;
          break;
        case osgGA::GUIEventAdapter::KEY_Left:
        if( m_currentIndex > 0 ) --m_currentIndex;
          //std::cerr << "Info: Selected " << m_currentIndex + 1 << " of " << m_entries->size() << std::endl;
          break;
        case osgGA::GUIEventAdapter::KEY_Return:
          m_main->enterPressed();
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return false;
}
