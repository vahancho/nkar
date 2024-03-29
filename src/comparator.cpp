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

#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <stack>
#include <set>

#include "comparator.h"
#include "point.h"

namespace nkar
{

// Default width and height of the scan rectangle. Each of parameters should be
// greater or equal to one. Smaller values correspond to the more precise and slower calculations.
static constexpr int s_scanRectWidth = 1;  // The highest horizontal precision
static constexpr int s_scanRectHeight = 1; // The highest vertical precision

#if __cplusplus > 201103L
  // std::max is constexpr starting from c++14
  static constexpr int s_maxRectDimentinon = std::max(s_scanRectWidth, s_scanRectHeight);
#else
  static constexpr int s_maxRectDimentinon = 1;
#endif

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
    if (m_begin.x() < other.m_begin.x()) {
      return true;
    } else if (m_begin.x() == other.m_begin.x()) {
      if (m_begin.y() < other.m_begin.y()) {
        return true;
      } else if (m_begin.y() == other.m_begin.y()) {
        if (m_end.x() < other.m_end.x()) {
          return true;
        } else if (m_end.x() == other.m_end.x()) {
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

  bool visited() const
  {
    return m_visited;
  }

  void setVisited()
  {
    m_visited = true;
  }

private:
  Point m_begin;
  Point m_end;
  bool m_visited{ false };
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
                const Image &img1, const Image &img2)
  :
    m_origin(origin),
    m_width(width),
    m_height(height),
    m_img1(img1),
    m_img2(img2),
    m_xLimit(std::min(img1.width() - 1, img2.width() - 1)),
    m_yLimit(std::min(img1.height() - 1, img2.height() - 1))
  {}

  int pointCount() const
  {
    return 4;
  }

  Edge edge(int idx) const
  {
    assert(idx < pointCount());

    //        0
    //   +---------+
    // 3 |         | 1
    //   |         |
    //   +---------+
    //       2

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
      return Edge(Point(0, 0), Point(0, 0));
    }
  }

  Point point(int idx) const
  {
    assert(idx < pointCount());

    // 0 +---------+ 1
    //   |         |
    //   |         |
    // 3 +---------+ 2

    switch (idx)
    {
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
    for (int c = m_origin.x(); c <= cMax; c++) {
      for (int r = m_origin.y(); r <= rMax; r++) {
        Color color1 = m_img1.pixel(r, c);
        Color color2 = m_img2.pixel(r, c);

        if (color1 != color2) {
          // Exist as soon as the first failure detected.
          return false;
        }
      }
    }
    return true;
  }

  //! Indicates whether the rectangle reached the end of scanning.
  bool atEnd() const
  {
    return m_origin.x() == 0 && m_origin.y() >= m_yLimit;
  }

  //! A pre-increment operator.
  ScanRectangle &operator++()
  {
    if (!atEnd()) {
      if (m_origin.x() < m_xLimit - m_width) {
        m_origin.x() += m_width;
      } else if (m_origin.y() < m_yLimit) {
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
  ScanRectangle& operator=(const ScanRectangle&) = delete;

  Point m_origin;
  int m_width;
  int m_height;

  const Image &m_img1;
  const Image &m_img2;

  int m_xLimit;
  int m_yLimit;
};

class Contours
{
public:
  using Contour = std::vector<Edge>;

  void addRect(const ScanRectangle &rect)
  {
    for (int i = 0; i < rect.pointCount(); ++i) {
      const Edge &edge = rect.edge(i);
      auto result = m_uniqueEdges.emplace(edge);
      if (!result.second) {
        m_uniqueEdges.erase(result.first);
      }
    }
  }

  std::vector<Contour> makeContours()
  {
    // Find connected components in an undirected graph by using depth-first search
    // algorithm.
    std::vector<Contour> contours;

    std::vector<Edge> edges(m_uniqueEdges.begin(), m_uniqueEdges.end());

    for (auto &e : edges) {
      if (!e.visited()) {
        contours.emplace_back();
        // Find all reachable edges from edge
        dfs(e, edges, contours.back());
      }
    }
    return contours;
  }

private:
  /// Implements the DFS algorithm
  void dfs(Edge &edge, std::vector<Edge> &edges, std::vector<Edge> &contour)
  {
    std::stack<Edge *> stack;
    stack.push(&edge);

    while (!stack.empty()) {
      auto e = stack.top();
      stack.pop();

      if (e->visited()) {
        continue;
      }

      // Mark the current edge as visited and save it
      e->setVisited();
      contour.emplace_back(*e);

      auto end = e->end();
      auto begin = e->begin();

      // Find all edges adjacent to this edge
      for (auto &currentEdge : edges) {
        if (!currentEdge.visited()) {
          auto cb = currentEdge.begin();
          auto ce = currentEdge.end();
          if (end   == cb || end   == ce ||
              begin == cb || begin == ce)
          {
            stack.push(&currentEdge);
          } else if (begin.minDistance(cb) > s_maxRectDimentinon &&
                     begin.minDistance(ce) > s_maxRectDimentinon &&
                     end.minDistance(cb)   > s_maxRectDimentinon &&
                     end.minDistance(ce)   > s_maxRectDimentinon) {
            // The edges are too far from each other. Doesn't make sense to
            // continue. This logic is valid, because the edges are sorted.
            break;
          }
        }
      }
    }
  }

  std::set<Edge> m_uniqueEdges;
};

////////////////////////////////////////////////////////////////////////////////

Result::Result(Result::Status diff, Result::Error error, const std::string &errorMessage)
  :
    m_status(diff),
    m_error(error),
    m_errorMessage(errorMessage),
    m_contourCount(0)
{}

Result::Status Result::status() const
{
  return m_status;
}

Result::Error Result::error() const
{
  return m_error;
}

const std::string &Result::errorMessage() const
{
  return m_errorMessage;
}

const Image &Result::resultImage() const
{
  return m_result;
}

void Result::setResultImage(const Image &image)
{
  m_result = image;
}

size_t Result::contourCount() const
{
  return m_contourCount;
}

void Result::setContourCount(size_t count)
{
  m_contourCount = count;
}

////////////////////////////////////////////////////////////////////////////////

Result Comparator::compare(const Image &image1, const Image &image2,
                           const Color &highlightColor)
{
  if (image1.isNull() || image2.isNull()) {
    return Result(Result::Status::Unknown, Result::Error::InvalidImage,
                  "Invalid image provided");
  }

  if (image1.width() != image2.width() || image1.height() != image2.height()) {
    return Result(Result::Status::Unknown, Result::Error::DifferentDimensions,
                  "Images have different dimensions");
  }

  Point origin{ 0, 0 }; // Start scanning from the upper left corner.
  ScanRectangle sr(origin, s_scanRectWidth, s_scanRectHeight, image1, image2);
  Contours contours;
  while (!sr.atEnd()) {
    if (!sr.test()) {
      contours.addRect(sr);
    }
    ++sr;
  }

  const auto &cont = contours.makeContours();

  if (cont.size() > 0) {
    Image output = image2;

    for (size_t i = 0; i < cont.size(); ++i) {
      const auto &contour = cont[i];
      for (const auto &edge : contour) {
        // Draw outline for each edge.
        output.drawLine(edge.begin(), edge.end(), highlightColor);
      }
    }

    Result result(Result::Status::Different, Result::Error::NoError);
    result.setResultImage(output);
    result.setContourCount(cont.size());
    return result;
  }

  return Result(Result::Status::Identical, Result::Error::NoError);
}

Result Comparator::compare(const std::string &file1, const std::string &file2)
{
  Image img1(file1);
  Image img2(file2);

  return compare(img1, img2);
}

}
