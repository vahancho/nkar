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
#include <chrono>
#include <iostream>

#include "comparator.h"
#include "image.h"

enum Status
{
  Ok,
  ComparisonError,
  Difference,
  IncorrectOptions
};

static void printUsage()
{
  printf("Usage: comparator file1 file2 output_file\n");
}

int main(int argc, char **argv)
{
  if (argc != 4)
  {
    printUsage();
    return IncorrectOptions;
  }

  auto start = std::chrono::high_resolution_clock::now();
  auto result = nkar::Comparator::compare(argv[1], argv[2]);
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "Comparison duration: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms. Contours found: " << result.contourCount() << '\n';

  if (result.error() != nkar::Result::Error::NoError)
  {
    fprintf(stderr, "%s\n", result.errorMessage().c_str());
    return Status::ComparisonError;
  }

  if (result.status() == nkar::Result::Status::Different)
  {
    if (result.resultImage().save(argv[3]))
    {
      return Status::Difference;
    } else {
      fprintf(stdout, "Failed to save result image\n");
      return Status::ComparisonError;
    }
  }

  fprintf(stdout, "Images are identical\n");
  return Status::Ok;
}
