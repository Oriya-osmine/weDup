#include "WeSession.h"
namespace fs = std::filesystem;
const char *defaultProjectsLocation =
    "C:\\Program Files "
    "(x86)\\Steam\\SteamApps\\common\\wallpaper_engine\\projects\\myprojects";
const char *defaultWorkshopocation =
    "C:\\Program Files (x86)\\Steam\\SteamApps\\workshop\\content\\431960";
const char *projects_txt = "PathToMyprojects.txt";
const char *workshop_txt = "PathToWorkshop.txt";
const char *WE_txt = "WE.txt";
WeSession::WeSession() {
  CheckPaths(defaultProjectsLocation, projects_txt);
  CheckPaths(defaultWorkshopocation, workshop_txt);
  lengthOfWorkshopPath = (int)pathToWorkshop.length() + 1;
  PutintoString();
}
void WeSession::PutintoString() {
  // Putting all recorded projects into a string since
  // it is much faster to check if a string is in another string
  // than to open a file, go all over his lines and check if the string matches
  if (!fs::exists(WE_txt)) {
    return;
  }
  std::fstream weFile(WE_txt);
  std::string currentLine{""};
  if (!weFile) {
    throw std::ios_base::failure("Failed to open WE.txt");
  }
  while (std::getline(weFile, currentLine)) {
    AddItem(currentLine);
  }
  weFile.close();
}
void WeSession::CheckPaths(const char *path, const char *path_txt) {

  if (!fs::exists(path_txt)) {
    std::cout << "file does not exist, creating file " << path_txt << "...\n";

    if (std::filesystem::exists(path))
        Writepaths(path, path_txt);
    else {
        setPathsFromUser(path_txt);
    }
  } else {
    // reads the path from PathTo(MyProjects/workshop).txt
    if (path_txt == projects_txt) {
      std::fstream WE_paths(path_txt, std::ios::in);
      pathToMyProjects.assign(std::istreambuf_iterator<char>(WE_paths),
                              std::istreambuf_iterator<char>());
      WE_paths.close();
    } else if (path_txt == workshop_txt) {
      std::fstream WE_paths(path_txt, std::ios::in);
      pathToWorkshop.assign(std::istreambuf_iterator<char>(WE_paths),
                            std::istreambuf_iterator<char>());
      WE_paths.close();
    } else {
      throw "Invalid txt when reading";
    }
  }
}
void WeSession::Writepaths(const char* path, const char* path_txt) {
    std::string MpOrWs = path_txt == projects_txt ? "Myprojects" : "Workshop";
    std::cout
        << "We have detected your" << MpOrWs  << " content location is in\n"
        << defaultProjectsLocation
        << "\nEnter: \"yes\" if that is correct and \"no\" if it is not/you "
        "want to choose a different path\n";

    std::string answer{ "" };

    while (answer != "yes" && answer != "no") {
        std::getline(std::cin, answer);
        void IsBadInput();
        if (answer == "no") {
            setPathsFromUser(path_txt);
            return;

        }
        else if (answer == "yes") {
            std::ofstream Myprojects(path_txt, std::ios::out);
            Myprojects << path;
            Myprojects.close();
            CheckPaths(path, path_txt);
        }
        else {
            std::cout
                << "invalid command, please enter again\n";
        }
    }
}
void WeSession::setPathsFromUser(const char *path_txt) {
  // gets the path and puts it in PathTo(Myprojects/Workshop).txt
  if (path_txt == projects_txt) {
    std::cout
        << "Enter: your projects steam path \nshould look something like "
           "this:\n"
        << defaultProjectsLocation
        << "\nYou can check this yourself by going to wallpaper engine and "
        << "right clicking on a wallpaper you CREATED(on this device) for wallpaper "
        << "engine and choose 'open in explorer' and then go back one "
        << "folder (the folder you go back to should be "
        << "'myprojects') copy the path and paste it here\n";
    getline(std::cin, pathToMyProjects);
    std::ofstream getPaths(path_txt, std::ios::out);
    getPaths << pathToMyProjects;
    getPaths.close();
  } else if (path_txt == workshop_txt) {
    std::cout
        << "Enter: your workshop path it should look something like "
           "this:\n"
        << defaultWorkshopocation
        << "\nYou can check this yourself by going to wallpaper engine "
           "and right clicking on a wallpaper you SUBSCRIBED to(downloaded) "
           "and choose 'open in explorer'and then go back one "
           "folder (the folder you go back to should be '431960') "
           "copy the path and paste it here\n";
    getline(std::cin, pathToWorkshop);
    std::ofstream getPaths(path_txt, std::ios::out);
    getPaths << pathToWorkshop;
    getPaths.close();
  } else {
    throw std::invalid_argument("Invalid txt when writing");
  }
}

std::string WeSession::GetpathToWorkshop() const { return pathToWorkshop; }
std::string WeSession::GetPathToMyProjects() const { return pathToMyProjects; }
void WeSession::AddItem(std::string addProject) {
  recordedItems.insert(addProject);
}
int WeSession::getLengthOfWorkshopPath() const { return lengthOfWorkshopPath; }
int WeSession::GetProjectsAdded() const { return projectsAdded; }
void WeSession::AddProjectsAdded() { ++projectsAdded; }
void WeSession::subtractprojectsAdded() { --projectsAdded; }
void WeSession::IsCopyCon() {
    std::cout << "Choose an option:\n"
        << "1. 'choose' to manually select what new projects to copy\n"
        << "2. 'copy' to copy every new project\n"
        << "3. 'record' to record all new projects\n";
    std::string option{ "" };
    while (option != "copy" && option != "choose" && option != "record") {
        std::getline(std::cin, option);
        IsBadInput();
        if (option == "choose") {
            isCopy = choose;
            break;
        }
        else if (option == "copy") {
            isCopy = copyAll;
            break;
        }
        else if (option == "record") {
            isCopy = recordAll;
            break;
        }
        std::cout << "Invalid option, please try again.\n";
        
    }
}

void WeSession::SetIsCopy(const CopyTypes &set) { isCopy = set; }
int WeSession::GetIsCopy() const { return isCopy; }
bool WeSession::SearchIfRecorded(const std::string &find) const {
  return std::any_of(recordedItems.begin(), recordedItems.end(),[&](const std::string &item) {return item.find(find) != std::string::npos;});
}
