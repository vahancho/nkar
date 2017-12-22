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

#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <set>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

class Point
{
public:
  Point(int x = 0, int y = 0)
    :
      m_x(x),
      m_y(y)
  {}

  bool operator==(const Point &other) const
  {
    return m_x == other.m_x && m_y == other.m_y;
  }

  bool operator<(const Point &other) const
  {
    if (m_x < other.m_x)
    {
      return true;
    }
    else if (m_x == other.m_x)
    {
      return m_y < other.m_y;
    }
    return false;
  }

  int x() const
  {
    return m_x;
  }

  int y() const
  {
    return m_y;
  }

  int &x()
  {
    return m_x;
  }

  int &y()
  {
    return m_y;
  }

private:
  int m_x;
  int m_y;
};

class Edge
{
public:
  Edge(const Point &begin, const Point &end)
    :
      m_begin(begin),
      m_end(end)
  {}

  bool operator<(const Edge &other) const
  {
    // Sort vertically
    if (m_begin.y() < other.m_begin.y())
    {
      return true;
    }
    else if (m_begin.y() == other.m_begin.y())
    {
      return m_begin.x() < other.m_begin.x();
    }
    return false;
  }

  const Point &begin() const
  {
    return m_begin;
  }

  const Point &end() const
  {
    return m_end;
  }

private:
  Point m_begin;
  Point m_end;
};

class Color
{
public:
  Color(int r, int g, int b)
    :
      m_red(r),
      m_green(g),
      m_blue(b)
  {}

  bool operator!=(const Color &other)
  {
    return m_red   != other.m_red ||
           m_green != other.m_green ||
           m_blue  != other.m_blue;
  }

//private:
  int m_red;
  int m_green;
  int m_blue;
};

class Image
{
public:
  Image(const std::string &file)
    :
      m_file(file),
      m_data(nullptr)
  {}

  ~Image()
  {
    stbi_image_free(m_data);
  }

  bool open()
  {
    // We use stbi_load() function here to avoid additional dependencies.
    // However it can be replaced with other image readers. For instance QImage
    // class of Qt framework. The equivalent code would look like:
    // QImage image(m_file.c_str());
    // m_data = image.bits();
    // ...
    int n;
    m_data = stbi_load(m_file.c_str(), &m_width, &m_height, &n, STBI_rgb);
    if (m_data == nullptr)
    {
      fprintf(stderr, "Error reading image file %s\n", m_file.c_str());
      return false;
    }
    printf("Image (%d, %d), %dbit\n", m_width, m_height, n);
    return true;
  }

  int width() const
  {
    return m_width;
  }

  int height() const
  {
    return m_height;
  }

  Color pixel(int row, int column) const
  {
    assert(row < m_height && column < m_width);

    auto pos = (row * m_width + column) * STBI_rgb;

    auto red   = m_data[pos];
    auto green = m_data[pos + 1];
    auto blue  = m_data[pos + 2];

    return { red, green, blue };
  }

  void setPixel(int row, int column, const Color &color)
  {
    assert(row < m_height && column < m_width);

    auto pos = (row * m_width + column) * STBI_rgb;

    m_data[pos]     = color.m_red;
    m_data[pos + 1] = color.m_green;
    m_data[pos + 2] = color.m_blue;
  }

  //! Draws either a horizontal or vertical line.
  void drawLine(const Point &start, const Point &end, const Color &color)
  {
    for (int c = std::min(start.x(), end.x()); c <= std::max(start.x(), end.x()); ++c)
    {
      for (int r = std::min(start.y(), end.y()); r <= std::max(start.y(), end.y()); ++r)
      {
        setPixel(r, c, color);
      }
    }
  }

  bool save(const std::string &file) const
  {
    return stbi_write_png(file.c_str(), width(), height(),
                          STBI_rgb, m_data, width() * STBI_rgb) != 0;

  }

private:
  std::string m_file;
  unsigned char *m_data;
  int m_width;
  int m_height;
};

//! Implement a scanning rectangle.
/*!
  This is a rectangle that moves through the whole image step by step until it
  reaches the lower right corner of the image. It moves from left to right and
  from top to bottom - this is scanning order.
*/
class ScanRectangle
{
public:
  ScanRectangle(const Point &origin, int width, int height,
                std::shared_ptr<Image> img1,
                std::shared_ptr<Image> img2)
    :
      m_origin(origin),
      m_width(width),
      m_height(height),
      m_img1(img1),
      m_img2(img2),
      m_xLimit(std::min(img1->width(), img2->width())),
      m_yLimit(std::min(img1->height(), img2->height()))
  {}

  int pointCount() const
  {
    return 4;
  }

