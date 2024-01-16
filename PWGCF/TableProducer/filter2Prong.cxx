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

	Produces<aod::CF2ProngTracks> output2ProngTracks;

	void init(InitContext& context){
		LOGF(info,"FilterCF2Prong initialized\n");
		printf("FilterCF2Prong initialized\n");
	}

	using HFCandidates = soa::Join<aod::HfCand2Prong, aod::HfSelD0>;
	//void processData(aod::Collisions::iterator const& collision, soa::Join<aod::CFCollisions, aod::CFCollRefs> const& cfcollisions, aod::BCsWithTimestamps const&, soa::Join<aod::Tracks, aod::TrackSelection> const& tracks, soa::Join<aod::CFTracks, aod::CFTrackRefs> const& cftracks, HFCandidates const& candidates){ //<-- ok, works, but no cftracks
	//void processData(soa::Join<aod::CFCollisions, aod::CFCollRefs>::iterator const& cfcollision, aod::BCsWithTimestamps const&, soa::Join<aod::Tracks, aod::TrackSelection> const& tracks, soa::Join<aod::CFTracks, aod::CFTrackRefs> const& cftracks, HFCandidates const& candidates){
	//void processData(aod::Collisions::iterator const& collision, aod::BCsWithTimestamps const&, soa::Join<aod::Tracks, aod::TrackSelection> const& tracks, soa::Join<aod::CFTracks, aod::CFTrackRefs> const& cftracks, HFCandidates const& candidates){ //<--- ok, works, but no cftracks
	//------
	//void processData(aod::Collisions::iterator const& collision, aod::BCsWithTimestamps const&, soa::Join<aod::CFCollRefs, aod::CFCollisions> const& cfcollisions, soa::Join<aod::CFTrackRefs, aod::CFTracks> const& cftracks, soa::Join<aod::Tracks, aod::TrackSelection> const& tracks, HFCandidates const& candidates){
	void processData(aod::Collisions::iterator const& collision, aod::BCsWithTimestamps const&, aod::CFCollRefs const& cfcollisions, soa::Join<aod::CFTrackRefs, aod::CFTracks> const& cftracks, HFCandidates const& candidates){
	//void processData(soa::Join<aod::CFCollRefs, aod::CFCollisions> const& cfcollisions, soa::Join<aod::CFTrackRefs, aod::CFTracks> const& cftracks, soa::Join<aod::Tracks, aod::TrackSelection> const& tracks, HFCandidates const& candidates){ //try Preslice the candidates with cfcollision.CollRef.globalIndex
	//Preslice<HFCandidates> perCollision = aod::hf_cand::collisionId;
	//void processData(soa::Join<aod::CFCollRefs, aod::CFCollisions> const& cfcollisions, soa::Join<aod::CFTrackRefs, aod::CFTracks> const& cftracks, soa::Join<aod::Tracks, aod::TrackSelection> const& tracks, HFCandidates const& candidates){ //try Preslice the candidates with cfcollision.CollRef.globalIndex
	//------
		/*auto collId = collision.cfCollision_as<aod::CFCollRefs>().cfCollisionId();
		if(collId < 0){
			LOGF(info,"rejecting collision\n");
			return;
		}*/
		/*for(auto &c : cfcollisions){
			auto cs = candidates.sliceBy(perCollision,c.collision_as<aod::CFCollRefs>().collisionId());
			printf("c: candidates: %u\n",cs.size());
		}*/
		if(cftracks.size() <= 0)
			return;
		if (cfgVerbosity > 0) {
			if(candidates.size() > 0)
				//LOGF(info, "processData2Prong: Candidates for collision: %u, cfcolls: %u, Tracks: %u, CFTracks: %u\n", candidates.size(),cfcollisions.size(),tracks.size(),cftracks.size());
				//LOGF(info, "processData2Prong: Candidates for collision: %u, cfcollisions: %u, Tracks: %u, CFTracks: %u\n", candidates.size(),cfcollisions.size(),tracks.size(),cftracks.size());
				LOGF(info, "processData2Prong: Candidates for collision: %u, cfcollisions: %u, CFTracks: %u\n", candidates.size(),cfcollisions.size(),cftracks.size());
		}
		/*int prongCFId[2] = {-1,-1};
		for(auto &track : tracks){
			for(auto &c : candidates)
				if(c.prong0Id() == track.index()){
					//prongCFId[0] = track.cfTrack.index();
					//track.cfTrack().index();
					//track.cfTrackId();
					//track.cfTrack_as<aod::CFTrackRefs>().index();
					prongCFId[0] = track.cfTrack_as<aod::CFTrackRefs>().cfTrackId();
					//track.template cfTrack_as<aod::CFTrackRefs>().cfTrackId();
					//track.cfTrack().index();
					break;
				}
			for(auto &c : candidates)
				if(c.prong1Id() == track.index()){
					prongCFId[1] = track.cfTrack_as<aod::CFTrackRefs>().cfTrackId();
					break;
				}
		}
		for (auto& c : candidates) {
			if ((c.hfflag() & 1 << aod::hf_cand_2prong::DecayType::D0ToPiK) == 0) // TODO <--- make configurable
				continue;
			output2ProngTracks(collision.cfCollision_as<aod::CFCollRefs>().cfCollisionId(),
				 prongCFId[0], prongCFId[1], c.pt(), c.eta(), c.phi(), 0u); //<-- selection mask
		 }*/
	}
	PROCESS_SWITCH(FilterCF2Prong, processData, "Process data", true);
}; //struct

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<FilterCF2Prong>(cfgc)};
}

