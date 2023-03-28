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

#include <algorithm>

#include "image.h"
#include "point.h"

#if defined(_MSC_VER)
  #pragma warning(push, 0)
#else
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#if defined(_MSC_VER)
  #pragma warning (pop)
#else
  #pragma GCC diagnostic pop
#endif

namespace nkar
{

Image::Image()
  :
    m_data(nullptr),
    m_width(0),
    m_height(0)
{}

Image::Image(const std::string &file)
  :
    m_data(nullptr),
    m_width(0),
    m_height(0)
{
  open(file);
}

Image::Image(const Image &other)
{
  m_width = other.m_width;
  m_height = other.m_height;

  const size_t size = m_width * m_height * STBI_rgb;
  m_data = (unsigned char *)malloc(size);
  memcpy(m_data, other.m_data, size);
}

Image::~Image()
{
  stbi_image_free(m_data);
}

bool Image::open(const std::string &file)
{
  // We use stbi_load() function here to avoid additional dependencies.
  // However it can be replaced with other image readers. For instance QImage
  // class of Qt framework. The equivalent code would look like:
  // QImage image(m_file.c_str());
  // m_data = image.bits();
  // ...
  int n = 0;
  m_data = stbi_load(file.c_str(), &m_width, &m_height, &n, STBI_rgb);
  if (m_data == nullptr) {
    fprintf(stderr, "Error reading image file %s\n", file.c_str());
    return false;
  }
  return true;
}

bool Image::isNull() const
{
  return !m_data;
}

int Image::width() const
{
  return m_width;
}

int Image::height() const
{
  return m_height;
}

Color Image::pixel(int row, int column) const
{
  if (isNull()) {
    return Color();
  }

  assert(row < m_height && column < m_width);

  auto pos = (row * m_width + column) * STBI_rgb;

  auto red   = m_data[pos];
  auto green = m_data[pos + 1];
  auto blue  = m_data[pos + 2];

  return{ red, green, blue };
}

void Image::setPixel(int row, int column, const Color &color)
{
  if (isNull()) {
    return;
  }

  assert(row < m_height && column < m_width);

  auto pos = (row * m_width + column) * STBI_rgb;

  m_data[pos]     = color.red();
  m_data[pos + 1] = color.green();
  m_data[pos + 2] = color.blue();
}

//! Draws either a horizontal or vertical line.
void Image::drawLine(const Point &start, const Point &end, const Color &color)
{
  if (isNull()) {
    return;
  }

  for (int c = std::min(start.x(), end.x()); c <= std::max(start.x(), end.x()); ++c) {
    for (int r = std::min(start.y(), end.y()); r <= std::max(start.y(), end.y()); ++r) {
      setPixel(r, c, color);
    }
  }
}

bool Image::save(const std::string &file) const
{
  if (isNull())
  {
    return false;
  }

  return stbi_write_png(file.c_str(), width(), height(),
                        STBI_rgb, m_data, width() * STBI_rgb) != 0;
}

Image &Image::operator=(const Image &other)
{
  // Delete old data.
  stbi_image_free(m_data);

  m_width = other.m_width;
  m_height = other.m_height;

  const size_t size = m_width * m_height * STBI_rgb;
  m_data = (unsigned char *)malloc(size);
  memcpy(m_data, other.m_data, size);

  return *this;
}

}