  Point point(int idx) const
  {
    assert(idx < pointCount());

    // 0 +---------+ 1
    //   |         |
    //   |         |
    // 3 +---------+ 2

    switch (idx) {
    case 0:
      return m_origin;
    case 1:
      return Point(std::min(m_origin.x() + m_width, m_xLimit), m_origin.y());
    case 2:
      return Point(std::min(m_origin.x() + m_width, m_xLimit),
                   std::min(m_origin.y() + m_height, m_yLimit));
    case 3:
      return Point(m_origin.x(),
                   std::min(m_origin.y() + m_height, m_yLimit));
    default:
      break;
    }
    return Point();
  }

  bool test() const
  {
    const int cMax = std::min(m_origin.x() + m_width, m_xLimit);
    const int rMax = std::min(m_origin.y() + m_height, m_yLimit);
    for (int c = m_origin.x(); c < cMax; c++)
    {
      for (int r = m_origin.y(); r < rMax; r++) {
        Color color1 = m_img1->pixel(r, c);
        Color color2 = m_img2->pixel(r, c);

        if (color1 != color2) {
          fprintf(stderr, "Different pixels at (%d, %d)\n", r, c);
          return false;
        }
      }
    }
    return true;
  }

  //! Indicates whether the rectangle reached the end of scanning.
  bool atEnd() const
  {
    return m_origin.x() == 0 &&
           m_origin.y() >= m_yLimit;
  }

  //! A pre-increment operator.
  ScanRectangle &operator++()
  {
    if (!atEnd()) {
      if (m_origin.x() < m_xLimit)
      {
        m_origin.x() += m_width;
      }
      else if (m_origin.y() < m_yLimit)
      {
        // Move to the next row and left most position.
        m_origin.x() = 0;
        m_origin.y() += m_height;
      }
    }

    return *this;
  }

private:
  Point m_origin;
  int m_width;
  int m_height;
  int m_xLimit;
  int m_yLimit;

  std::shared_ptr<Image> m_img1;
  std::shared_ptr<Image> m_img2;
};

class Contour
{
public:
  void addEdge(const Edge &edge)
  {
    m_edges.emplace(edge);
  }

//private:
  std::set<Edge> m_edges;
};

class Contours
{
public:
  size_t pointCount() const
  {
    return m_points.size();
  }

  void addRect(const ScanRectangle &rect)
  {
    for (int i = 0; i < rect.pointCount(); ++i) {
      auto result = m_points.emplace(rect.point(i));
      if (!result.second) {
        m_points.erase(result.first);

        for (auto &contour : m_contours)
        {
          contour.erase(rect.point(i));
        }
      }
      else if (i == 0)
      {
        printf("new contour (%d, %d)\n", rect.point(i).m_x, rect.point(i).m_y);
        m_contours.emplace_back();
        m_contours.back().emplace(rect.point(i));
      }
      else
      {
        m_contours.back().emplace(rect.point(i));
      }
    }
  }

  void extractContours()
  {
    assert(pointCount() % 2 == 0);

    Contour contour;
    auto it = m_points.begin();
    while (it != m_points.end())
    {
      const Point &p1 = *it;
      const Point &p2 = *(std::next(it));
      Edge edge{ p1, p2 };

      contour.addEdge(edge);

      std::advance(it, 2);
    }

    int a = 0;

    auto eit = contour.m_edges.begin();
    while (eit != contour.m_edges.end())
    {
      auto eitNext = contour.m_edges.begin();
      while (eitNext != contour.m_edges.end())
      {
        if (eit->m_begin.m_y == eitNext->m_begin.m_y)
        {
          printf("connection: (%d, %d) -> (%d, %d)\n",
                 eit->m_begin.m_x, eit->m_begin.m_y,
                 eitNext->m_begin.m_x, eitNext->m_begin.m_y);
        }
        else if (eit->m_end.m_y == eitNext->m_end.m_y)
        {
          printf("connection: (%d, %d) -> (%d, %d)\n",
                 eit->m_end.m_x, eit->m_end.m_y,
                 eitNext->m_end.m_x, eitNext->m_end.m_y);
        }
        ++eitNext;
      }
      ++eit;
    }
  }

private:
  std::set<Point> m_points;
  std::vector<std::set<Point>> m_contours;
};

int main(int argc, char **argv)
{
  std::string file1("d:/test1.png");
  std::string file2("d:/test2.png");

  std::shared_ptr<Image> img1 = std::make_shared<Image>(file1);
  std::shared_ptr<Image> img2 = std::make_shared<Image>(file2);

  if (!img1->open() || !img2->open()) {
    return 1;
  }

  if (img1->width() != img2->width() || img1->height() != img2->height()) {
    fprintf(stderr, "Images have different dimensions\n");
    return 1;
  }

  Point origin{0, 0};
  ScanRectangle sr(origin, 3, 3, img1, img2);
  std::vector<ScanRectangle> failedRects;
  while (!sr.atEnd()) {
    if (!sr.test()) {
      failedRects.emplace_back(sr);
    }
    ++sr;
  }

  Contours contours;
  for (const auto &rect : failedRects) {
    contours.addRect(rect);
  }
  assert(contours.pointCount() % 2 == 0);

  contours.extractContours();

  return 0;
}
