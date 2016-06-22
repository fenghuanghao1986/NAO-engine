/*
Copyright (c) 2016 "University of Denver"

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
-- Created by Paul Heinen 06/21/16

// FILE DESCRIPTION
This debug module enable an easy global use of debugging at various levels of verbosity,
as well as easy to use unit tests. It makes extensive use of Boost.Log and Boost.Test.
*/



#ifdef PINEAPPLE_VERSION_0_0_1
// Put version specific code here
#endif

#ifdef NAO_SDK_VERSION_2_1_4_13
// Put Nao SDK version specific code here
#endif

#include "debug/debug.h"
#include <iomanip>
namespace bl		 = boost::log;
namespace src		 = boost::log::sources;
namespace expr		 = boost::log::expressions;
namespace sinks		 = boost::log::sinks;
namespace attributes = boost::log::attributes;


/** Defining the Log attributes **/

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)


/** Initialize the global logger **/

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, src::severity_logger_mt)
{
   src::severity_logger_mt<boost::log::trivial::severity_level> logger;
   
   //Attributes
   ///Log Line Number
   logger.add_attribute("LineID", attributes::counter<unsigned int>(1));
   ///Log Timestamp
   logger.add_attribute("TimeStamp", attributes::local_clock());

   // text sink
   typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
   boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
   
   //add a log file stream for the sink to use
   sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(LOGFILE));
   

   //add a console output stream to the sink
   sink->locked_backend()->add_stream(boost::shared_ptr<std::ofstream>(&std::clog, boost::serialization::null_deleter()));
   


   //Log message format
   bl::formatter formatter = expr::stream
	  << std::setw(7) << std::setfill('0') << line_id << std::setfill(' ') << " | "
	  << expr::format_date_time(timestamp, "%Y-%m-%d, %H:%M:%S.%f") << " "
	  << "[" << bl::trivial::severity << "]"
	  << " - " << expr::smessage;
   sink->set_formatter(formatter);

   //Set severity filter
   sink->set_filter(severity >= SEVERITY_THRESHOLD);

   //register sink
   bl::core::get()->add_sink(sink);

   return bl;
}
