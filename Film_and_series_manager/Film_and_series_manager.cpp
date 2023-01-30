#include <iostream>
#include <filesystem>
#include <regex>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

// Add a space before each word in the series name, if the word starts with an uppercase character
void format_series_name(string& series) {
    for (int i = 1; i < series.length(); i++) {
        if (isupper(series[i])) {
            series.insert(i, " ");
            i++;
        }
    }
}

// Extract the series name, season, and episode number from the file name
void extract_info(string file_name, string& series, int& season, int& episode) {
    regex pattern("(.*?)(?:[s|S](\\d+)[e|E](\\d+)|[s|S](\\d+)[_|-][e|E](\\d+))");
    smatch match;
    if (regex_search(file_name, match, pattern)) {
        series = match[1].str();
        // remove any non-alphabetical characters from the series name, except for the first character
        for (int i = 1; i < series.length(); i++) {
            if (!isalpha(series[i])) {
                series.erase(i, 1);
                i--;
            }
        }
        format_series_name(series);
        season = stoi(match[2].str().empty() ? match[4].str() : match[2].str());
        episode = stoi(match[3].str().empty() ? match[5].str() : match[3].str());
    }
}

// Create the folder structure
void create_folders(string download_dir, string series, int season, int episode) {
    replace(series.begin(), series.end(), '_', ' ');
    fs::create_directories(download_dir + "/" + series + "/Season " + to_string(season) + "/Episode " + to_string(episode));
}

// Move the file to the appropriate folders
void move_file(string file_path, string download_dir, string series, int season, int episode) {
    // Check if the folder already exists
    string folder_path = download_dir + "/" + series + "/Season " + to_string(season) + "/Episode " + to_string(episode);
    if (!fs::exists(folder_path)) {
        create_folders(download_dir, series, season, episode);
    }
    fs::path new_file_path = folder_path + "/" + file_path.substr(file_path.find_last_of("/\\") + 1);
    fs::rename(file_path, new_file_path);
}

// Check if the file is a video
bool is_video_file(string file_path) {
    string file_extension = file_path.substr(file_path.find_last_of(".") + 1);
    return (file_extension == "mkv" || file_extension == "mp4" || file_extension == "mov" || file_extension == "avi");
}

int main() {
    string download_dir = "D:/Series/TEST";
    string series_name = "*";

    for (const auto& entry : fs::recursive_directory_iterator(download_dir)) {
        if (!fs::is_regular_file(entry.path())) // check if the entry is a regular file 
            continue;
        std::string file_path = entry.path().string();
        if (!is_video_file(file_path)) // check if the file is a video file 
            continue;

        std::string series;
        int season, episode;

        extract_info(entry.path().stem().string(), series, season, episode);
        create_folders(download_dir, series, season, episode);
        move_file(file_path, download_dir, series, season, episode);
    }
    return 0;
}