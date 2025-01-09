#include "WeSession.h"
#include <windows.h>

namespace fs = std::filesystem;
enum answer { no, yes, alwaysNo, alwaysYes };
void Start(WeSession paths);
bool WeExists();
bool TransferProject(const std::string &addProject, WeSession &paths);
void CopyFiles(const std::string &oldPath, const std::string &newPath);
void ProcessProjects(WeSession &paths, bool isRecord);
void DirExists(const std::string &Directory);
void Unpkg(const std::string &oldPathProject,
           const std::string &newProjectPath);
void RunSystemCommands(const std::string &command);
void SearchMenu(WeSession &paths);
bool Search(WeSession &paths);
void ToLowerLoop(std::string &toLower);
std::string FindTitle(const std::string &path, const std::string &project);
bool CompareTitles(const std::string &project, const std::string &toProject);

void AddToWE(const std::string &project, const std::string &title,
             const WeSession &paths);
void IsBadInput();
// project == wallpaper
int main() {
  try {
    WeSession paths;
    Start(paths);
  } catch (std::string exception) {
    std::cout << exception;
    return -1;
  }
  std::cout << "\nPress Enter to finish the program";
  std::cin.get();
}

void Start(WeSession paths) {
  if (WeExists()) {
    std::cout << "File found!, sending you to search menu...\n";
    SearchMenu(paths);
    std::cout << "Checking if upddates are needed \n";
    ProcessProjects(paths, false);
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
  } else {
    ProcessProjects(paths, true);
  }
}
bool WeExists() {
  // check if WE.txt exists and if it is not then creates it
  // and asks if you want to copy all new projects or not
  // if it does exist returns 1
  if (!fs::exists(WE_txt)) {
    std::cout << "File does not exist, creating file...\n";
    std::ofstream createFile;
    createFile.open(WE_txt);
    createFile.close();
    std::string answer;
    std::cout
        << "Would you like to copy all of your current downloaded projects?\n"
        << "Choose an option:\n"
        << "1. 'yes' to copy them\n"
        << "2. 'no' to just record them (so they won't show up again)\n";

    while (answer != "yes" && answer != "no") {
      std::getline(std::cin, answer);
      IsBadInput();
      if (answer == "yes") {
        return true;
      } else if (answer == "no") {
        return false;
      }
      std::cout << "Invalid input, please enter again\n";
    }
  }
  return true;
}
void ProcessProjects(WeSession &paths, bool isRecord) {
  int dontMakeNewLine{0};
  if (isRecord)
    paths.SetIsCopy(recordAll);
  else
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

    if (paths.SearchIfRecorded(newProject)) {
      continue;
    }

    if (paths.GetIsCopy() == recordAll) {
      if (isRecord && !dontMakeNewLine) {
        std::fstream listWE(WE_txt);
        std::string title{FindTitle(paths.GetpathToWorkshop(), newProject)};
        listWE << newProject << " - " << title;
        ++dontMakeNewLine;
        listWE.close();
        continue;
      }
      // if wants to only record all new projects
      std::string title{FindTitle(paths.GetpathToWorkshop(), newProject)};
      AddToWE(newProject, title, paths);
      continue;
    }
    if (TransferProject(newProject, paths)) {
      paths.AddProjectsAdded();
    }
  }
}

bool TransferProject(const std::string &addProject, WeSession &paths) {

  std::string pathToWorkshopProject{paths.GetpathToWorkshop() + "\\" +
                                    addProject};
  std::string title{FindTitle(paths.GetpathToWorkshop(), addProject)};
  std::string newProjectPath{paths.GetPathToMyProjects() + "\\" + title};
  static std::string ifShow{""};
  std::string isCopy{""};
  switch (paths.GetIsCopy()) {
  case choose: {
    // the title may sometimes be with some strange characters
    // beacuse it only shows ascii ones(basic english and numbers)

    std::cout << "Would you like to copy the wallpaper: " << title
              << "\nChoose an option:\n"
              << "1. 'yes' to copy the wallpaper\n"
              << "2. 'no' to not copy the wallpaper\n";
    std::getline(std::cin, isCopy);
    IsBadInput();
    while (isCopy != "yes" && isCopy != "no") {
      std::cout << "Invalid input, please enter again\n";
      std::getline(std::cin, isCopy);
      IsBadInput();
    }
    if (isCopy == "no") {
      if (paths.SearchIfRecorded(addProject))
        return false;

      if (ifShow == "never show") {
        AddToWE(addProject, title, paths);
        return false;
      }
      if (ifShow == "always show")
        return false;

      std::cout
          << "Would you like this project:\n"
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
      while (ifShow != "dont show" && ifShow != "never show" &&
             ifShow != "show" && ifShow != "always show") {
        std::cout << "Invalid input, please enter again\n";
        std::getline(std::cin, ifShow);
        IsBadInput();
      }
      if (ifShow == "no" || ifShow == "never show") {
        AddToWE(addProject, title, paths);
      }
      return false;
    } else { // isCopy == "yes"
      break;
    }
  }
  case copyAll: {
    break;
  }
  case recordAll: { // shouldn't happen here but just in case
    return false;
  }
  default: { // shouldn't happen here but just in case
    throw "Invalid isCopy value";
  }
  }

  // make sure we dont overwrite any files
  DirExists(newProjectPath);

  // copy the folder
  CopyFiles(pathToWorkshopProject, newProjectPath);

  // upack the pkg file if there is one
  Unpkg(pathToWorkshopProject, newProjectPath);

  // records the project in WE.txt
  AddToWE(addProject, title, paths);
  return true;
}
void DirExists(const std::string &Directory) {
  if (fs::exists(Directory)) {

    throw("This project's:\n" + Directory +
          "\nDirectory already exists, this program does not override any "
          "directories");
  }
}
void CopyFiles(const std::string &oldPath, const std::string &newPath) {
  // copies files
  fs::copy(oldPath.c_str(), newPath.c_str(),
           std::filesystem::copy_options::overwrite_existing |
               std::filesystem::copy_options::recursive);
}

