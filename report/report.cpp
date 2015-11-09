#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

template <typename Os>
inline std::stringstream getlinestream(Os && os)
{
    std::string line;
    std::getline(os, line);
    return std::stringstream(line);
}

template <typename Os, typename T>
inline void read(Os && os, T& t)
{
    os >> t;
}

template <typename Os, typename T, typename... Ts>
inline void read(Os && os, T& t, Ts&... ts)
{
    read(os, t);
    read(os, ts...);
}

struct result_times
{
    result_times() = default;

    result_times(std::string const& ts, std::string const& sh, double av, double mi, double ma)
        : timestamp(ts)
        , sha(sh)
        , avg(av)
        , min(mi)
        , max(ma)
    {}

    std::string timestamp;
    std::string sha;
    double avg;
    double min;
    double max;
};

int main(int argc, char * argv[])
{
    // path, sha, time
    if (argc < 3)
    {
        std::cout << "Intended usage:\n    report TIME SHA" << std::endl;
        return 1;
    }

    // load commit file
    std::string commit_time = argv[1];
    std::string commit_name = argv[2];
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
        read(getlinestream(commit_file), test_name, test_time);
                
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

        // calculate the min, max and avg time
        double time_min = t.second[0];
        double time_avg = 0.0;
        double time_max = t.second[0];
        for (auto sec : t.second)
        {
            if (sec < time_min)
                time_min = sec;
            if (sec > time_max)
                time_max = sec;
            time_avg += sec;
        }
        time_avg /= t.second.size();

        // create a container for commits times
        std::vector<result_times> results;
        {
            // load the currently stored commits times
            std::ifstream test_file(t.first + ".txt");
            if (test_file.is_open())
            {
                while (test_file.good())
                {
                    result_times result;
                    read(getlinestream(test_file), result.timestamp, result.sha, result.avg, result.min, result.max);
                    if (!result.timestamp.empty() && !result.sha.empty())
                    {
                        results.push_back(result);
                    }
                }
            }
        }

        result_times current_result(commit_time, commit_name, time_avg, time_min, time_max);

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
            std::ofstream test_file(t.first + ".txt", std::ios::trunc);
            if (test_file.is_open())
            {
                for (auto const& r : results)
                {
                    test_file << r.timestamp << " " << r.sha << " " << std::fixed << std::setprecision(12) << r.avg << " " << r.min << " " << r.max << std::endl;
                }
            }
        }

        //save html chart file
        {
            std::ofstream test_file(t.first + ".html", std::ios::trunc);
            if (test_file.is_open())
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
                    << "data.addColumn({ id:'i0', type : 'number', role : 'interval' });"
                    << "data.addColumn({ id:'i1', type : 'number', role : 'interval' });"
                    << "data.addRows([";

                bool first = true;
                for (auto const& r : results)
                {
                    if (!first)
                        test_file << ",";
                    else
                        first = false;
                    std::string sha = r.sha;
                    if (sha.size() > 7)
                        sha.resize(7);
                    test_file << "['" << sha << "', " << std::fixed << std::setprecision(12) << r.avg << ", " << r.min << ", " << r.max << "]" << std::endl;
                }

                test_file
                    << "]);"

                    << "var options = {"
                    << "curveType : 'none',"
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

        // problem: if a test won't be in the commit file the chart won't be included here
        {
            std::ofstream test_file("index.html", std::ios::trunc);
            test_file
                << "<html><head><style>"
                << "iframe{ display: block; overflow: hidden; width: 1050px; height: 260px; border: 0; }"
                << "</style></head><body>";

            // TOC
            test_file << "<div style=\"float:left; vertical-align: top; width:200px;\">"
                      << "<h2 style=\"margin-top:0;\">Benchmarks</h2>"
                      << "<ul style=\"list-style-type:none; padding: 0;\">";
            for (auto const& t : tests)
            {
                test_file << "<li><a href=\"#" << t.first << "\">" << t.first << "</a></li>";
            }
            test_file << "</ul></div>";

            test_file << "<div style=\"margin-left:200px; width: 1100px;\">";
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