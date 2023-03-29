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

#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>

#include "comparator.h"
#include "point.h"

enum Status
{
  Ok,
  Fail
};

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)
#define LOCATION std::string(__FILE__ ":" STRINGIFY(__LINE__))
#define TEST(EXP) \
    if (!(EXP)) {\
        std::cerr << "FAIL: " << #EXP << " at: " << LOCATION << '\n'; \
        return Status::Fail; \
    }

bool test(const std::string &img1, const std::string img2, const std::string &tmpImg,
          const std::string &baseline)
{
  auto start = std::chrono::high_resolution_clock::now();
  auto result = nkar::Comparator::compare(img1, img2);
  auto end = std::chrono::high_resolution_clock::now();

  std::cout << "comparison duration: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms.\n";

  std::cout << "Images are different. " << result.contourCount() << " contours found\n";
  // Temporarily save the resulting image.
  result.resultImage().save(tmpImg);

  // Compare saved image with baseline - they should be identical.
  result = nkar::Comparator::compare(baseline, tmpImg);
  if (result.status() != nkar::Result::Status::Identical ||
      result.error()  != nkar::Result::Error::NoError) {
    std::cerr << "Comparison of '" << tmpImg << "' and '" << baseline << "' failed\n";
    return false;
  }

  // Remove temp image on success.
  std::remove(tmpImg.c_str());
  return true;
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    std::cerr << "Incorrect number of parameters. Expected the path to the directory with images\n";
    return Status::Fail;
  }

  // Color test.
  nkar::Color color;
  if (color.red() != 0 || color.green() != 0 || color.blue() != 0)
  {
    std::cerr << "Incorrect default constructed color\n";
    return Status::Fail;
  }

  const std::string imagePath(argv[1]);
  const std::string tmpImg(imagePath + "/tmp.png");
  {
    // Negative tests
    auto result = nkar::Comparator::compare(imagePath + "/empty.png", "foo");
    std::cout << "Expected error: " << result.errorMessage() << '\n';
    TEST(result.error() == nkar::Result::Error::InvalidImage);
  }
  {
    // Negative tests
    auto result = nkar::Comparator::compare("foo", imagePath + "/empty.png");
    std::cout << "Expected error: " << result.errorMessage() << '\n';
    TEST(result.error() == nkar::Result::Error::InvalidImage);
  }
  {
    // Negative tests
    auto result = nkar::Comparator::compare(imagePath + "/lenna.png", imagePath + "/empty.png");
    std::cout << "Expected error: " << result.errorMessage() << '\n';
    TEST(result.error() == nkar::Result::Error::DifferentDimensions);
  }

  // Images
  for (int i = 1; i < 19; ++i) {
    const std::string baseline(imagePath + "/" + std::to_string(i) + "_result.png");
    TEST(test(imagePath + "/empty.png", imagePath + "/" + std::to_string(i) + ".png",
               tmpImg, baseline));
  }

  // Lenna test
  TEST(test(imagePath + "/lenna.png", imagePath + "/lenna_changed.png", tmpImg,
             imagePath + "/lenna_result.png"));

  // Maps
  TEST(test(imagePath + "/map1.png", imagePath + "/map2.png", tmpImg,
             imagePath + "/map_result.png"));

  // Large
  TEST(test(imagePath + "/large.png", imagePath + "/empty_large.png", tmpImg,
            imagePath + "/large_result.png"));

  // Test the nkar::Point
  nkar::Point p0{0, 0};
  nkar::Point p1{1, 0};
  nkar::Point p2{0, 1};
  nkar::Point p3{1, 1};
  nkar::Point p4{0, 0};

  TEST(p0 < p1);
  TEST(p0 < p2);
  TEST(p0 < p3);
  TEST(!(p0 < p4));
  TEST(p2 < p1);

  // Test nkar::Image
  nkar::Image image;
  TEST(image.pixel(0, 0).red() == 0);
  TEST(image.pixel(0, 0).green() == 0);
  TEST(image.pixel(0, 0).blue() == 0);

  image.drawLine({}, {}, {});
  TEST(image.save("foo") == false);

  return Status::Ok;
}
