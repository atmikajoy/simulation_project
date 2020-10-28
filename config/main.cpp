#include "config.h"
#include <iostream>
#include "config.h"

// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/xml_parser.hpp>

// namespace bpt = boost::property_tree;

int main()
{
	double fuel_rate = atmik::config::get<double>("simulation.bays.petrol.fuel_rate", 8.0);
	std::cout << "simulation.bays.petrol.fuel_rate " << fuel_rate << '\n';

	int num_petrol_bays = atmik::config::get<int>("simulation.bays.petrol.num_bays", 3);
	std::cout << "simulation.bays.petrol.num_bays " << num_petrol_bays << '\n';

	/*
	const std::string file_name = atmik::config::file_name();

	bpt::ptree ptree;

	{
		ptree.add("simulation.vehicles.car.arrival_rate", 12.5);
		ptree.add("simulation.bays.fuel_type", "petrol");
		ptree.put("simulation.bays.petrol.fuel_rate", 9.5);

		bpt::write_xml(file_name, ptree, std::locale::classic(),
			bpt::xml_parser::xml_writer_make_settings<std::string>(' ', 4));
		std::cout << "xml written to file " << file_name << '\n';
		// ptree.clear();
	}
	
	{
		bpt::read_xml(file_name, ptree, bpt::xml_parser::trim_whitespace );

		double fuel_rate = ptree.get<double>("simulation.bays.petrol.fuel_rate", 8.0);
		std::cout << "simulation.bays.petrol.fuel_rate" << fuel_rate << '\n';

		try { ptree.get<double>("xxx.yyy"); }
		catch (const std::exception& e)
		{
			std::cout << "exception of type " << typeid(e).name() << '\n';
		}
	}
	
	
	ptree.add("simulation.bays.fuel_type", "LPG");
	bpt::write_xml(file_name, ptree, std::locale::classic(),
		bpt::xml_parser::xml_writer_make_settings<std::string>(' ', 4));
	std::cout << "xml written to file " << file_name << '\n';
	*/
}
