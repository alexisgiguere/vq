// Copyright (c) 2012-2014 Eric M. Heien, Michael K. Sachs, John B. Rundle
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include "BASSAftershocks.h"
#include <algorithm>
#include <math.h>

void BASSAftershocks::initDesc(const SimFramework *_sim) const {
	const VCSimulation			*sim = static_cast<const VCSimulation*>(_sim);
	
	sim->console() << "# Creating BASS model aftershocks (max "
		<< sim->getBASSMaxGenerations() << " generations)." << std::endl;
}

SimRequest BASSAftershocks::run(SimFramework *_sim) {
	VCSimulation				*sim = static_cast<VCSimulation*>(_sim);
	unsigned int				genNum = 0, start, stop, count = 0;
	VCEventAftershock			initial_shock;
	VCGeneralEventSet::iterator	it;
	quakelib::Conversion		convert;
	
	// Get BASS parameters
	_Mm = sim->getBASSMinMagnitude();
	_dM = sim->getBASSAftershockStrength();
	_b = sim->getBASSFrequencyScale();
	_c = sim->getBASSAftershockStart();
	_p = sim->getBASSTimeDecay();
	_d = convert.m2km(sim->getBASSDistance());
	_q = sim->getBASSDistanceDecay();
	
	first = true;
	events_to_process = sim->getCurrentEvent().getAftershockPtr();
	
	// Generate initial seed shock based on the current event
	event_blocks.clear();
	sim->getCurrentEvent().getInvolvedBlocks(event_blocks);
	
	// Select a random block in the event to be the center of the aftershocks
	initial_shock = VCEventAftershock(sim->getCurrentEvent().getMagnitude(event_blocks),
									  sim->getCurrentEvent().getEventYear(),
									  0,		// No need to set x,y coords since they will be selected from entire fault
									  0,
									  0);
	events_to_process->push_back(initial_shock);
	
	// Also create aftershocks for Poisson generated events
	for (it=sim->getBGEvents().begin();it!=sim->getBGEvents().end();++it) {
		events_to_process->push_back(VCEventAftershock(it->mag, it->t, it->x, it->y, 0));
	}
													   
	start = 0;
	stop  = events_to_process->size();
	
	// Loop over N generations of aftershocks
	do {
		count = 0;
		
		for (unsigned int i=start; i<stop; ++i)
			count += generateAftershocks(sim, events_to_process->at(i));
		
		start = stop;
		stop  = events_to_process->size();
	} while ( (++genNum < sim->getBASSMaxGenerations()) && (count > 0) );
	
	// Remove the initial seed event (main shock) and Poisson background initial events
	events_to_process->erase(events_to_process->begin(), events_to_process->begin()+sim->getBGEvents().size()+1);
	
	// Finally we sort the events into time order since the aftershocks in the
	// BASS model are usually not generated in ordered time sequence.
	std::sort(events_to_process->begin(), events_to_process->end());
	
	return SIM_STOP_OK;
}

unsigned int BASSAftershocks::generateAftershocks(VCSimulation *sim, VCEventAftershock seed) {
	VCEventAftershock		aftershock;
	BlockIDSet::iterator	it;
	float					Ms, t, r, theta, seed_x, seed_y;
	int						selected_ind;
	
	Ms = seed.mag;
	// Calc number of aftershocks
	unsigned int N = (int)powf( 10 , _b*(Ms - _dM - _Mm) );
	
	// Loop over N aftershocks
	for (unsigned int i=0; i<N; i++) {
		// Calc aftershock generation number
		aftershock.gen = seed.gen + 1;
		
		// Calc aftershock mag
		aftershock.mag = _Mm - log10(sim->randFloat())/_b;
		// No aftershocks bigger than the seeds
		if (aftershock.mag >= seed.mag) {
			i--;
			continue;
		}
		
		// Calc aftershock time
		t = _c*powf(sim->randFloat(), -1.0/(_p-1.0)) - _c;
		aftershock.t = seed.t + t;
		
		// Calc aftershock position
		// old method
		//r = _d*powf(10,0.5*Ms)*powf(sim->randFloat() , -1.0/(_q-1.0)) - _d*powf(10,0.5*Ms);
		// new method
		r = _d*powf(sim->randFloat(), -1.0/(_q-1.0)) - _d;
		
		theta = 2.0 * M_PI * sim->randFloat();
		// If we're generating daughter events from the initial seed, use a center point
		// randomly selected from along the entire set of ruptured blocks
		if (first) {
			selected_ind = sim->randInt(event_blocks.size());
			it = event_blocks.begin();
			advance(it, selected_ind);
			seed_x = sim->getBlock(*it).center()[0];
			seed_y = sim->getBlock(*it).center()[1];
		} else {
			seed_x = seed.x;
			seed_y = seed.y;
		}
		aftershock.x = seed_x + r*cos(theta);
		aftershock.y = seed_y + r*sin(theta);
		
		// Add the aftershock to our list
		events_to_process->push_back(aftershock);
	}
	
	first = false;
	
	return N;
}

#undef RAND
