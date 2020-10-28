#include <iostream>

#include <cstdio>
#include "tee.h"
#include <sstream>
#include <fstream>
#include <random>
#include <algorithm>
#include "log.h"
#include <thread>
#include <chrono>
#include <typeinfo>
#include "thread_pool.h"
#include "report_writer.h" 
#include <iomanip>


void print(const std::vector<std::string>& vec)
{
    static auto& log = atmik::util::log();
    static std::mutex log_mutex;
    int lnno = 0;
    for (const auto& str : vec)
    {
        {
            std::lock_guard temp(log_mutex);
            // log_mutex.lock();
           log << std::setw(2) << ++lnno << ". " << str << '\n' << std::flush;
            // log_mutex.unlock();
        }
    }
}

int main()
{
   
    {   
        std::vector<std::string> lines;
        if (std::ifstream file{ __FILE__ })
        {

            lines.push_back("THIS IS A TEST\n-----\n");
            std::string ln;
            while (std::getline(file, ln)) lines.push_back(ln);
        }
        atmik::util::report_writer::write(lines);

        std::cout << "stopping default pool\n";
        atmik::util::thread_pool::default_pool().stop();

        return 0;
    }
    
        
    
    std::vector<std::string> lines;
    int line_num = 0;
    if (std::ifstream file{ __FILE__ })
    {
        lines.push_back("THIS IS A TEST\n-----\n");
        std::string ln;
        while ( lines.size() < 5000 && std::getline(file, ln)) lines.push_back( std::to_string(++line_num) + ". " + ln );
    }
    lines.push_back("---------------------------------------------");

    //print(lines);

    for (int i = 0; i < 10; ++i) // std::thread(print, std::cref(lines)).detach();
    {
        auto cpy = lines;
        atmik::util::report_writer::write( std::move(cpy) );
    }
   
    // std::cin.get();
    std::cout << "stopping default pool\n";
    atmik::util::thread_pool::default_pool().stop();
   
    std::cout << "exit from main ***\n";
}


#ifdef not_used_for_now

template < typename FN >
void do_task(FN fn)
{
    std::thread{ fn }.detach() ;
}

auto id() { return std::this_thread::get_id(); }

std::mutex cout_mutex;

void my_task(int arg)
{
    std::chrono::milliseconds sleep_time{};
    {
        static std::mutex random_mutex;
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_int_distribution distrib(10, 100);

        using namespace std::literals;
        std::lock_guard lock(random_mutex);
        sleep_time = std::chrono::milliseconds(distrib(rng));
    }
    std::this_thread::sleep_for(sleep_time) ;
    std::lock_guard lock(cout_mutex);
    std::cout << "task " << arg << " by thread " << id() << '\n' << std::flush;
}

int main()
{
    {
       // static std::ofstream file(atmik::util::log_file_name());
       auto& stm = atmik::util::log();
       // return 0;
    }
    {
        
        //auto& logstm = atmik::util::log();
        //logstm << "this is a test\n";
        // return 0;
        
    }
    {
        if (std::ifstream file{ __FILE__ })
        {
            std::vector<std::string> lines;
            lines.push_back("THIS IS A TEST\n-----\n");
            std::string ln;
            while (std::getline(file, ln)) lines.push_back(ln);
            atmik::util::report_writer::write(std::move(lines));
        }
        return 0;
    }

    {
        // atmik::util::thread_pool pool;

        
        std::cout << "main thread " << id() << '\n' << std::flush;

        for (int i = 0; i < 100; ++i) atmik::util::thread_pool::default_pool().queue_task(my_task, i);

        return 0;
    }
    try
    {
        do_task([] { int v; std::cout << "v? " && std::cin >> v; std::cout << v << '\n';  });
    }

    catch (const std::exception& e)
    {
        std::cerr << "eception of type " << typeid(e).name() << '\n'
            << "what: " << e.what() << '\n';
    }

    using namespace std::literals;
    std::this_thread::sleep_for(15s);
}


template struct atmik::util::basic_teebuf<char>;
template struct atmik::util::basic_teestream<char>;

int main()
{
    {
        std::mt19937 rng(std::random_device{}());
        std::normal_distribution<double> distrib(100, 19);
        for (int i = 0; i < 100; ++i)
            atmik::util::log() << "clamped value is " << std::clamp( distrib(rng), 80.0, 120.0 ) << '\n';
        return 0;
    }
    std::ofstream file("test_it.txt");

    atmik::util::teestream tstm( file, std::cout);

    tstm << std::ifstream(__FILE__).rdbuf();

    /*
    std::stringbuf sbuf(std::ios::out);

    atmik::util::basic_teebuf<char> tbuf( std::addressof(sbuf), std::cout.rdbuf() );

    std::ostream test_stm(std::addressof(tbuf));

    test_stm << "this is a test\n" << 12345 << "\nend of test\n";

    std::string str = sbuf.str();

    std::printf("\n\n\nstringbuf contains:\n '%s'\n", str.c_str());

    /*
    std::cout.sync_with_stdio(false);

    for (int i = 0; i < 20; ++i)
    {
        std::printf("from printf: %d   ", i);
        std::cout << "from cout: " << i ;
        std::puts("");
    }
    */
}

#endif // not_used_for_now
