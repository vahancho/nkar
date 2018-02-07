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

#include "comparator.h"
#include "image.h"

enum Status
{
  Ok,
  ComparisonError,
  Difference
};

int main(int argc, char **argv)
{
  auto result = nkar::Comparator::compare("d:/test1.png", "d:/test2.png");

  if (result.error() != nkar::Result::Error::NoError)
  {
    fprintf(stderr, "%s\n", result.errorMessage().c_str());
    return Status::ComparisonError;
  }

  if (result.status() == nkar::Result::Status::Different)
  {
    if (result.resultImage().save("d:/res.png")) {
      return Status::Difference;
    } else {
      fprintf(stdout, "Failed to save result image\n");
      return Status::ComparisonError;
    }
  }

  fprintf(stdout, "Images are identical\n");
  return Status::Ok;
}
