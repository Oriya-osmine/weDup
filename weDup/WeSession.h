#pragma once
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
namespace strings {
	// paths
	const char* const defaultProjectsLocation =
		"C:\\Program Files "
		"(x86)\\Steam\\SteamApps\\common\\wallpaper_engine\\projects\\myprojects";
	const char* const defaultWorkshopocation =
		"C:\\Program Files (x86)\\Steam\\SteamApps\\workshop\\content\\431960";
	const char* const projectsTXT = "PathToMyprojects.txt";
	const char* const workshopTXT = "PathToWorkshop.txt";
	const char* const WeTXT = "WE.txt";
	const char* const RePKGEXE = "RePKG.exe";
	// Actions
	const char* const choose = "choose";
	const char* const copy = "copy";
	const char* const record = "record";
	const char* const skip = "skip";
	// Answers
	const char* const yes = "yes";
	const char* const no = "no";
	const char* const alwaysNo = "alwaysNo";
	const char* const alwaysYes = "alwaysYes";
	// Numbers
	const char* const one = "1";
	const char* const two = "2";
	const char* const three = "3";
	const char* const four = "4";
	// search
	const char* const title = "title";
	const char* const description = "description";
	const char* const both = "both";
	// wallpaper engine
	const char* const scenePKG = "\\scene.pkg";
	const char* const gifScenePKG = "\\gifscene.pkg";
	const char* const previewGif = "\\preview.gif";
}
extern void ToLowerLoop(std::string& toLower);
extern void IsBadInput();
enum Action { choose, copy, record, skip };
enum answer { no, yes, alwaysNo, alwaysYes };


class WeSession {
private:
	// Most valid actions and answers (excepts description related options)
	inline static const std::array<const char* const, 6> validStartActions = {
		strings::choose, strings::copy, strings::skip, strings::one, strings::two, strings::three
	};
	inline static const std::array<const char* const, 6> validBasicActions = {
		strings::choose, strings::copy, strings::record, strings::one, strings::two, strings::three
	};
	inline static const std::array<const char* const, 4> validBasicAnswers = {
		strings::yes, strings::no, strings::one, strings::two
	};
	inline static const std::array<const char* const, 8> validAnswers = {
		strings::yes, strings::no, strings::alwaysNo, strings::alwaysYes, strings::one, strings::two, strings::three, strings::four
	};
	// The paths to the workshop and "my projects"
	std::string pathToWorkshop;
	std::string pathToMyProjects;
	// To check if the project already exists, much faster than reading a
	// file every time
	std::vector<std::string> recordedItems;
	// To only use the number of the project instead of the whole path
	int lengthOfWorkshopPath;
	// To see the number of projects added to WE
	int projectsAdded{ 0 };
	// To decide what do to when found a new project
	Action isCopy = copy;

private:
	void ReserveOrPutInVector(bool);
	void CheckPaths(const char*, const char*);
	void Writepaths(const char*, const char*);
	void setPathsFromUser(const char*);
	void CheckRePKG();

public:
	WeSession();
	void IsCopyCon();
	std::string GetPathToWorkshop() const { return pathToWorkshop; }
	std::string GetPathToMyProjects() const { return pathToMyProjects; }
	void SetIsCopy(const Action& set) { isCopy = set; }
	int GetIsCopy() const { return isCopy; }
	int GetProjectsAdded() const { return projectsAdded; }
	void subtractprojectsAdded() { --projectsAdded; }
	void AddProjectsAdded() { ++projectsAdded; }
	int getLengthOfWorkshopPath() const { return lengthOfWorkshopPath; }
	bool AddItem(const std::string&);
	bool SearchIfRecorded(const std::string&) const;
	static bool isBasicAnswerInValidOption(const std::string& option) {
		return std::find(validBasicAnswers.begin(), validBasicAnswers.end(), option) == validBasicAnswers.end();
	}
	static bool isAnswerInValidOption(const std::string& option) {
		return std::find(validAnswers.begin(), validAnswers.end(), option) == validAnswers.end();
	}
	static bool isStartActionInValidOption(const std::string& option) {
		return std::find(validStartActions.begin(), validStartActions.end(), option) == validStartActions.end();
	}
	static bool isBasicActionInValidOption(const std::string& option) {
		return std::find(validBasicActions.begin(), validBasicActions.end(), option) == validBasicActions.end();
	}
};

struct TitleAndDescription {
	inline static const std::array<const char*, 6> validOptions = {
		strings::title, strings::description, strings::both, strings::one, strings::two, strings::three
	};
	std::string title;
	std::string description;
	TitleAndDescription(std::string t, std::string d)
		: title(t), description(d) {
	}
	static bool isInValidOption(const std::string& option) {
		return std::find(validOptions.begin(), validOptions.end(), option) == validOptions.end();
	}
};