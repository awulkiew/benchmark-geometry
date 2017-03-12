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
    result_times(std::string const& sha_) : sha(sha_) {}

    std::string sha;
    std::string timestamp;
    std::vector<double> times;
};

struct get_times
{
    std::vector<double> & operator()(result_times & rt) const { return rt.times; }
    std::vector<double> const& operator()(result_times const& rt) const { return rt.times; }
};

struct non_first_comma
{
    bool first;
    non_first_comma() : first(true) {}
    const char * get()
    {
        if (first)
        {
            first = false;
            return "";
        }
        else
        {
            return ",";
        }
    }
};

template <typename RIt>
inline double calculate_median(RIt first, RIt last)
{
    std::size_t size = std::distance(first, last);
    if (size < 1)
        return 0.0;
    std::sort(first, last);
    size_t index_mid = size / 2;
    RIt mid = first + index_mid;
    return size % 2 != 0 ?
        *mid :
        (*mid + *(mid - 1)) / 2.0;
}

typedef std::map<std::string, std::vector<double>> new_tests_map;
typedef std::map<std::string, std::vector<result_times>> tests_map;

result_times & insert_by_sha(std::vector<result_times> & results, std::string const& sha)
{
    auto it = std::find_if(results.begin(), results.end(),
                           [&](result_times const& r) {
                               return r.sha == sha;
                           });
    if (it != results.end())
        return *it;

    results.push_back(result_times(sha));
    return results.back();
}

void sort_by_timestamp(std::vector<result_times> & results)
{
    std::sort(results.begin(), results.end(),
              [](result_times const& r1, result_times const& r2) {
                  return r1.timestamp < r2.timestamp;
              });
}

inline bool load_new_results(std::string const& commit_file_path,
                             new_tests_map & new_tests)
{
    new_tests.clear();

    // load commit file
    std::ifstream commit_file(commit_file_path);
    if (!commit_file.is_open())
    {
        std::cout << "Unable to open file " << commit_file_path << std::endl;
        return false;
    }

    // load all of the test results, name and the container of times for this test
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

    return ! new_tests.empty();
}

template <typename GetTimes>
inline void load_old_results_integrate_and_save(std::string const& output_dir_prefix,
                                                std::string const& commit_time,
                                                std::string const& commit_name,
                                                new_tests_map const& new_tests,
                                                tests_map & all_tests,
                                                GetTimes get_times)
{
    // for each test (name and container of times)
    for (auto const& t : new_tests)
    {
        std::vector<result_times> & results = all_tests[t.first];

        {
            // load the currently stored commits times
            std::ifstream test_file(output_dir_prefix + t.first + ".txt");
            while (test_file.good())
            {
                std::string sha;
                std::string timestamp;
                std::vector<double> times;
                readline(test_file, timestamp, sha, times);
                if (!timestamp.empty() && !sha.empty() && !times.empty())
                {
                    result_times & res = insert_by_sha(results, sha);
                    res.timestamp = timestamp;
                    get_times(res) = times;
                }
            }
        }

        // merge the latest result
        result_times & res = insert_by_sha(results, commit_name);
        res.timestamp = commit_time;
        get_times(res) = t.second;

        // sort the results by timestamp
        sort_by_timestamp(results);

        // save the commit file
        {
            std::ofstream test_file(output_dir_prefix + t.first + ".txt", std::ios::trunc);
            if (test_file.is_open())
            {
                for (auto const& r : results)
                {
                    test_file << r.timestamp << " " << r.sha << " " << std::fixed << std::setprecision(12);
                    std::copy(get_times(r).begin(), get_times(r).end(), std::ostream_iterator<double>(test_file, " "));
                    test_file << std::endl;
                }
            }
        }
    }
}

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

    new_tests_map new_tests;
    if (!load_new_results(commit_name, new_tests))
    {
        return 1;
    }

    // load old tests, integrate new tests and save data
    tests_map all_tests;
    load_old_results_integrate_and_save(output_dir_prefix, commit_time, commit_name,
                                        new_tests, all_tests, get_times());

    // save js containing json
    {
        std::ofstream file(output_dir_prefix + "data.js", std::ios::trunc);

        if(! file.is_open())
            return 0;

        file << std::fixed << std::setprecision(12);
        file << "data_json = '[";

        non_first_comma comma;
        for (auto & t : all_tests)
        {
            // non-const ref because times are sorted below in order to find median
            std::vector<result_times> & results = t.second;

            std::string id = to_valid_id(t.first);
            
            file << comma.get() << "{" /*<< std::endl*/;

            file << "\"name\": \"" << t.first << "\"," /*<< std::endl*/;
            file << "\"id\": \"" << id << "\"," /*<< std::endl*/;
            file << "\"results\": [";

            non_first_comma comma;
            for (auto & r : results)
            {
                file << comma.get() << "{" /*<< std::endl*/;
                file << "\"timestamp\": \"" << r.timestamp << "\"," /*<< std::endl*/;
                file << "\"sha\": \"" << r.sha << "\"," /*<< std::endl*/;
                file << "\"median\": " << calculate_median(r.times.begin(), r.times.end()) << "," /*<< std::endl*/;
                file << "\"times\": [";
                {
                    non_first_comma comma;
                    for (size_t i = 0; i < r.times.size(); ++i)
                    {
                        file << comma.get() << r.times[i];
                    }
                }
                file << "]"/* << std::endl*/;
                file << "}";
            }
            
            file << "]"/* << std::endl*/ << "}";
        }

        file << "]';"/* << std::endl*/;
    }

    return 0;
}
