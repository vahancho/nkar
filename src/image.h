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

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include "color.h"
#include "export.h"

namespace nkar
{

class Point;

//! Implements an image data representation.
class NKAR_EXPORT Image
{
public:
  //! Default constructor creates an empty image.
  /*!
    Empty image has zero dimensions and contains no data.
  */
  Image();

  //! Constructs an image object and fills it with the image data
  Image(const std::string &file);

  //! Copy constructor
  Image(const Image &other);

  //! Destructor
  ~Image();

  //! Returns the width of the image.
  /*!
    Is image is not opened function returns 0.
  */
  int width() const;

  //! Returns the height of the image.
  /*!
    Is image is not opened function returns 0.
  */
  int height() const;

  //! Returns color of the given image pixel.
  Color pixel(int row, int column) const;

  //! Returns true if image object represents an empty image.
  bool isNull() const;

  //! Draws either a horizontal or vertical line.
  void drawLine(const Point &start, const Point &end, const Color &color);

  //! Save image to the given file.
  /*!
    \return true on success and false otherwise.
  */
  bool save(const std::string &file) const;

  //! The assignment operator.
  Image &operator = (const Image &other);

private:
  //! Opens the image file.
  /*!
    \param file The image file to open.
    \return true on success and false otherwise.
  */
  bool open(const std::string &file);

  //! Sets color of the particular pixel.
  void setPixel(int row, int column, const Color &color);

  unsigned char *m_data;
  int m_width;
  int m_height;
};

}

#endif // _IMAGE_H_
