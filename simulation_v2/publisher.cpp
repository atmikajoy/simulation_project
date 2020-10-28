#include"publisher.h"

atmik::simulation::event_publisher& atmik::simulation::event_publisher::default_publisher()
{
	static event_publisher publisher;
	return publisher;
}
