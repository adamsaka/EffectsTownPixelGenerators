/********************************************************************************************************

Authors:		(c) 2022 Maths Town

Licence:		The GNU General Public License Version 3

*********************************************************************************************************
This program is free software: you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************

Description:

	Tools to build a host independant parameter list.

*********************************************************************************************************/
#pragma once

//Project Specific Includes
#include "parameter-id.h"

#include <string>
#include <vector>
#include <exception>

/*********************************************************************************************************
Type of Parameters
*********************************************************************************************************/
enum class ParameterType {
	seed,
	number,
	percent,
	angle,
	list,
	check,
	group_start,
	group_end,
	
};

/*********************************************************************************************************
A Parameter Entry
*********************************************************************************************************/
struct ParameterEntry {
	ParameterID id{};
	std::string name{ };
	ParameterType type {};
	double initial_value{};
	std::string contents{};
	double min{};
	double max{};
	double slider_min{};
	double slider_max{};
	short precision{};
	double value{};
	std::string value_string {};

	static ParameterEntry make_seed(ParameterID parameter_id, std::string parameter_name) {
		ParameterEntry p{};
		p.id = parameter_id;
		p.name = parameter_name;
		p.type = ParameterType::seed;
		p.initial_value = 0.0;
		p.min = 0.0;
		p.max = static_cast<double>(std::numeric_limits<uint64_t>::max());
		p.slider_min = p.min;
		p.slider_max = p.max;
		p.value = p.initial_value;
		return p;
	}

	static ParameterEntry make_number(ParameterID parameter_id, std::string parameter_name, double minimum = 0.0, double maximum = 1000.0, double default_value = 0.0, double slider_minimum = 0.0, double slider_maximum = 1000.00, short decimals=2) {
		ParameterEntry p{};
		p.id = parameter_id;
		p.name = parameter_name;
		p.type = ParameterType::number;
		p.initial_value = default_value;
		p.min = minimum;
		p.max = maximum;
		p.slider_min = slider_minimum;
		p.slider_max = slider_maximum;
		p.precision = decimals;
		p.value = p.initial_value;
		return p;
	}	

};


/*********************************************************************************************************
A List of parameters.
Simply a std::vector wrapped in a struct with helper functions.
*********************************************************************************************************/
struct ParameterList {
	std::vector<ParameterEntry> entries{};

	//Add a new entry
	void add_entry(ParameterEntry entry) {
		entries.push_back(entry);
	}

	//Get a value (as a double float)
	bool contains(ParameterID id) const {
		for (const auto& e : entries) {
			if (id == e.id) return true;
		}
		return false;
	}

	//Get a value (as a double float)
	double get_value(ParameterID id) const  {
		for (const auto & e : entries) {
			if (id == e.id) return e.value;
		}
		return 0.0;
	}

	//Get a value (as a string)
	std::string get_string(ParameterID id) const {
		for (const auto & e : entries) {
			if (id == e.id) return e.value_string;
		}
		return std::string();
	}

	//Set a numerical value
	double set_value(ParameterID id, double v) {
		for (auto & e : entries) {
			if (id == e.id) e.value = v;
		}
		throw(std::exception("Parameter not found."));
	}

	//Set a string value
	double set_value_string(ParameterID id, std::string v) {
		for (auto & e : entries) {
			if (id == e.id) e.value_string = v;
		}
		throw(std::exception("Parameter not found."));
	}

};
