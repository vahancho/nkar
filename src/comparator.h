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

#ifndef _COMPARATOR_H_
#define _COMPARATOR_H_

#include <string>
#include "export.h"
#include "image.h"

namespace nkar
{

//! Implements image comparison result.
class NKAR_EXPORT Result
{
public:
  //! The comparison status.
  enum class Status
  {
    Unknown,   //! Comparison result unknown.
    Identical, //! Images are identical - no differences have been found.
    Different  //! Images are different.
  };

  enum class Error
  {
    NoError,            //! No error detected.
    InvalidImage,       //! Error in input images.
    DifferentDimensions //! Images have different dimensions.
  };

  //! Constructs a result object with the given \p status \p error and \p errorMessage if any.
  Result(Status status, Error error, const std::string &errorMessage = std::string());

  //! Returns the comparison status.
  Status status() const;

  //! Returns the comparison error.
  Error error() const;

  //! Returns error string or empty string on if there was no error.
  const std::string &errorMessage() const;

  //! Returns the resulting image with highlighted differences.
  const Image &resultImage() const;

  //! Sets the result image.
  void setResultImage(const Image &image);

  //! Returns the number of difference contours.
  size_t contourCount() const;

  //! Set the number of difference contours.
  void setContourCount(size_t count);

private:
  Status m_status;
  Error m_error;
  std::string m_errorMessage;
  Image m_result;
  size_t m_contourCount;
};

//! The class performs comparison of two images and outputs result of comparison.
class NKAR_EXPORT Comparator
{
public:
  //! Compares two image file and returns comparison result.
  static Result compare(const std::string &file1, const std::string &file2);

  //! Compares two images and returns comparison result.
  /*!
    \param image1 An actual image to compare
    \param image2 A baseline image to compare with. The diff outline will be drawn on this image
    \param highlightColor The color the diff outlines. Default color is red.
  */
  static Result compare(const Image &image1, const Image &image2,
                        const Color &highlightColor = {255, 0, 0});
};

}

#endif // _COMPARATOR_H_