void Unpkg(const std::string &oldPathProject,
           const std::string &newProjectPath) {
  // gets all the path pkg and zip
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
  // runs the commands
  RunSystemCommands(commandToUnpkg);
  // upacks the .pkg to .zip in the projects directory
  RunSystemCommands(commandToUnzip);
  // upcks the .zip to normal files and folders in the projects directory
  remove(newProjectPath_pkg.c_str());
  remove(newProjectPath_zip.c_str());
  // deletes the .zip and .pkg file as they are no longer needed
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
  std::cout
      << "Choose an option:\n"
         "1. 'skip' to skip search\n"
         "2. 'copy' to copy every project search finds\n"
         "3. 'choose' to choose if to copy or not every project search finds\n"
         "This is case insensitive Search in ENGLISH (bla == BLA)\n";
  std::string option{""};
  while (option != "skip" && option != "copy" && option != "choose") {
    std::getline(std::cin, option);
    IsBadInput();
    if (option == "skip") {
      break;
    } else if (option == "copy") {
      paths.SetIsCopy(copyAll);
      if (!Search(paths)) {
        std::cout << "Project not found\nReturned to search menu\n";
      }
    } else if (option == "choose") {
      paths.SetIsCopy(choose);
      if (!Search(paths)) {
        std::cout << "Project not found or you chose not to copy every time\n"
                     "Returned to search menu\n";
      }
    } else {
      std::cout << "invalid command, please enter again\n";
    }
  }
}

bool Search(WeSession &paths) {
  std::string newProject{""};
  int found{0};
  int copied{0};
  // goes over every project in workshop and then checks title to see if it
  // matches if it does depending on what the user choose before then he can
  // choose what to copy or it copies all
  // if not found and or chose to not copy everything returns false
  std::cout
      << "Enter the name of the wallpaper you want to copy and I'll Search "
         "for it, you can any language you want\n"
         "Just make sure that your terminal supports it\n";

  std::string checkProject;
  std::getline(std::cin, checkProject);
  IsBadInput();
  ToLowerLoop(checkProject);
  bool isFound{false};
  std::string currentLine{""};
  for (const auto &entry : fs::directory_iterator(paths.GetpathToWorkshop())) {
    // gets the the project's path
    std::string pathOfNewProject{entry.path().string()};
    // gets the length and then removes the path except for the project
    // number and inserts it in newProject
    int checkLength{static_cast<int>(pathOfNewProject.length() -
                                     paths.getLengthOfWorkshopPath())};
    newProject = {
        pathOfNewProject.substr(paths.getLengthOfWorkshopPath(), checkLength)};
    std::string tempCheckProject =
        FindTitle(paths.GetpathToWorkshop(), newProject);
    if (CompareTitles(tempCheckProject, checkProject)) {
      ++found;
      // if found a match
      isFound = true;
      if (paths.GetIsCopy() == choose) {
        // if they chose to be able to choose if to copy or not
        if (TransferProject(newProject, paths)) {
          // if it does not exist yet then copy it and
          // add one to number of added proejcts and insert it to list
          // then returns the newly added project
          paths.AddItem(newProject);
          paths.AddProjectsAdded();
          ++copied;
        } else { // if chose to not copy
          isFound = false;
        }
      } else {
        // if chose to copy all
        if (TransferProject(newProject, paths)) {
          paths.AddItem(newProject);
          paths.AddProjectsAdded();
          ++copied;
          // if it does not exist yet then copy it and
          // add one to number of added proejcts and insert it to list
          // then returns the newly added project
        }
      }
    }
  }

  // if did not found and or chose to not copy every option
  std::cout << "Search finished\nCopied projects: " << copied
            << "\nFound projects: " << found << "\n";
  return isFound;
}
void ToLowerLoop(std::string &toLower) {
  for (auto &c : toLower) {
    // Check if the character is an uppercase English letter
    if (c >= 'A' && c <= 'Z') {
      // Convert to lowercase by adding the difference between 'a' and 'A'
      c = c + ('a' - 'A');
    }
  }
}
std::string FindTitle(const std::string &path, const std::string &project) {
  std::string jsonPath{path + "\\" + project + "\\project.json"};
  std::ifstream projectName(jsonPath);
  std::string currentLine("");
  while (std::getline(projectName, currentLine)) {
    if (currentLine.find("title") != std::string::npos) {
      ToLowerLoop(currentLine);
      currentLine = currentLine.substr(12, currentLine.size() - 14);
      currentLine.erase(
          std::remove(currentLine.begin(), currentLine.end(), '\"'),
          currentLine.end()); // removes all /" in case of a " inside the string
      currentLine.erase(
          std::remove(currentLine.begin(), currentLine.end(), '\\'),
          currentLine.end());
      return currentLine;
    }
  }
  // shouldnt happen but just to make sure
  throw "Invalid ending";
}
bool CompareTitles(const std::string &project, const std::string &toProject) {
  return project.find(toProject) != std::string::npos;
}
void IsBadInput() {
  if (std::cin.fail()) {
    throw "Invalid input";
  }
}
void AddToWE(const std::string &project, const std::string &title,
             const WeSession &paths) {
  if (!paths.SearchIfRecorded(project)) {
    std::fstream listWE2("WE.txt", std::ios::app);
    listWE2 << "\n" << project << "-" << title;
    listWE2.close();
  }
  std::cout << "recorded in WE.txt\n";
}