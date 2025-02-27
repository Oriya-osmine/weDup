#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

extern const char *const defaultProjectsLocation;
extern const char *const defaultWorkshopocation;
extern const char *const projectsTXT;
extern const char *const workshopTXT;
extern const char *const WeTXT;
extern const char *const zip2pkgBAT;
extern const char *const pkg2zipBAT;
extern const char *const pkg2zipEXE;
extern void ToLowerLoop(std::string &toLower);
extern void IsBadInput();
enum CopyTypes { choose, copy, record, skip };
enum answer { no, yes, alwaysNo, alwaysYes };

class WeSession {
private:
  std::string pathToWorkshop{""};
  std::string pathToMyProjects{""};
  // To check if the project already exists, much faster than reading a
  // file every time
  std::vector<std::string> recordedItems;
  // To only use the number of the project instead of the whole path
  int lengthOfWorkshopPath;
  // To see the number of projects added to WE
  int projectsAdded{0};
  // To decide what do to when found a new project
  CopyTypes isCopy = copy;

private:
  void PutintoSet();
  void CheckPaths(const char *, const char *);
  void Writepaths(const char *, const char *);
  void setPathsFromUser(const char *);
  void CheckPKG2ZIP();

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
  void AddItem(const std::string &);
  bool SearchIfRecorded(const std::string &) const;
};

struct TitleAndDescription {
	std::string title;
	std::string description;
	TitleAndDescription(std::string t, std::string d) : title(t), description(d) {}
};