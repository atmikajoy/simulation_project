#ifndef EVENT_PUBLISHER_H_INCLUDED
#define EVENT_PUBLISHER_H_INCLUDED

#include "event.h"
#include <map>
#include <vector>
#include <functional>

namespace atmik
{
	namespace simulation
	{
		struct event_publisher
		{

			static event_publisher& default_publisher();

			// important: handlers implement a chain of responsibility
			// a handler that completely handles an event should return true 
			//     so that he event is bot propahated to down-stream handlers
			//
			// listeners implement a publish and subscribe scenario, listeers are the subscribers
			//     the event is forwarded to all listeners 

			using handler_fn_type = bool(event);
			using listener_fn_type = void(event);

			// important: the order of calling add_handler is crucial
			//            the handler that is added earlier will get the first shot at handling the event
			template < typename FN > void add_handler(event::event_type etype, FN fn )
			{
				subscriptions[etype].handlers.emplace_back(fn);
			}

			// add a handler member function eg.  &TARGET::mem_fun
			template < typename TARGET, typename MEM_FUN >
			void add_handler( event::event_type etype, TARGET* target, MEM_FUN mfn )
			{
				subscriptions[etype].handlers.emplace_back( std::bind( mfn, target, std::placeholders::_1));
			}

			template < typename FN > void add_listener(event::event_type etype, FN fn)
			{
				subscriptions[etype].listeners.emplace_back(fn);
			}

			// add a listener member function eg.  &TARGET::mem_fun
			template < typename TARGET, typename MEM_FUN >
			void add_listener(event::event_type etype, TARGET* target, MEM_FUN mfn)
			{
				subscriptions[etype].listeners.emplace_back(std::bind(mfn, target, std::placeholders::_1));
			}

			// add a listener to all the events that are present in the subscribers map
			template < typename FN > void add_universal_listener(FN fn)
			{
				for( auto et : event::event_type_list() ) 
					subscriptions[et].listeners.emplace_back(fn);
			}

			// add a listener member function to all the events that are present in the subscribers map
			template < typename TARGET, typename MEM_FUN >
			void add_universal_listener( TARGET* target, MEM_FUN mfn )
			{
				for (auto et : event::event_type_list() )
					subscriptions[et].listeners.emplace_back(std::bind(mfn, target, std::placeholders::_1));
			}
			
			void publish( event ev )
			{
				// go down the chain of responsibility till the event is handled
				for (auto& handler : subscriptions[ev.type].handlers)
					if( (ev.handled = handler(ev)) == true ) break;

				// forward to all listeners
				for (auto& listener : subscriptions[ev.type].listeners) listener(ev);
			}

			struct subscribers
			{
				std::vector< std::function<handler_fn_type> > handlers;
				std::vector< std::function<listener_fn_type> > listeners;
			};

			std::map< event::event_type, subscribers > subscriptions;

			void stop() { subscriptions.clear(); }
		};
	}
}

#endif // EVENT_PUBLISHER_H_INCLUDED