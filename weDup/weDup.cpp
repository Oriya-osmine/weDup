#include "WeSession.h"

namespace fs = std::filesystem;
enum answer { no, yes, alwaysNo, alwaysYes };
void Start(WeSession paths);
void WeExists();
bool TransferProject(const std::string &addProject, WeSession &paths);
void CopyFolder(const std::string &oldPath, const std::string &newPath);
void ProcessProjects(WeSession &paths);
void DirExists(const std::string &Directory);
void Unpkg(const std::string &oldPathProject,
           const std::string &newProjectPath);
void NotCopying(WeSession &paths, const std::string &addProject,
                const std::string &title);
void RunSystemCommands(const std::string &command);
void SearchMenu(WeSession &paths);
bool Search(WeSession &paths);
const char* SearchTitleorDescription();
void ToLowerLoop(std::string &toLower);
std::pair<std::string, std::string> FindTitleAndDescription(const std::string& path, const std::string& project);
bool CompareTitles(const std::string &project, const std::string &toProject);

void AddToWE(const std::string &project, const std::string &title,
             WeSession &paths);
void IsBadInput();
// project == wallpaper
int main() {
  try {
    WeSession paths;
    Start(paths);
  } catch (const std::exception &e) {
    std::cout << "Exception: " << e.what() << std::endl;
    return -1;
  }
  std::cout << "\nPress Enter to finish the program";
  std::cin.get();
}

void Start(WeSession paths) {
  WeExists();
  std::cout << "sending you to search menu...\n";
  SearchMenu(paths);
  std::cout << "Checking if upddates are needed \n";
  ProcessProjects(paths);
  switch (paths.GetProjectsAdded()) {
  case 0: {
    std::cout << "Nothing changed";
    break;
  }
  default: {
    std::cout << "Projects added: " << paths.GetProjectsAdded();
    break;
  }
  }
}
void WeExists() {
  if (!fs::exists(WE_txt)) {
    std::cout << "File does not exist, creating file...\n";
    std::ofstream createWe(WE_txt);
    if (!createWe) {
        throw std::runtime_error("Failed to open WE.txt");
    }
    createWe.close();
  }
}
// "Main Menu"
void ProcessProjects(WeSession &paths) {
  paths.IsCopyCon();
  std::string newProject{""};
  for (const auto &entry : fs::directory_iterator(paths.GetpathToWorkshop())) {
    std::string pathOfNewProject{entry.path().string()};
    int checkLength{static_cast<int>(pathOfNewProject.length() -
                                     paths.getLengthOfWorkshopPath())};
    newProject = {
        pathOfNewProject.substr(paths.getLengthOfWorkshopPath(), checkLength)};
    // gets the length and then removes the path except for the project number
    // and inserts it in newProject(this is for cleaner look mostly)

    if (paths.GetIsCopy() == recordAll) {
      std::string title{FindTitleAndDescription(paths.GetpathToWorkshop(), newProject).first};
      AddToWE(newProject, title, paths);
    } else if (!paths.SearchIfRecorded(newProject) &&
               TransferProject(newProject, paths)) {
      paths.AddProjectsAdded();
    }
  }
}

bool TransferProject(const std::string &addProject, WeSession &paths) {

  std::string pathToWorkshopProject{paths.GetpathToWorkshop() + "\\" +
                                    addProject};
  std::string title{FindTitleAndDescription(paths.GetpathToWorkshop(), addProject).first};
  std::string newProjectPath{paths.GetPathToMyProjects() + "\\" + title};
  static std::string ifShow{""};
  std::string isCopy{""};
  switch (paths.GetIsCopy()) {
  case choose: {
    // the title may sometimes be with some strange characters
    // usually beacuse the terminal only shows ascii ones(basic english and numbers)
    // unless you configured it to show all utf-8

    std::cout << "Would you like to copy the wallpaper: " << title
              << "\nChoose an option:\n"
              << "1. 'yes' to copy the wallpaper\n"
              << "2. 'no' to not copy the wallpaper\n";
    std::getline(std::cin, isCopy);
    IsBadInput();
    ToLowerLoop(isCopy);
    while (isCopy != "yes" && isCopy != "no") {
      std::cout << "Invalid input, please enter again\n";
      std::getline(std::cin, isCopy);
      IsBadInput();
      ToLowerLoop(isCopy);
    }
    if (isCopy == "no") {
        NotCopying(paths, addProject, title);
        return false;
    }
    else if (isCopy == "yes")
      break;
    else
      throw std::invalid_argument("Invalid ending while choosing to copy");
  }
  case copyAll: {
    break;
  }
  case recordAll: {
    throw std::invalid_argument("Invalid ending in recordAll path");
  }
  default: {
    throw std::invalid_argument("Invalid isCopy value");
  }
  }

  DirExists(newProjectPath);

  CopyFolder(pathToWorkshopProject, newProjectPath);

  Unpkg(pathToWorkshopProject, newProjectPath);

  AddToWE(addProject, title, paths);
  return true;
}
void DirExists(const std::string &Directory) {
  if (fs::exists(Directory)) {

    throw std::runtime_error(
        "This project's:\n" + Directory +
        " Directory already exists, this program does not override any "
        "directories");
  }
}
void CopyFolder(const std::string &oldPath, const std::string &newPath) {
  fs::copy(oldPath.c_str(), newPath.c_str(),
           std::filesystem::copy_options::overwrite_existing |
               std::filesystem::copy_options::recursive);
}

