#include "event.h"

std::priority_queue<atmik::simulation::event> atmik::simulation::event::queued_events;

unsigned int atmik::simulation::event::next_id = 0;