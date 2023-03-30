/**********************************************************************************
*  MIT License                                                                    *
*                                                                                 *
*  Copyright (c) 2017 Vahan Aghajanyan <vahancho@gmail.com>                       *
*                                                                                 *
*  Permission is hereby granted, free of charge, to any person obtaining a copy   *
*  of this software and associated documentation files (the "Software"), to deal  *
*  in the Software without restriction, including without limitation the rights   *
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
*  copies of the Software, and to permit persons to whom the Software is          *
*  furnished to do so, subject to the following conditions:                       *
*                                                                                 *
*  The above copyright notice and this permission notice shall be included in all *
*  copies or substantial portions of the Software.                                *
*                                                                                 *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
*  SOFTWARE.                                                                      *
***********************************************************************************/

#include "color.h"

namespace nkar
{

Color::Color(uint8_t r, uint8_t g, uint8_t b)
  :
    m_rgba{(uint32_t) r | (uint32_t) g << 8 | (uint32_t) b << 16 | (uint32_t) 0 << 24}
{
}

bool Color::operator!=(const Color &other)
{
  /*
  return m_red   != other.m_red ||
         m_green != other.m_green ||
         m_blue  != other.m_blue;
  */
  return m_rgba != other.m_rgba;
}

uint8_t Color::red() const
{
  //return m_red;
  return (uint8_t)m_rgba;
}

uint8_t Color::green() const
{
  //return m_green;
  return (uint8_t)(m_rgba >> 8);
}

uint8_t Color::blue() const
{
  //return m_blue;
  return (uint8_t)(m_rgba >> 16);
}

}
