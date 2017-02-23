
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
Pineapple module for communication with the NAO hardware. This module does not directly set/read values
from the NAO hardware, instead it communicates via shared memory with a seperate module that uses
Aldebaran's API for hardware communication. The advantage of keeping the processes seperate is that it allows
us to optimize how we communicate with the hardware, and it allows us to create our own API. Another advantage
is that we can simulate modules communicating with the NAO hardware, so for development purposes people won't even
need to physically work with a NAO robot.
*/

#ifndef NAO_INTERFACE_H
#define NAO_INTERFACE_H

//INCLUDES

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <boost/any.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp> //Implemented in STL in C++17 :(
#define BOOST_BIND_NO_PLACEHOLDERS
#include "Head.h"
#include "RArm.h"
#include "LArm.h"
#include "LLeg.h"
#include "RLeg.h"
#include "include/common.h"
#include "include/debug/debugging.h"


/**
  * @class NAOInterface
  * Main class/module class for the NAOInterface. A number of other classes
  * are what are really doing most of the work. 
  **/
class NAOInterface : public Module
{
public:

	/*
	* hardware_datatypes - boost::variant datatype that can be set to a variety of different datatypes.
	**/
	typedef boost::variant < std::pair<double, double>, double, std::string, std::unordered_map<std::string, Head::EyeLED> > hardware_datatypes;
	void parse_intent(const std::string &input);
	void initialize_hardware_map(); ///< Initially sets all of the values of our absurd hashmap.
	void initialize_function_map(); ///< Initializes a map for calling functions.
	
	//Debug Functions
	void sanity_test(const std::string &foo); ///< Simple method for testing lookups and value sets
	void print_commands_list(); ///< Lists all available function calls
	void randomly_set_joints(); ///< This function randomly sets all of the joint values of the NAO's hardware.
	void print_hardware_map(); ///< Will probably be deprecated


	double generate_random_bound_val(double min, double max); ///< Generates a random number between a lower and upper limit
	double generate_random_bound_val(std::pair<double, double> bounds); ///< Generates a random value given a actuators bounds (in radians, don't use degrees).
	std::vector<std::string> command_list; ///< Stores the commands. 
	/**
	  * @brief Writes a new value to a hardware component.
	  * @param hardware_component - The hardware component's name to be written to.
	  * @param values - A vector containing list of values to be set.
	  * @return - boolean which is true if operation was succesful or false if a failure occured.
	  *
	  * execute_intent_write first checks if the given harware component string is valid
	  * and a real hardware component, then updates the joint_and_sensor_data map.
	  * After all values have been written to the local map of values, we send that data
	  * to the Shared Memory Map using the synch_pineapple() method.
	  */
	bool set_hardware_value(const std::string &hardware_component, double  value); 
	/**
	  * @brief Reads requested hardware map values and returns the requested value.]
	  * @param request_module - Name of the the module that requested the values
	  * @param hardware_component - The hardware component's name to be read from.
	  * @param requested_value - The hardware value(s) to be sent back to the module
	  * @return - boolean which is true if operation was succesful or false if a failure occured.
	  */
	bool execute_intent_read(std::string &request_module, std::string &hardware_component, std::string &requested_value);
	//auto get_hardware_value(std::string &args);
	/** @brief - Synchronizes hardware data values with the module running in NAOqi
	  * @return - boolean which is true if operation was succesful or false if a failure occured.
	  *
	  * This method is the most important of NAOInterface, it synchronizes data by reading and writing 
	  * from/to Shared Memory. There are safety mechanisims built in (namely a semaphore) to prevent
	  * data corruption from occuring when both this module and the libpineapple module running in NAOqi
	  * attempt to both read/write to the shared memory. 
	  *
	  * This method is called after all of the write intents occur and before any read intent occurs to 
	  * ensure that both processes have the most up to date hardware values.
	  */
	bool sync_pineapple(); 
	typedef std::function<hardware_datatypes(hardware_datatypes)> generic_function;
	generic_function f;
	/*typedef std::map<std::string, generic_function> function_map;*/
	
	/**
	  * shared_ptr's for Ligaments: Due to these being shared pointers, be careful with Ref Counts and how destruction is handled (it's not automatic).
	  * Need to be shared_ptrs for function pointers and binding.
	  **/
	static NAOInterface* Instance();
    void Reconfigure(std::string config_file, uint16_t id);
    bool RunFrame();
    bool ProcessIntent(Intent &i);
    bool Install();
    bool Uninstall();
	std::shared_ptr<Head> head;
	std::shared_ptr<RArm> RightArm;
	std::shared_ptr<LArm> LeftArm;
	std::shared_ptr<LLeg> LeftLeg;
	std::shared_ptr<RLeg> RightLeg;
private:
    static NAOInterface* instance;
	NAOInterface();

    //Intent processing
    std::deque<Intent> pendingIntents;

protected: 
	std::unordered_map<std::string, hardware_datatypes> joint_and_sensor_data; ///< Clusterf*** of a hashmap containing all of the hardware values. Subject to change...
	std::unordered_map<std::string, std::function<void(double)>> hardware_set_functions; ///< unordered_map of API calls and value set function pointers
	std::unordered_map<std::string, std::function<double(void)>> hardware_get_map;///< unordered_map of API calls and value get function pointers
};

#endif //NAOInterface_H