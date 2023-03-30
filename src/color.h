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

#ifndef _COLOR_H_
#define _COLOR_H_

#include "export.h"

#include <cstdint>

namespace nkar
{

//! Implements an RGB based color
class NKAR_EXPORT Color
{
public:
  //! Constructs a color with the RGB value \p red, \p green, \p blue.
  /*
    A color is normally specified in terms of RGB (red, green, and blue) components.
  */
  Color(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0);

  //! Compares two colors.
  /*!
    Colors are equal if their all RGB components are equal.
  */
  bool operator!=(const Color &other);

  //! Returns the red component of the color.
  uint8_t red() const;

  //! Returns the green component of the color.
  uint8_t green() const;

  //! Returns the blue component of the color.
  uint8_t blue() const;

private:
  uint32_t m_rgba{0};
};

}

#endif // _COLOR_H_
