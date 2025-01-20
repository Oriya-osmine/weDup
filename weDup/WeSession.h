#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <windows.h>

extern const char *defaultProjectsLocation;
extern const char *defaultWorkshopocation;
extern const char *projects_txt;
extern const char *workshop_txt;
extern const char *WE_txt;
extern const char *zip2pkg_bat;
extern const char *pkg2zip_bat;
extern const char *pkg2zip_exe;
extern void ToLowerLoop(std::string& toLower);
void IsBadInput();
enum CopyTypes { choose, copyAll, recordAll, skip };

class WeSession {
private:
  std::string pathToWorkshop{""};
  std::string pathToMyProjects{""};
  // To check if the project already exists, much faster than reading a
  // file every time
  std::set<std::string> recordedItems;
  // To only use the number of the project instead of the whole path
  int lengthOfWorkshopPath;
  // To see the number of projects added to WE
  int projectsAdded{0};
  // To decide what do to when found a new project
  CopyTypes isCopy = copyAll;

private:
  void PutintoSet();
  void CheckPaths(const char *, const char *);
  void Writepaths(const char *, const char *);
  void setPathsFromUser(const char *);
  void Check_pkg2zip();

public:
  WeSession();
  void IsCopyCon();
  std::string GetpathToWorkshop() const;
  std::string GetPathToMyProjects() const;
  void SetIsCopy(const CopyTypes &);
  int GetIsCopy() const;
  int GetProjectsAdded() const;
  void subtractprojectsAdded();
  void AddProjectsAdded();
  int getLengthOfWorkshopPath() const;
  void AddItem(const std::string&);
  bool SearchIfRecorded(const std::string &) const;
};