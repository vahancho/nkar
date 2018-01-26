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
#include <map>

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
  {
    assert(m_begin < m_end);
  }

  bool operator<(const Edge &other) const
  {
    if (m_begin.x() < other.m_begin.x())
    {
      return true;
    }
    else if (m_begin.x() == other.m_begin.x())
    {
      if (m_begin.y() < other.m_begin.y())
      {
        return true;
      }
      else if (m_begin.y() == other.m_begin.y())
      {
        if (m_end.x() < other.m_end.x())
        {
          return true;
        }
        else if (m_end.x() == other.m_end.x())
        {
          return m_end.y() < other.m_end.y();
        }
      }
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

  Edge edge(int idx) const
  {
    assert(idx < pointCount());

    switch (idx)
    {
    case 0:
      return Edge(point(0), point(1));
    case 1:
      return Edge(point(1), point(2));
    case 2:
      return Edge(point(3), point(2));
    case 3:
      return Edge(point(0), point(3));
    default:
      // This should never happen.
      assert(false);
      return Edge(Point(0,0), Point(0,0));
    }
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

  //! Tests whether this scan rectangle contains a failed pixel.
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

  const Point &origin() const
  {
    return m_origin;
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

class Contours
{
public:
  using Contour = std::vector<Edge>;

  void addRect(const ScanRectangle &rect)
  {
    for (int i = 0; i < rect.pointCount(); ++i)
    {
      const Edge &edge = rect.edge(i);
      auto result = m_edges.emplace(edge, false);
      if (!result.second)
      {
        m_edges.erase(result.first);
      }
    }
  }

  std::vector<Contour> contours()
  {
    // Find connected components in an undirected graph by using depth-first search
    // algorithm.
    std::vector<Contour> contours;

    // Mark all as not visited.
    for (auto &e : m_edges)
    {
      e.second = false;
    }

    for (const auto &e : m_edges)
    {
      if (!e.second)
      {
        contours.emplace_back();
        printf("Contour %d\n", contours.size());
        // print all reachable edges from edge
        dfs(e.first, contours.back());
      }
    }
    return contours;
  }

private:
  void dfs(const Edge &edge, std::vector<Edge> &contour)
  {
    // Mark the current edge as visited and save it
    m_edges[edge] = true;
    contour.emplace_back(edge);

    printf("\tedge (%d, %d)(%d, %d)\n", edge.begin().x(), edge.begin().y(),
                                        edge.end().x(), edge.end().y());

    // Recur for all the edges adjacent to this edge
    for (const auto &e : m_edges)
    {
      if (!e.second)
      {
        const auto &currentEdge = e.first;
        if (edge.end()   == currentEdge.begin() || edge.end()   == currentEdge.end() ||
            edge.begin() == currentEdge.begin() || edge.begin() == currentEdge.end())
        {
          dfs(currentEdge, contour);
        }
      }
    }
  }

  std::map<Edge, bool> m_edges;
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
