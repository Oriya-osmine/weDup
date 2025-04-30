#include "WeSession.h"
namespace fs = std::filesystem;
const char *const defaultProjectsLocation =
    "C:\\Program Files "
    "(x86)\\Steam\\SteamApps\\common\\wallpaper_engine\\projects\\myprojects";
const char *const defaultWorkshopocation =
    "C:\\Program Files (x86)\\Steam\\SteamApps\\workshop\\content\\431960";
const char *const projectsTXT = "PathToMyprojects.txt";
const char *const workshopTXT = "PathToWorkshop.txt";
const char *const WeTXT = "WE.txt";
const char *const RePKGEXE = "RePKG.exe";

WeSession::WeSession() {
  CheckPaths(defaultProjectsLocation, projectsTXT);
  CheckPaths(defaultWorkshopocation, workshopTXT);
  lengthOfWorkshopPath = (static_cast<int>(pathToWorkshop.length()) + 1);
  PutintoSet();
  CheckRePKG();
}
void WeSession::PutintoSet() {
  if (!fs::exists(WeTXT)) {
    return;
  }
  std::fstream WeFile(WeTXT);
  std::string currentLine{""};
  if (!WeFile) {
    throw std::runtime_error("Failed to open WE.txt");
  }
  while (std::getline(WeFile, currentLine)) {
    AddItem(currentLine);
  }
  WeFile.close();
}
void WeSession::CheckPaths(const char *const path, const char *const pathTXT) {

  if (!fs::exists(pathTXT)) {
    std::cout << "file does not exist, creating file " << pathTXT << "...\n";

    if (std::filesystem::exists(path))
      Writepaths(path, pathTXT);
    else {
      setPathsFromUser(pathTXT);
    }
  } else {
    std::fstream WePaths(pathTXT, std::ios::in);
    if (!WePaths) {
        throw std::runtime_error("Failed to open " + std::string(pathTXT));
    }
    pathTXT == projectsTXT
        ? pathToMyProjects.assign(std::istreambuf_iterator<char>(WePaths),
                                  std::istreambuf_iterator<char>())
        : pathToWorkshop.assign(std::istreambuf_iterator<char>(WePaths),
                                std::istreambuf_iterator<char>());
    WePaths.close();
  }
}
void WeSession::Writepaths(const char *const path, const char *const pathTXT) {
  std::string MpOrWs = pathTXT == projectsTXT ? "Myprojects" : "Workshop";
  std::string DefaultMporWslocation = pathTXT == projectsTXT
                                          ? defaultProjectsLocation
                                          : defaultWorkshopocation;

  std::cout << "We have detected your" << MpOrWs << " content location is in\n"
            << DefaultMporWslocation << "\nChoose an option:\n"
            << "1. 'yes' if correct \n"
            << "2. 'no' if wrong, allows you to enter the path yourself\n";

  std::string option{""};
  while (option != "yes" && option != "no") {
    std::getline(std::cin, option);
    IsBadInput();
    ToLowerLoop(option);
    if (option == "no") {
      setPathsFromUser(pathTXT);
      return;
    } else if (option == "yes") {
      std::ofstream WePaths(pathTXT, std::ios::out);
      if (!WePaths) {
          throw std::runtime_error("Failed to open " + std::string(pathTXT));
      }
      WePaths << path;
      WePaths.close();
      CheckPaths(path, pathTXT);
    } else {
      std::cout << "invalid command, please enter again\n";
    }
  }
}
void WeSession::setPathsFromUser(const char *const pathTXT) {
  // gets the path and puts it in PathTo(Myprojects/Workshop).txt
  if (pathTXT == projectsTXT) {
    std::cout
        << "Enter your projects steam path \nshould look something like "
           "this:\n"
        << defaultProjectsLocation
        << "\nYou can check this yourself by going to wallpaper engine and "
        << "right clicking on a wallpaper you CREATED(on this device) for "
           "wallpaper "
        << "engine and choose 'open in explorer' and then go back one "
        << "folder (the folder you go back to should be "
        << "'myprojects') copy the path and paste it here\n";
    std::getline(std::cin, pathToMyProjects);
    std::ofstream WePaths(pathTXT, std::ios::out);
    if (!WePaths) {
        throw std::runtime_error("Failed to open " + std::string(pathTXT));
    }
    WePaths << pathToMyProjects;
    WePaths.close();
  } else if (pathTXT == workshopTXT) {
    std::cout
        << "Enter your workshop path it should look something like "
           "this:\n"
        << defaultWorkshopocation
        << "\nYou can check this yourself by going to wallpaper engine "
           "and right clicking on a wallpaper you SUBSCRIBED to(downloaded) "
           "and choose 'open in explorer'and then go back one "
           "folder (the folder you go back to should be '431960') "
           "copy the path and paste it here\n";
    std::getline(std::cin, pathToWorkshop);
    std::ofstream WePaths(pathTXT, std::ios::out);
    if (!WePaths) {
        throw std::runtime_error("Failed to open " + std::string(pathTXT));
    }
    WePaths << pathToWorkshop;
    WePaths.close();
  } else {
    throw std::invalid_argument("Invalid txt when writing");
  }
}

void WeSession::CheckRePKG() {
  if (!fs::exists(RePKGEXE))
    throw std::runtime_error((std::string)RePKGEXE + "doesn't exist");
}

std::string WeSession::GetpathToWorkshop() const { return pathToWorkshop; }
std::string WeSession::GetPathToMyProjects() const { return pathToMyProjects; }
void WeSession::AddItem(const std::string &addProject) {
  recordedItems.push_back(addProject);
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
  std::string option{""};
  while (option != "copy" && option != "choose" && option != "record") {
    std::getline(std::cin, option);
    IsBadInput();
    ToLowerLoop(option);
    if (option == "choose") {
      isCopy = choose;
      break;
    } else if (option == "copy") {
      isCopy = copy;
      break;
    } else if (option == "record") {
      isCopy = record;
      break;
    }
    std::cout << "Invalid option, please try again.\n";
  }
}

void WeSession::SetIsCopy(const CopyTypes &set) { isCopy = set; }
int WeSession::GetIsCopy() const { return isCopy; }
bool WeSession::SearchIfRecorded(const std::string &find) const {
  return std::any_of(recordedItems.begin(), recordedItems.end(),
                     [&](const std::string &item) {
                       return item.find(find) != std::string::npos;
                     });
}
