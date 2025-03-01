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
// $Maintainer: Chris Bielow $
// $Authors: Chris Bielow $
// --------------------------------------------------------------------------

/**
  @page TOPP_SwathWizard SwathWizard

  @brief An assistant for Swath analysis.

  The Wizard takes the user through the whole analysis pipeline for SWATH proteomics data analysis, 
  i.e. the @ref UTILS_OpenSwathWorkflow tool, including downstream tools such
  as <a href="https://github.com/PyProphet/pyprophet" target="_blank">pyProphet</a> and
  the <a href="https://github.com/msproteomicstools/msproteomicstools" target="_blank">TRIC alignment</a> tool.
  
  Since the downstream tools require Python and the respective modules, the Wizard will check their proper
  installation status and warn the user if a component is missing.

  Users can enter the required input data (mzML MS/MS data, configuration files) in dedicated fields, usually by drag'n'droping files from the
  operating systems' file explorer (Explorer, Nautilus, Finder...).
  The output of the Wizard is both the intermediate files from OpenSWATH (e.g. the XIC data in .sqMass format) and the 
  tab-separated table format (.tsv) from pyProphet and TRIC.
  
  This is how the wizard looks like:
  @image html SwathWizard.png


  Schematic of the internal data flow (all tools are called by SwathWizard in the background):
  @dot
    digraph wizard_workflow
    {
      node [ style="solid,filled", color=black, fillcolor=grey60, width=1.0, fixedsize=false, shape=square, fontname=Helvetica ];
      edge [ arrowhead="open", style="solid" ];

      rankdir="LR";
      splines=ortho;

      subgraph cluster_SwathWizard {
        fontsize=26;
        style=filled;
        color=grey90;
        topp_ows -> osw [ label="out_osw" ];
        topp_ows -> sqMass [ label="out_chrom" ];
        osw -> pyProphet;
        pyProphet -> osw_inter;
        osw_inter -> pyProphet;
        pyProphet -> tsv;
        tsv -> TRIC;
        label = "SwathWizard";
      }


      mzml [ label="Swath mzML file(s)\n(.mzML | XML)" shape=oval fillcolor=white group=1];
      iRTlib [ label="iRT library file\n(.pqp | sqlLite)" shape=oval fillcolor=white group=1];
      transitionLib [ label="Swath library file\n(.pqp | sqlLite)" shape=oval fillcolor=white group=1];
      topp_ows [ label="OpenSwath-\nWorkflow" URL="\ref OpenMS::OpenSwathWorkflow" group=1];
      osw [ label="ID file(s)\n(.osw | sqlLite)" shape=oval fillcolor=white group=1];
      sqMass [ label="Raw XIC data file(s)\n(.sqMass | sqlLite)" shape=oval fillcolor=white group=1];
      mzml -> topp_ows [ xlabel = "in"];
      iRTlib -> topp_ows[ xlabel = "tr_irt"];
      transitionLib -> topp_ows[ xlabel = "tr"];


      pyProphet [ label="pyProphet (FDR)" URL="https://github.com/PyProphet/pyprophet" group=2];
      osw_inter [ label="annotated ID file(s)\n(.osw | sqlLite)" shape=oval fillcolor=white group=2];
      tsv [ label="ID file(s)\n(.tsv | text)" shape=oval fillcolor=white group=2];

      TRIC [ label="TRIC\n(TRansfer of\nIdentification\nConfidence)" URL="https://github.com/msproteomicstools/msproteomicstools/blob/master/TRIC-README.md" group=3];
      tsv_aligned [ label="Aligned IDs file\n(.tsv | text)" shape=oval fillcolor=white group=3];
      tsv_aligned_matrix [ label="Aligned Matrix IDs file\n(.tsv | text)" shape=oval fillcolor=white group=3];
      TRIC -> tsv_aligned;
      TRIC -> tsv_aligned_matrix;

      topp_tv [ label = "TOPPView", group=4]
      osw_inter -> topp_tv;
      sqMass -> topp_tv;
    }
  @enddot


  A recommended test data for the Wizard is the
  <a href="https://db.systemsbiology.net/sbeams/cgi/PeptideAtlas/PASS_View?identifier=PASS00779" target="_blank">PASS00779</a>
  dataset.

*/

