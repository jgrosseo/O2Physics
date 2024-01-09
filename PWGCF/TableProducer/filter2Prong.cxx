// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/ASoAHelpers.h"
#include "Framework/O2DatabasePDGPlugin.h"

#include "MathUtils/detail/TypeTruncation.h"

#include "PWGCF/DataModel/CorrelationsDerived.h"
#include "Common/DataModel/EventSelection.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "Common/DataModel/Centrality.h"

#include "PWGHF/Core/HfHelper.h"
#include "PWGHF/DataModel/CandidateReconstructionTables.h"
#include "PWGHF/DataModel/CandidateSelectionTables.h"

#include <TH3F.h>
#include <TDatabasePDG.h>

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;
using namespace o2::math_utils::detail;

#define FLOAT_PRECISION 0xFFFFFFF0
#define O2_DEFINE_CONFIGURABLE(NAME, TYPE, DEFAULT, HELP) Configurable<TYPE> NAME{#NAME, DEFAULT, HELP};

struct FilterCF2Prong{
  O2_DEFINE_CONFIGURABLE(cfgVerbosity, int, 1, "Verbosity level (0 = major, 1 = per collision)")

  using HFCandidates = soa::Join<aod::HfCand2Prong, aod::HfSelD0>;
  void processData(soa::Join<aod::Collisions, aod::CFCollRefs>::iterator const& collision, aod::BCsWithTimestamps const&, soa::Join<aod::Tracks, aod::TrackSelection, aod::CFTrackRefs> const& tracks, HFCandidates const& candidates){
	if (cfgVerbosity > 0) {
      LOGF(info, "processData2Prong: Candidates for collision: %u", candidates.size());
    }
  }
  int prongCFId[2] = {-1,-1};
  for(auto &c : candidates)
  	if(c.prong0Id() == tracks.index()){
		prongCFId[0] = tracks.cfTrack();
		break;
	}
  for(auto &c : candidates)
  	if(c.prong1Id() == tracks.index()){
		prongCFId[1] = tracks.cfTrack();
		break;
	}
  PROCESS_SWITCH(FilterCF2Prong, processData, "Process data", true);
}

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<FilterCF2Prong>(cfgc)};
}

