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
    while (is.good())
    {
        T tmp;
        read(is, tmp);
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
    std::map<std::string, std::vector<double>> tests;
    while (commit_file.good())
    {
        // load test name and time
        std::string test_name;
        double test_time = 0.0;
        readline(commit_file, test_name, test_time);
                
        // append the time to the container
        if (!test_name.empty())
        {
            tests[test_name].push_back(test_time);
        }
    }

    // for each test (name and container of times)
    for (auto const& t: tests)
    {
        if (t.second.empty())
            continue;

        // create a container for commits times
        std::vector<result_times> results;
        {
            // load the currently stored commits times
            std::ifstream test_file(output_dir_prefix + t.first + ".txt");
            if (test_file.is_open())
            {
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
        }

        result_times current_result(commit_time, commit_name, t.second);

        // merge the latest result
        auto it = std::find_if(results.begin(), results.end(), [&](result_times const& r) { return r.sha == commit_name; });
        if (it != results.end())
        {
            *it = current_result;
        }
        else
        {
            results.push_back(current_result);
        }

        // sort the results by timestamp
        std::sort(results.begin(), results.end(), [](result_times const& r1, result_times const& r2) { return r1.timestamp < r2.timestamp; });

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

        //save html chart file
        {
            std::ofstream test_file(output_dir_prefix + t.first + ".html", std::ios::trunc);
            if (test_file.is_open())
            {
                size_t max_times_size = 0;
                for (auto const& r : results)
                {
                    if (r.times.size() > max_times_size)
                        max_times_size = r.times.size();
                }

                if (max_times_size > 0)
                {
                    test_file
                        << "<html><head>"
                        << "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
                        << "<script type=\"text/javascript\">"
                        << "google.load(\"visualization\", \"1\", { packages:[\"corechart\"] });"
                        << "google.setOnLoadCallback(drawChart);"

                        << "function drawChart() {"
                        << "var data = new google.visualization.DataTable();"
                        << "data.addColumn('string', 'sha');"
                        << "data.addColumn('number', 'time');"
                        << "data.addColumn({ id:'i0', type : 'number', role : 'interval' });";
                    if (max_times_size >= 2)
                        test_file << "data.addColumn({ id:'i1', type : 'number', role : 'interval' });";
                    for (size_t i = 2; i < max_times_size; ++i)
                        test_file << "data.addColumn({ id:'i2', type : 'number', role : 'interval' });";

                    test_file << "data.addRows([" << std::endl;

                    bool first = true;
                    for (auto const& r : results)
                    {
                        if (r.times.empty())
                            continue;

                        if (!first)
                            test_file << ",";
                        else
                            first = false;

                        double avg = std::accumulate(r.times.begin(), r.times.end(), 0.0) / r.times.size();

                        std::string sha = r.sha;
                        if (sha.size() > 7)
                            sha.resize(7);

                        test_file << "['" << sha << "', " << std::fixed << std::setprecision(12) << avg << ", ";
                        for (size_t i = 0; i < max_times_size; ++i)
                        {
                            if (i < r.times.size())
                                test_file << r.times[i];

                            test_file << ", "; // last empty element must be ended with comma and for non-empty elements it doesn't matter
                        }
                        test_file << "]" << std::endl;
                    }

                    test_file
                        << "]);"

                        << "var options = {"
                        << "curveType : 'none',"
                        << "intervals: { 'style':'points', pointSize: 5 },"
                        << "legend : 'none'"
                        << "};"

                        << "var chart_lines = new google.visualization.LineChart(document.getElementById('chart_lines'));"
                        << "chart_lines.draw(data, options);"
                        << "}"
                        << "</script>"
                        << "</head><body style=\"margin:0;\">"
                        << "<div id = \"chart_lines\" style = \"width: 1024px; height: 256px;\"></div>"
                        << "</body></html>";
                }
            }
        }

        // problem: if a test won't be in the commit file the chart won't be included here
        {
            std::ofstream test_file(output_dir_prefix + "index.html", std::ios::trunc);
            test_file
                << "<html><head><style>"
                << "iframe{ display: block; overflow: hidden; width: 1050px; height: 260px; border: 0; }"
                << "</style></head><body>";

            // TOC
            test_file << "<div style=\"float:left; vertical-align: top; width:256px;\">"
                      << "<h2 style=\"margin-top:0;\">Benchmarks</h2>"
                      << "<ul style=\"list-style-type:none; padding: 0;\">";
            for (auto const& t : tests)
            {
                test_file << "<li><a href=\"#" << t.first << "\">" << t.first << "</a></li>";
            }
            test_file << "</ul></div>";

            test_file << "<div style=\"margin-left:256px; width: 1100px;\">";
            for (auto const& t : tests)
            {
                test_file << "<h3 id=\"" << t.first << "\" style=\"margin-bottom:0;\">" << t.first << "</h3>";
                test_file << "<iframe src=\"" << t.first << ".html" << "\"></iframe>";
            }
            test_file << "</div>";

            test_file
                << "</body></html>";
        }
    }

    return 0;
}
