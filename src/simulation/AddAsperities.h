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

#include "VCSimulation.h"

#ifndef _ADD_ASPERITIES_H_
#define _ADD_ASPERITIES_H_

/*!
 A plugin which adds asperities to a given Virtual California model.
 The asperities are defined by a number of parameters:
 the number of asperities, their width, and their magnitude.
 Asperities are "created" by multiplying the stress drop of the selected
 blocks by the user specified magnitude.
 */
class AddAsperities : public SimPlugin {
public:
    virtual std::string name(void) const { return "Add asperities to faults"; }
	virtual void initDesc(const SimFramework *_sim) const;
	
    virtual void init(SimFramework *_sim);
};

#endif
