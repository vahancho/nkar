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

#ifndef _POINT_H_
#define _POINT_H_

#include "export.h"

namespace nkar
{

//! Implements a point in 2D space.
class NKAR_EXPORT Point
{
public:
  //! Constructs a point object with the integer coordinates.
  Point(int x = 0, int y = 0);

  //! Compares two points.
  /*!
    Points are equal if they have the same coordinates.
  */
  bool operator==(const Point &other) const;

  //! Compare two points.
  /*!
    A point is less than other point if it's upper and lefter of \p other point.
  */
  bool operator<(const Point &other) const;

  //! Returns the x coordinate of the point
  int x() const;

  //! Returns the y coordinate of the point
  int y() const;

  //! Returns the x coordinate of the point
  int & x();

  //! Returns the y coordinate of the point
  int & y();

private:
  int m_x;
  int m_y;
};

}

#endif // _POINT_H_
