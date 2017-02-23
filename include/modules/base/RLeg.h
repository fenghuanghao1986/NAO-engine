/*
Copyright (c) 2017 "University of Denver"
This file is part of Pineapple.
Pineapple is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
VERSION HISTORY
-- Created by Paul Heinen 2/2/17
// FILE DESCRIPTION
Derived Right Leg class
*/

//INCLUDES
#pragma once
#include "Leg.h"
class RLeg :
	public Leg
{
public:
	RLeg();
	~RLeg();
	void set_hip(double roll, double yaw, double pitch);
	void set_knee(double pitch);
	void set_ankle(double roll, double pitch);
	BoundList get_actuator_bounds();
private:
	std::unique_ptr<Hip> RHip;
	std::unique_ptr<Knee> RKnee;
	std::unique_ptr<Ankle> RAnkle;
};

