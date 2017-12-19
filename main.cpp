#include <string>
#include <memory>
#include <algorithm>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

private:
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

  const unsigned char *data() const
  {
    return m_data;
  }

  Color pixel(int row, int column) const
  {
    assert(row < m_height && column < m_width);

    auto pos = row * m_width + column;

    auto red   = m_data[pos * STBI_rgb];
    auto green = m_data[pos * STBI_rgb + 1];
    auto blue  = m_data[pos * STBI_rgb + 2];

    return { red, green, blue };
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
  ScanRectangle(int x, int y, int width, int height,
                std::shared_ptr<Image> img1,
                std::shared_ptr<Image> img2)
    :
      m_x(x),
      m_y(y),
      m_width(width),
      m_height(height),
      m_img1(img1),
      m_img2(img2),
      m_xLimit(std::min(img1->width(), img2->width())),
      m_yLimit(std::min(img1->height(), img2->height()))
  {}

  bool test() const
  {
    const int cMax = std::min(m_x + m_width, m_xLimit);
    const int rMax = std::min(m_y + m_height, m_yLimit);
    for (int c = m_x; c < cMax; c++)
    {
      for (int r = m_y; r < rMax; r++)
      {
        Color color1 = m_img1->pixel(r, c);
        Color color2 = m_img2->pixel(r, c);

        if (color1 != color2)
        {
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
    return m_x == 0 &&
           m_y >= m_yLimit;
  }

  //! A pre-increment operator.
  ScanRectangle &operator++()
  {
    if (!atEnd())
    {
      if (m_x < m_xLimit)
      {
        m_x += m_width;
      }
      else if (m_y < m_yLimit)
      {
        // Move to the next row and left most position.
        m_x = 0;
        m_y += m_height;
      }
    }

    return *this;
  }

private:
  int m_x;
  int m_y;
  int m_width;
  int m_height;
  int m_xLimit;
  int m_yLimit;

  std::shared_ptr<Image> m_img1;
  std::shared_ptr<Image> m_img2;
};

int main(int argc, char **argv)
{
  printf("run...\n");

  std::string file1("d:/test1.png");
  std::string file2("d:/test2.png");

  std::shared_ptr<Image> img1 = std::make_shared<Image>(file1);
  std::shared_ptr<Image> img2 = std::make_shared<Image>(file2);

  if (!img1->open())
  {
    return 1;
  }

  if (!img2->open())
  {
    return 1;
  }

  if (img1->width() != img2->width() || img1->height() != img2->height())
  {
    fprintf(stderr, "Images have different dimensions\n");
    return 1;
  }

  ScanRectangle sr(0, 0, 5, 5, img1, img2);
  std::vector<ScanRectangle> failedRects;
  while (!sr.atEnd())
  {
    if (!sr.test())
    {
      failedRects.emplace_back(sr);
    }
    ++sr;
  }

  printf("end\n");
  return 0;
}