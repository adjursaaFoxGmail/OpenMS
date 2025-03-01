// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2020.
//
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS.
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Timo Sachsenberg $
// $Authors: Marc Sturm $
// --------------------------------------------------------------------------


#pragma once

#include <OpenMS/CONCEPT/Types.h>

#include <QtWidgets/QDialog>

namespace Ui
{
  class SpectrumAlignmentDialogTemplate;
}

namespace OpenMS
{
  class Plot1DWidget;

  /**
      @brief Lets the user select two spectra and set the parameters for the spectrum alignment.

      @ingroup Dialogs
  */
  class SpectrumAlignmentDialog :
    public QDialog
  {
    Q_OBJECT

public:

    /// Constructor
    SpectrumAlignmentDialog(Plot1DWidget * parent);
    ~SpectrumAlignmentDialog();
  
    double getTolerance() const;
    bool isPPM() const;

    /// Returns the index of the selected non-flipped layer
    Int get1stLayerIndex();
    /// Returns the index of the selected flipped layer
    Int get2ndLayerIndex();

protected slots:

protected:

    /// Stores the layer indices of the layers in the left list (non-flipped layers)
    std::vector<UInt> layer_indices_1_;
    /// Stores the layer indices of the layers in the right list (flipped layers)
    std::vector<UInt> layer_indices_2_;

private:
    Ui::SpectrumAlignmentDialogTemplate* ui_;
  };

}
