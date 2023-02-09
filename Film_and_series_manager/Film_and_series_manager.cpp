#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <filesystem>
#include <wx/wx.h>
#include <wx/dir.h>
#include <regex>
#include <algorithm>
#include <cctype>
#include <string>

using namespace std;
namespace fs = std::filesystem;

// Format the series for create_folder
void format_series_name(string& series) {
    int num_digits = 0;
    // Add a space before each word in the series name, if the word starts with an uppercase character
    transform(series.begin(), series.end(), series.begin(), ::tolower);
    series[0] = toupper(series[0]);
    for (int i = 1; i < series.length(); i++) {
        if (isalpha(series[i]) && (series[i - 1] == '.' || series[i - 1] == '-' || series[i - 1] == '_' || series[i - 1] == ' ')) {
            series[i] = toupper(series[i]);
            series[i - 1] = ' ';
        }
    }
    // Remove any non-alphabetical characters from the series name, except for the first character, numbers and spaces
    for (int i = 1; i < series.length(); i++) {
        if (!isalpha(series[i]) && !(series[i] == ' ') && !isdigit(series[i])) {
            series.erase(i, 1);
            i--;
        }
    }
    // Remove the released year
    for (int i = series.length() - 1; i >= 0; i--) {
        if (isdigit(series[i])) {
            num_digits++;
        }
        else {
            break;
        }
    }
    if (num_digits >= 4 && series.length() > 4) {
        series.erase(series.length() - num_digits, num_digits);
    }
}

// Removes specified words from input
void remove_words(string& input) {
    vector<string> words_to_remove = { "480p", "720p", "1080p", "2160p", "-DL", "-dl", "WEB", "web", "x265", "x264" };

    for (string word : words_to_remove) {
        size_t found = input.find(word);
        while (found != string::npos) {
            input.erase(found, word.length());
            found = input.find(word);
        }
    }
}

// Extract the series name, season, and episode number from the file name
void extract_info(string file_name, string& series, int& season, int& episode) {
    remove_words(file_name); // remove specified words from the file name
    regex pattern("(.*?)(?:[s|S](\\d+)[e|E](\\d+)|[s|S](\\d+)[_|-][e|E](\\d+))");
    smatch match;
    if (regex_search(file_name, match, pattern)) {
        series = match[1].str();
        
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

class The_GUI : public wxApp {
public:
    virtual bool OnInit() {
        wxDirDialog dialog(NULL, "Choose the directory of your series", wxGetCwd(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dialog.ShowModal() == wxID_CANCEL)
            return false;

        std::string download_dir = dialog.GetPath().ToStdString();
        std::string series_name = "*";

        for (const auto& entry : fs::recursive_directory_iterator(download_dir)) {
            if (!fs::is_regular_file(entry.path())) // check if the entry is a regular file 
                continue;
            std::string file_path = entry.path().string();
            if (!is_video_file(file_path)) // check if the file is a video file 
                continue;

            std::string series;
            int season, episode;

            remove_words(series_name);
            extract_info(entry.path().stem().string(), series, season, episode);
            create_folders(download_dir, series, season, episode);
            move_file(file_path, download_dir, series, season, episode);
        }

        return true;
    }
};

wxIMPLEMENT_APP(The_GUI);

int main(int argc, char** argv) {
    return wxEntry(argc, argv);
}