void Unpkg(const std::string &oldPathProject,
           const std::string &newProjectPath) {
  // gets all the paths to pkg and zip
  // then uses commands to copy the pkg, unpack it to a zip then uzip it
  std::string pathToWorkshopProject_pkg{oldPathProject + "\\scene.pkg"};
  std::string newProjectPath_pkg{newProjectPath + "\\scene.pkg"};
  std::string pathToWorkshopProject_zip(oldPathProject + "\\scene.zip");
  std::string newProjectPath_zip{newProjectPath + "\\scene.zip"};
  std::string commandToUnpkg{"pkg2zip.exe -pkg2zip \"" + newProjectPath_pkg +
                             "\" " + "\"" + newProjectPath_zip + "\""};
  std::string commandToUnzip{"tar -xkvf \"" + newProjectPath_zip + "\" -C \"" +
                             newProjectPath + "\""};
  if (!fs::exists(pathToWorkshopProject_pkg)) {
    // if there is no scene.pkg
    if (fs::exists(oldPathProject + "\\gifscene.pkg")) {
      // if there is a gifscene.pkg
      pathToWorkshopProject_pkg = oldPathProject + "\\gifscene.pkg";
      newProjectPath_pkg = newProjectPath + "\\gifscene.pkg";
      pathToWorkshopProject_zip = oldPathProject + "\\gifscene.zip";
      newProjectPath_zip = newProjectPath + "\\gifscene.zip";
      commandToUnpkg = "pkg2zip.exe -pkg2zip \"" + newProjectPath_pkg + "\" " +
                       "\"" + newProjectPath_zip + "\"";
      commandToUnzip = "tar -xvf \"" + newProjectPath_zip + "\" -C \"" +
                       newProjectPath + "\"";
    } else {
      // if there is no scene.pkg nor gifscene.pkg
      return;
    }
  }

  RunSystemCommands(commandToUnpkg);
  RunSystemCommands(commandToUnzip);
  remove(newProjectPath_pkg.c_str());
  remove(newProjectPath_zip.c_str());
  // deletes the .zip and .pkg file as they are no longer needed
}

void NotCopying(WeSession &paths, const std::string &addProject,
                const std::string &title) {
  static std::string ifShow{""};

  if (paths.SearchIfRecorded(addProject))
    return;
  if (ifShow == "never show") {
    AddToWE(addProject, title, paths);
    return;
  }
  if (ifShow == "always show")
    return;

  std::cout << "Would you like this project:\n"
            << title
            << "\nTo not show up anymore in options for copying in the future";
  std::cout << "Choose an option:\n"
            << "1. 'show' to show them in future sessions\n"
            << "2. 'dont show' to not show them in future sessions\n"
            << "3. 'always show' to do same as 'show' but for all "
               "wallpapers this session that you choose not to copy\n"
            << "4. 'never show' to do same as 'dont show' but for all "
               "wallpapers this session that you choose not to copy\n"
               "(Entring 'always show' or 'never show' is for this "
               "session, while 'show' or "
               "'dont show' is only for this wallpaper)\n";
  std::getline(std::cin, ifShow);
  IsBadInput();
  ToLowerLoop(ifShow);

  while (ifShow != "dont show" && ifShow != "never show" && ifShow != "show" &&
         ifShow != "always show") {
    std::cout << "Invalid input, please enter again\n";
    std::getline(std::cin, ifShow);
    IsBadInput();
    ToLowerLoop(ifShow);
  }
  if (ifShow == "no" || ifShow == "never show") {
    AddToWE(addProject, title, paths);
  }
}

