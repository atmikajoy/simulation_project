
#include "clock.h"
#include <iostream>
#include "event.h"
#include "object_with_unique_id.h"
#include <cassert>
#include "publisher.h"
#include "fuel.h"
#include "vehicle.h"
#include "fuel_storage.h"
#include <unordered_set>

struct node { int data = 0; node* next; };

bool cycle( node* n )
{
	/*
	node* x = n->next;
	while ( x != nullptr && x != n ) x = x->next;
		
	return x == n;
	*/

	std::unordered_set<node*> visited;

	while (n != nullptr);+
		

}

atmik::simulation::event_publisher p;

struct A : atmik::util::object_with_unique_id<A,1000000>
{
	A(int x = 1, double y = 2) : xx(x), yy(y) {}

	bool foo(const atmik::simulation::event& ev) const { return false;  }

	int xx;
	double yy;
};

void report_unhandld_event(atmik::simulation::event ev)
{
	if (!ev.handled) std::cout << "*** warning *** event " << ev << "\n";
}

void print_arrival( atmik::simulation::event e)
{
	std::cout << "arrival: " << atmik::simulation::vehicle::look_up(e.info) << '\n';
}

int main()
{
	{
		using namespace atmik::simulation;
		vehicle::connect_events();

		fuel_storage a(fuel::PETROL);
		fuel_storage b(fuel::DIESEL);

		// service_bay::connect_events();
		a.connect_events();
		b.connect_events();

		auto& v = vehicle::next_arrival(vehicle_type::CAR, 0);
		atmik::simulation::event_publisher::default_publisher().publish(
			event(event::VEHICLE_ARRIVAL, v.arrival_time, 0, v.id()));
		return 0;
	}
	{
		using namespace atmik::simulation;
		p.add_listener(event::VEHICLE_ARRIVAL, print_arrival);

		vehicle& a = vehicle::next_arrival(vehicle_type::CAR, 0);
		vehicle& c = vehicle::next_arrival(vehicle_type::HEAVY_VEHICLE, 0);
		vehicle& d = vehicle::next_arrival(vehicle_type::TWO_WHEELER, 0);
		vehicle& e = vehicle::next_arrival(vehicle_type::THREE_WHEEELER, 0);

		p.publish( event(event::VEHICLE_ARRIVAL, a.arrival_time, 0, a.id() ) );
		p.publish(event(event::VEHICLE_ARRIVAL, c.arrival_time, 0, c.id()));
		p.publish(event(event::VEHICLE_ARRIVAL, d.arrival_time, 0, d.id()));
		p.publish(event(event::VEHICLE_ARRIVAL, e.arrival_time, 0, e.id()));

		auto prev_arrival = a.arrival_time;
		for (int i = 0; i < 20; ++i)
		{
			vehicle& b = vehicle::next_arrival(vehicle_type::CAR, prev_arrival);
			p.publish(event(event::VEHICLE_ARRIVAL, b.arrival_time, 0, b.id()));
			prev_arrival = b.arrival_time;
		}

		return 0;

	}
	{
		using namespace atmik::simulation;
		for (fuel f : { fuel::PETROL, fuel::DIESEL, fuel::LPG })
			std::cout << to_string(f) << ' ' << price_per_litre(f) << '\n';
		return 0;
	}
	{
		using namespace atmik::simulation;
		p.add_handler( event::VEHICLE_ARRIVAL, [] ( event e ) { return false; } );
		A a;
		p.add_handler(event::VEHICLE_ARRIVAL, &a, &A::foo );

		p.add_universal_listener(report_unhandld_event );

		p.publish(event(event::VEHICLE_ARRIVAL, 1234, 100, 8888 ));

		// return 0;
	}
	{
		int ii = 12;
		double d = 34.56;
		for (int i = 0; i < 10; ++i)
		{
			A& a = A::create( ii, d );
			std::cout << a.id() << ' ' << a.xx << ' ' << a.yy << '\n';
			auto& a1 = A::look_up(a.id());
			assert(std::addressof(a) == std::addressof(a1));
			++ii;
			d += 0.33;
		}

		for (const auto& [k, v] : A::objects) std::cout << k << '\n';
		std::cout << '\n';

		A::erase(10005);
		for (const auto& [k, v] : A::objects) std::cout << k << '\n';

		return 0;
	}
	auto& clk = atmik::simulation::default_clock();
	clk = { { 23, 56, 58 } } ;

	using namespace atmik::simulation;
	event::add_event(event::VEHICLE_ARRIVAL, clk(), 10);
	clk += 600;
	event::add_event(event::VEHICLE_ARRIVAL, clk(), 10, 8765 );
	event::add_event(event::START_SERVICE, clk(), 20 );
	event::add_event(event::END_SERVICE, clk(), 21, 234 );
	event::add_event(event::END_SIMULATION, clk() );

	while (!event::empty()) std::cout << event::next_event() << '\n';

}
