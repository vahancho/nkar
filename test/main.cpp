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

#include <string>
#include <cstdio>

#include "comparator.cpp"
#include "image.cpp"
#include "color.cpp"
#include "point.cpp"

enum Status
{
  Ok,
  Fail
};

bool test(const std::string &img1, const std::string img2, const std::string &tmpImg,
          const std::string &baseline)
{
  auto result = nkar::Comparator::compare(img1, img2);
  fprintf(stdout, "Images are different. %d contours found\n", result.contourCount());
  // Temporarily save the resulting image.
  result.resultImage().save(tmpImg);

  // Compare saved image with baseline - they should be identical.
  result = nkar::Comparator::compare(baseline, tmpImg);
  if (result.status() != nkar::Result::Status::Identical ||
      result.error()  != nkar::Result::Error::NoError) {
    fprintf(stderr, "Comparison of '%s' and '%s' failed\n", tmpImg.c_str(),
            baseline.c_str());
    return false;
  }

  // Remove temp image on success.
  std::remove(tmpImg.c_str());
  return true;
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    return Status::Fail;
  }

  // Color test.
  nkar::Color color;
  if (color.red() != 0 || color.green() != 0 || color.blue() != 0)
  {
    fprintf(stderr, "Incorrect default constructed color\n");
    return Status::Fail;
  }

  const std::string imagePath(argv[1]);
  const std::string tmpImg(imagePath + "tmp.png");

  {
    // Negative tests
    auto result = nkar::Comparator::compare(imagePath + "empty.png", "foo");
    fprintf(stdout, "Expected error: %s\n", result.errorMessage().c_str());
    if (result.error() != nkar::Result::Error::InvalidImage)
    {
      return Status::Fail;
    }
  }
  {
    // Negative tests
    auto result = nkar::Comparator::compare("foo", imagePath + "empty.png");
    fprintf(stdout, "Expected error: %s\n", result.errorMessage().c_str());
    if (result.error() != nkar::Result::Error::InvalidImage)
    {
      return Status::Fail;
    }
  }
  {
    // Negative tests
    auto result = nkar::Comparator::compare(imagePath + "lenna.png", imagePath + "empty.png");
    fprintf(stdout, "Expected error: %s\n", result.errorMessage().c_str());
    if (result.error() != nkar::Result::Error::DifferentDimensions)
    {
      return Status::Fail;
    }
  }

  // Images
  for (int i = 1; i < 14; ++i) {
    const std::string baseline(imagePath + std::to_string(i) + "_result.png");
    if (!test(imagePath + "empty.png", imagePath + std::to_string(i) + ".png",
              tmpImg, baseline))
    {
      return Status::Fail;
    }
  }

  // Lenna test
  if (!test(imagePath + "lenna.png", imagePath + "lenna_changed.png", tmpImg,
            imagePath + "lenna_result.png"))
  {
    return Status::Fail;
  }

  return Status::Ok;
}