void RunSystemCommands(const std::string &command) {
  // creates a child prossesor to run the unzip and upack commands
  // honestly I barely know what is happening in this function
  // but it works
  std::cout << command << " runs\n";
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  LPSTR lpstr = const_cast<char *>(command.c_str());
  int length = MultiByteToWideChar(CP_ACP, 0, lpstr, -1, NULL, 0);
  LPWSTR useForCommands = new wchar_t[length];
  MultiByteToWideChar(CP_ACP, 0, lpstr, -1, useForCommands, length);
  // Start the child process.
  CreateProcess(NULL,           // No module name (use command line)
                useForCommands, // Command line
                NULL,           // Process handle not inheritable
                NULL,           // Thread handle not inheritable
                FALSE,          // Set handle inheritance to FALSE
                0,              // No creation flags
                NULL,           // Use parent's environment block
                NULL,           // Use parent's starting directory
                &si,            // Pointer to STARTUPINFO structure
                &pi);           // Pointer to PROCESS_INFORMATION structure

  // Wait until child process exits.
  WaitForSingleObject(pi.hProcess, INFINITE);
  // Close process and thread handles.
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

void SearchMenu(WeSession &paths) {
    std::string option{ "" };

  std::cout
      << "Choose an option:\n"
         "1. 'skip' to skip search and go to Main menu\n"
         "2. 'copy' to copy every project search finds\n"
         "3. 'choose' to choose if to copy or not every project search finds\n"
         "This is case insensitive Search in ENGLISH (bla == BLA)\n";
  while (option != "skip" && option != "copy" && option != "choose") {
    std::getline(std::cin, option);
    IsBadInput();
    ToLowerLoop(option);
    if (option == "skip") {
      std::cout << "Returning to Main menu\n";
      break;
    } else if (option == "copy" || option == "choose") {
      paths.SetIsCopy(option == "copy" ? copyAll : choose);
      if (!Search(paths)) {
        std::cout << "Project not found\n";
      }
      std::cout << "Returning to search menu\n";
      option = "";
      std::cout << "Choose an option:\n"
                   "1. 'skip' to skip search\n"
                   "2. 'copy' to copy every project search finds\n"
                   "3. 'choose' to choose if to copy or not every project "
                   "search finds\n"
                   "This is case insensitive Search in ENGLISH (bla == BLA)\n";
    } else {
      std::cout << "invalid command, please enter again\n";
    }
  }
}

bool Search(WeSession &paths) {
  std::string newProject{""};
  std::pair<std::string, std::string> tmpcheckProject{"", ""};
  int found{0};
  int copied{0};
  bool matchFound = false;
  static const char* whereToSearch = SearchTitleorDescription();
  std::cout
      << "Enter the name of the wallpaper you want to copy and I'll Search "
         "for it, you can any language you want\n"
         "Just make sure that your terminal supports it and it is enabled\n";
  std::string checkProject;
  std::getline(std::cin, checkProject);
  IsBadInput();
  ToLowerLoop(checkProject);
  for (const auto &entry : fs::directory_iterator(paths.GetpathToWorkshop())) {
    std::string pathOfNewProject{entry.path().string()};
    int checkLength{static_cast<int>(pathOfNewProject.length() -
                                     paths.getLengthOfWorkshopPath())};
    newProject = {
        pathOfNewProject.substr(paths.getLengthOfWorkshopPath(), checkLength)};
    tmpcheckProject = FindTitleAndDescription(paths.GetpathToWorkshop(), newProject);
    ToLowerLoop(tmpcheckProject.first);
    ToLowerLoop(tmpcheckProject.second);
    matchFound = (whereToSearch == "Title" && CompareTitles(tmpcheckProject.first, checkProject)) ||
        (whereToSearch == "Description" && CompareTitles(tmpcheckProject.second, checkProject)) ||
        (whereToSearch == "TitleAndDescription" && (CompareTitles(tmpcheckProject.first, checkProject) || CompareTitles(tmpcheckProject.second, checkProject)));
    if (matchFound) {
      ++found;
      // if found a match
      if (paths.GetIsCopy() == choose || paths.GetIsCopy() == copyAll) {
        if (TransferProject(newProject, paths)) {
          // if it does not exist yet then copy it and
          // add one to number of added proejcts and insert it to list
          // then returns the newly added project
          paths.AddProjectsAdded();
          ++copied;
        }
      } else
        throw std::invalid_argument(
            "Invalid option for IsCopy while searching");
    }
  }

  // if did not found and or chose to not copy every option
  std::cout << "Search finished\nCopied projects: " << copied
            << "\nFound projects: " << found << "\n";
  return found;
}
const char * SearchTitleorDescription() {
    std::cout << "Choose what to search for:\n"
        << "1. 'title' to search title only\n"
        << "2. 'description' to search description only\n"
        << "3. 'both' to search both\n";
    std::cout << "*Note: this shows only once per session\n";
    std::cout << "Please enter your choice: ";

    std::string option;
    std::getline(std::cin, option); 
    IsBadInput();
    ToLowerLoop(option);
    while (option != "title" && option != "description" && option != "both") {
        std::cout << "invalid command, please enter again\n";
        std::getline(std::cin, option);
        IsBadInput();
        ToLowerLoop(option);
    }
    if (option == "title") {
        return "Title";
    }
    else if (option == "description") {
        return "Description";
    }
    else if (option == "both") {
        return "TitleAndDescription";
    }
}
void ToLowerLoop(std::string &toLower) {
  // used for case insensitive search
  for (auto &c : toLower) {
    if (c >= 'A' && c <= 'Z') {
      c = c + ('a' - 'A');
    }
  }
}

std::pair<std::string, std::string> FindTitleAndDescription(const std::string& path, const std::string& project) {
    std::string jsonPath{ path + "\\" + project + "\\project.json" };
    std::ifstream projectName(jsonPath);
    if (!projectName.is_open()) {
        throw std::runtime_error("Unable to open file: " + jsonPath);
    }

    std::string currentLine{""};
    std::string title = {""};
    std::string description = {""};

    while (std::getline(projectName, currentLine)) {
        if (currentLine.find("\"description\" :") != std::string::npos) {
            // Removes all double quotes
            currentLine.erase(
                std::remove(currentLine.begin(), currentLine.end(), '\"'),
                currentLine.end());

            // Removes all backslashes
            currentLine.erase(
                std::remove(currentLine.begin(), currentLine.end(), '\\'),
                currentLine.end());

            // Removes "description :"
            size_t colonPos = currentLine.find(":");
            if (colonPos == std::string::npos) {
                throw std::invalid_argument("Invalid char missing: Missing ':' in " + jsonPath);
            }
            description = currentLine.substr(colonPos + 1);

            // Removes the ","
            if (!description.empty()) {
                description.pop_back();
            }

            // Removes all spaces and tabs before the first character
            size_t firstNonSpace = description.find_first_not_of(" \t");
            if (firstNonSpace != std::string::npos) {
                description = description.substr(firstNonSpace);
            }
        }
        else if (currentLine.find("\"title\" :") != std::string::npos) {
            // Removes all double quotes
            currentLine.erase(
                std::remove(currentLine.begin(), currentLine.end(), '\"'),
                currentLine.end());

            // Removes all backslashes
            currentLine.erase(
                std::remove(currentLine.begin(), currentLine.end(), '\\'),
                currentLine.end());

            // Removes "title :"
            size_t colonPos = currentLine.find(":");
            if (colonPos == std::string::npos) {
                throw std::invalid_argument("Invalid char missing: Missing ':' in " + jsonPath);
            }
            title = currentLine.substr(colonPos + 1);

            // Removes the ","
            if (!title.empty()) {
                title.pop_back();
            }

            // Removes all spaces and tabs before the first character
            size_t firstNonSpace = title.find_first_not_of(" \t");
            if (firstNonSpace != std::string::npos) {
                title = title.substr(firstNonSpace);
            }
            return { title, description };
        }

    }
    throw std::invalid_argument("Invalid Ending: Title not found in " + jsonPath); 
}

bool CompareTitles(const std::string &project, const std::string &toProject) {
  return project.find(toProject) != std::string::npos;
}
void IsBadInput() {
  if (std::cin.fail()) {
    throw std::invalid_argument("Invalid input");
  }
}
void AddToWE(const std::string &project, const std::string &title,
             WeSession &paths) {
  if (!paths.SearchIfRecorded(project)) {
    paths.AddItem(project);
    std::fstream addToWe(WE_txt, std::ios::app);
    if (!addToWe) {
        throw std::runtime_error("Failed to open WE.txt");
    }
    addToWe << "\n" << project << " - " << title;
    addToWe.close();
  }
}