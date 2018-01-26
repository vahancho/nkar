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

#ifndef COLOR_H
#define COLOR_H

//! Implements an RGB based color
class Color
{
public:
  //! Construct a color object.
  /*
    A color is normally specified in terms of RGB (red, green, and blue) components.
  */
  Color(int red, int green, int blue);

  //! Compares two colors.
  /*!
    Colors are equal if their all components are equal.
  */
  bool operator!=(const Color &other);

  //! Returns the red component of the color.
  int red() const;

  //! Returns the green component of the color.
  int green() const;

  //! Returns the blue component of the color.
  int blue() const;

private:
  int m_red;
  int m_green;
  int m_blue;
};


#endif // COLOR_H