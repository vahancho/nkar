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
#include <assert.h>

#include "comparator.h"
#include "point.h"

namespace nkar
{

// Default width and height of the scan rectangle. Each of parameters should be
// greater or equal to one. Smaller values correspond to the preciser and slower calculations.
static const int scanRectWidth = 1;  // The highest precision
static const int scanRectHeight = 1; // The highest precision

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

private:
  Point m_begin;
  Point m_end;
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
    m_xLimit(std::min(img1.width(), img2.width())),
    m_yLimit(std::min(img1.height(), img2.height()))
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
    for (int c = m_origin.x(); c < cMax; c++) {
      for (int r = m_origin.y(); r < rMax; r++) {
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
      if (m_origin.x() < m_xLimit) {
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
  int m_xLimit;
  int m_yLimit;

  const Image &m_img1;
  const Image &m_img2;
};

class Contours
{
public:
  using Contour = std::vector<Edge>;

  void addRect(const ScanRectangle &rect)
  {
    for (int i = 0; i < rect.pointCount(); ++i) {
      const Edge &edge = rect.edge(i);
      auto result = m_edges.emplace(edge, false);
      if (!result.second) {
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
    for (auto &e : m_edges) {
      e.second = false;
    }

    for (const auto &e : m_edges) {
      if (!e.second) {
        contours.emplace_back();
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

    // Recur for all the edges adjacent to this edge
    for (const auto &e : m_edges) {
      if (!e.second) {
        const auto &currentEdge = e.first;
        if (edge.end() == currentEdge.begin() || edge.end() == currentEdge.end() ||
            edge.begin() == currentEdge.begin() || edge.begin() == currentEdge.end()) {
          dfs(currentEdge, contour);
        }
      }
    }
  }

  std::map<Edge, bool> m_edges;
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

int Result::contourCount() const
{
  return m_contourCount;
}

void Result::setContourCount(int count)
{
  m_contourCount = count;
}

////////////////////////////////////////////////////////////////////////////////

Result Comparator::compare(const Image &image1, const Image &image2)
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
  ScanRectangle sr(origin, scanRectWidth, scanRectHeight, image1, image2);
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

  const auto &cont = contours.contours();

  if (cont.size() > 0) {
    Image output = image2;

    for (size_t i = 0; i < cont.size(); ++i) {
      const auto &contour = cont[i];
      for (const auto &edge : contour) {
        // Draw red line for each edge.
        static const Color highlightColor{ 255, 0, 0 };
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
