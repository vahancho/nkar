/*
MIT License

Copyright (c) 2017 Vahan Aghajanyan <vahancho@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "point.h"

namespace nkar
{

Point::Point(int x, int y)
  :
    m_x(x),
    m_y(y)
{}

bool Point::operator==(const Point &other) const
{
  return m_x == other.m_x && m_y == other.m_y;
}

bool Point::operator<(const Point &other) const
{
  if (m_x < other.m_x) {
    return true;
  } else if (m_x == other.m_x) {
    return m_y < other.m_y;
  }
  return false;
}

int Point::x() const
{
  return m_x;
}

int Point::y() const
{
  return m_y;
}

int & Point::x()
{
  return m_x;
}

int & Point::y()
{
  return m_y;
}

}
