#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

template <typename Is, typename T>
inline void read(Is & is, T& t)
{
    is >> t;
}

template <typename Is, typename T>
inline void read(Is & is, std::vector<T>& vec)
{
    for (;;)
    {
        T tmp = T();
        read(is, tmp);
        if (!is.good())
            break;
        vec.push_back(tmp);
    }
}

template <typename Is, typename T, typename... Ts>
inline void read(Is & is, T& t, Ts&... ts)
{
    read(is, t);
    read(is, ts...);
}

template <typename Is, typename... Ts>
inline void readline(Is & is, Ts&... ts)
{
    std::string line;
    std::getline(is, line);
    std::stringstream ss(line);

    read(ss, ts...);
}

inline std::string to_valid_id(std::string const& str)
{
    std::string res = str;
    for (char & c : res)
    {
        if (c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
            c = '_';
    }
    if (res.empty())
        res = "_";
    if (res[0] >= '0' && res[0] <= '9')
        res = std::string("_") + res;
    return res;
}

struct result_times
{
    result_times() = default;

    result_times(std::string const& ts, std::string const& sh, std::vector<double> const& ti)
        : timestamp(ts)
        , sha(sh)
        , times(ti)
    {}

    std::string timestamp;
    std::string sha;
    std::vector<double> times;
};

int main(int argc, char * argv[])
{
    // path, sha, time, output-dir
    if (argc < 4)
    {
        std::cout << "Intended usage:\n    report TIME SHA OUTPUT-DIR" << std::endl;
        return 1;
    }

    std::string commit_time = argv[1];
    std::string commit_name = argv[2];
    std::string output_dir_name = argv[3];
    std::string output_dir_prefix = output_dir_name.empty() ? "" : output_dir_name + "/";

    // load commit file
    std::ifstream commit_file(commit_name);
    if (!commit_file.is_open())
    {
        std::cout << "Unable to open file " << commit_name << std::endl;
        return 1;
    }

    // load all of the test results, name and the container of times for this test
    std::map<std::string, std::vector<double>> new_tests;
    while (commit_file.good())
    {
        // load test name and time
        std::string test_name;
        double test_time = 0.0;
        readline(commit_file, test_name, test_time);
                
        // append the time to the container
        if (!test_name.empty())
        {
            new_tests[test_name].push_back(test_time);
        }
    }
    
    // load old tests, integrate new tests and save data
    std::map<std::string, std::vector<result_times>> all_tests;
    // for each test (name and container of times)
    for (auto const& t : new_tests)
    {
        std::vector<result_times> & results = all_tests[t.first];

        {
            // load the currently stored commits times
            std::ifstream test_file(output_dir_prefix + t.first + ".txt");
            while (test_file.good())
            {
                result_times result;
                readline(test_file, result.timestamp, result.sha, result.times);
                if (!result.timestamp.empty() && !result.sha.empty())
                {
                    results.push_back(result);
                }
            }
        }

        result_times current_result(commit_time, commit_name, t.second);

        // merge the latest result
        auto it = std::find_if(results.begin(), results.end(),
                               [&](result_times const& r) {
                                   return r.sha == commit_name;
                               });

        if (it != results.end())
        {
            *it = current_result;
        }
        else
        {
            results.push_back(current_result);
        }

        // sort the results by timestamp
        std::sort(results.begin(), results.end(),
                  [](result_times const& r1, result_times const& r2) {
                     return r1.timestamp < r2.timestamp;
                  });

        // save the commit file
        {
            std::ofstream test_file(output_dir_prefix + t.first + ".txt", std::ios::trunc);
            if (test_file.is_open())
            {
                for (auto const& r : results)
                {
                    test_file << r.timestamp << " " << r.sha << " " << std::fixed << std::setprecision(12);
                    std::copy(r.times.begin(), r.times.end(), std::ostream_iterator<double>(test_file, " "));
                    test_file << std::endl;
                }
            }
        }
    }

    // save html
    {
        std::ofstream test_file(output_dir_prefix + "index.html", std::ios::trunc);
        
        if (! test_file.is_open())
            return 0;
        
        test_file
            << "<html><head>" << std::endl
            << "<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>" << std::endl
            << "<script type=\"text/javascript\">" << std::endl
            << "google.charts.load('current', {'packages':['corechart']});" << std::endl;

        for (auto & t : all_tests)
        {
            // non-const ref because times are sorted below in order to find median
            std::vector<result_times> & results = t.second;

            std::string id = to_valid_id(t.first);
            std::string draw_function = std::string("draw_") + id;
            std::string elem_id = std::string("chart_") + id;

            size_t max_times_size = 0;
            for (auto const& r : results)
            {
                if (r.times.size() > max_times_size)
                    max_times_size = r.times.size();
            }

            if (max_times_size > 0)
            {
                test_file
                    << "google.charts.setOnLoadCallback(" << draw_function << ");" << std::endl

                    << "function " << draw_function << "() {" << std::endl
                    << "var data = new google.visualization.DataTable();" << std::endl
                    << "data.addColumn('string', 'sha');" << std::endl
                    << "data.addColumn('number', 'time');" << std::endl
                    << "data.addColumn({ id:'i0', type : 'number', role : 'interval' });" << std::endl;
                if (max_times_size >= 2)
                    test_file << "data.addColumn({ id:'i1', type : 'number', role : 'interval' });" << std::endl;
                for (size_t i = 2; i < max_times_size; ++i)
                    test_file << "data.addColumn({ id:'i2', type : 'number', role : 'interval' });" << std::endl;

                test_file << "data.addRows([" << std::endl;

                bool first = true;
                for (auto & r : results)
                {
                    if (r.times.empty())
                        continue;

                    if (!first)
                        test_file << ",";
                    else
                        first = false;

                    //double mean = std::accumulate(r.times.begin(), r.times.end(), 0.0) / r.times.size();

                    std::sort(r.times.begin(), r.times.end());
                    size_t index_mid = r.times.size() / 2;
                    double median = r.times.size() % 2 != 0 ?
                        r.times[index_mid] :
                        (r.times[index_mid] + r.times[index_mid - 1]) / 2.0;

                    std::string sha = r.sha;
                    if (sha.size() > 7)
                        sha.resize(7);

                    test_file << "['" << sha << "', " << std::fixed << std::setprecision(12) << median << ", ";
                    for (size_t i = 0; i < max_times_size; ++i)
                    {
                        if (i < r.times.size())
                            test_file << r.times[i];

                        test_file << ", "; // last empty element must be ended with comma and for non-empty elements it doesn't matter
                    }
                    test_file << "]" << std::endl;
                }

                test_file
                    << "]);" << std::endl

                    << "var options = {" << std::endl
                    << "curveType : 'none'," << std::endl
                    << "intervals: { 'style':'points', pointSize: 5 }," << std::endl
                    << "legend : 'none'" << std::endl
                    << "};" << std::endl

                    << "var chart = new google.visualization.LineChart(document.getElementById('" << elem_id << "'));" << std::endl
                    << "chart.draw(data, options);" << std::endl

                    << "var selectHandler = function(e) {" << std::endl
                    << "var loc = 'https://github.com/boostorg/geometry/commit/' + data.getValue(chart.getSelection()[0]['row'], 0 );" << std::endl
                    << "window.location = loc;" << std::endl
                    << "window.top.location = loc;" << std::endl
                    << "};" << std::endl

                    << "google.visualization.events.addListener(chart, 'select', selectHandler);" << std::endl

                    << "}" << std::endl;
            }
        }

        test_file
            << "</script></head>" << std::endl
            << "<body>" << std::endl;

        for (auto const& t : all_tests)
        {
            std::string id = to_valid_id(t.first);
            std::string elem_id = std::string("chart_") + id;

            test_file << "<h3 id=\"" << id << "\" style=\"margin-bottom:0;\">" << t.first << "</h3>";
            test_file << "<div id=\"" << elem_id << "\">";

            test_file << "</div>";
        }

        test_file
            << "</body></html>";
    }

    return 0;
}
