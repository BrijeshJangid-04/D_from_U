#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
const std::string tool_folder = "tools\\";
const std::string yt_dlp_path = tool_folder + "yt-dlp.exe";
const std::string deno_path   = tool_folder + "deno.exe";
#else
const std::string tool_folder = "./tools/";
const std::string yt_dlp_path = tool_folder + "yt-dlp";
const std::string deno_path   = tool_folder + "deno";
#endif

namespace fs = std::filesystem;

// ?? Check if yt-dlp and deno are available
void validateTools() {
    bool yt_dlp_ok = fs::exists(yt_dlp_path);
    bool deno_ok   = fs::exists(deno_path);

    if (!yt_dlp_ok) {
        std::cerr << "? yt-dlp not found at: " << yt_dlp_path << "\n";
        exit(1);
    }
    if (!deno_ok) {
        std::cerr << "? deno not found at: " << deno_path << "\n";
        exit(1);
    }

    std::cout << "? yt-dlp and deno found.\n";

    // Update yt-dlp to nightly for SABR fix
    std::string update_cmd = yt_dlp_path + " -U --update-to nightly";
    system(update_cmd.c_str());

    // Show deno version
    std::string deno_check = deno_path + " --version";
    system(deno_check.c_str());
}

// ?? Create all required folders at startup
void ensureAllFolders() {
    std::string folders[] = { "audio", "videos", "playlist" };
    for (const auto& folder : folders) {
        if (!fs::exists(folder)) {
            fs::create_directories(folder);
            std::cout << "?? Created folder: " << folder << "\n";
        }
    }
}

// ?? Audio download
void downloadAudioOnly(const std::string& url) {
    std::string folder = "audio";
    std::string command = yt_dlp_path + " -x --audio-format mp3 -o \"" + folder + "/%(title)s.%(ext)s\" " + url;
    std::cout << "\n?? Downloading audio to " << folder << "...\n";
    system(command.c_str());
}

// ?? Single video download
void downloadSingle(const std::string& url, const std::string& format) {
    std::string folder = "videos";
    std::string command = yt_dlp_path + " -f " + format + " -o \"" + folder + "/%(title)s.%(ext)s\" " + url;
    std::cout << "\n?? Downloading video to " << folder << "...\n";
    system(command.c_str());
}

// ?? Playlist download with auto subfolder
void downloadPlaylist(const std::string& url) {
    std::string baseFolder = "playlist";
    std::string command = yt_dlp_path + " --yes-playlist -o \"" + baseFolder + "/%(playlist_title)s/%(title)s.%(ext)s\" " + url;
    std::cout << "\n?? Downloading playlist into subfolder...\n";
    system(command.c_str());
}

// ?? Main logic
int main() {
    std::cout << "?? C++ Video Downloader (yt-dlp + deno)\n";

    validateTools();     // ? Check yt-dlp and deno
    ensureAllFolders();  // ?? Create folders

    std::string url;
    std::cout << "Enter video or playlist URL: ";
    std::getline(std::cin, url);

    std::cout << "\nChoose download type:\n1. Video\n2. Audio only\n3. Playlist\nEnter choice (1/2/3): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    if (choice == 1) {
        char show;
        std::cout << "\nShow available formats? (y/n): ";
        std::cin >> show;
        std::cin.ignore();
        if (show == 'y' || show == 'Y') {
            std::string command = yt_dlp_path + " -F " + url;
            system(command.c_str());
        }

        std::string format;
        std::cout << "\nEnter format code (e.g., 22, best): ";
        std::getline(std::cin, format);
        downloadSingle(url, format);

    } else if (choice == 2) {
        downloadAudioOnly(url);

    } else if (choice == 3) {
        downloadPlaylist(url);

    } else {
        std::cout << "? Invalid choice.\n";
    }

    std::cout << "\n? Done!\n";
    return 0;
}