//QT
#include <QApplication>
#include <QtWidgets/QSplashScreen>


//OpenMS
#include <OpenMS/CONCEPT/LogStream.h>
#include <OpenMS/CONCEPT/VersionInfo.h>
#include <OpenMS/DATASTRUCTURES/Map.h>
#include <OpenMS/VISUAL/APPLICATIONS/SwathWizardBase.h>
#include <OpenMS/VISUAL/APPLICATIONS/MISC/QApplicationTOPP.h>

//STL
#include <iostream>
#include <map>
#include <vector>

#ifdef OPENMS_WINDOWSPLATFORM
#   ifndef _WIN32_WINNT
#       define _WIN32_WINNT 0x0501 // Win XP (and above)
#   endif
#   include <Windows.h>
#endif

using namespace OpenMS;
using namespace std;

//-------------------------------------------------------------
// command line name of this tool
//-------------------------------------------------------------
const char* tool_name = "SwathWizard";

//-------------------------------------------------------------
// description of the usage of this TOPP tool
//-------------------------------------------------------------

void print_usage(Logger::LogStream& stream = OpenMS_Log_info)
{
  stream << "\n"
         << tool_name << " -- An assistant for Swath-Analysis." << "\n"
         << "\n"
         << "Usage:" << "\n"
         << " " << tool_name << " [options] [files]" << "\n"
         << "\n"
         << "Options are:" << "\n"
         << "  --help           Shows this help" << "\n"
         << "  --debug          Enables debug messages\n"
         << "  -ini <File>      Sets the INI file (default: ~/.SwathWizard.ini)" << "\n"
         << endl;
}

int main(int argc, const char** argv)
{
  // list of all the valid options
  std::map<std::string, std::string> valid_options, valid_flags, option_lists;
  valid_flags["--help"] = "help";
  valid_flags["--debug"] = "debug";
  valid_options["-ini"] = "ini";

  Param param;
  param.parseCommandLine(argc, argv, valid_options, valid_flags, option_lists);

  // '--help' given
  if (param.exists("help"))
  {
    print_usage();
    return 0;
  }

  // '-debug' given
  if (param.exists("debug"))
  {
    OPENMS_LOG_INFO << "Debug flag provided. Enabling 'OPENMS_LOG_DEBUG' ..." << std::endl;
    OpenMS_Log_debug.insert(cout); // allows to use OPENMS_LOG_DEBUG << "something" << std::endl;
  }

  // test if unknown options were given
  if (param.exists("unknown"))
  {
    // if packed as Mac OS X bundle it will get a -psn_.. parameter by default from the OS
    // if this is the only unknown option it will be ignored .. maybe this should be solved directly
    // in Param.h
    if (!(String(param.getValue("unknown").toString()).hasSubstring("-psn") && !String(param.getValue("unknown").toString()).hasSubstring(", ")))
    {
      OPENMS_LOG_ERROR << "Unknown option(s) '" << param.getValue("unknown").toString() << "' given. Aborting!" << endl;
      print_usage(OpenMS_Log_error);
      return 1;
    }
  }

  QApplicationTOPP a(argc, const_cast<char**>(argv));
  a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));

  SwathWizardBase mw;
  mw.show();

  if (param.exists("ini"))
  {
    //mw.loadPreferences((String)param.getValue("ini"));
  }

#ifdef OPENMS_WINDOWSPLATFORM
  FreeConsole(); // get rid of console window at this point (we will not see any console output from this point on)
  AttachConsole(-1); // if the parent is a console, reattach to it - so we can see debug output - a normal user will usually not use cmd.exe to start a GUI)
#endif

  int result = a.exec();

  return result;
}
