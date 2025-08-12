#include "WeSession.h"

namespace fs = std::filesystem;
void Start(WeSession&);
void WeExists();
bool CopyProject(const std::string&, WeSession&);
void ProcessProjects(WeSession&);
void DirExists(const std::string&);
void UnPKG(const std::string&, const std::string&);
void copyPreviewGif(const std::string&, const std::string&);
void NotCopying(WeSession&, const std::string&, const std::string&);
void RunSystemCommands(const std::string&);
void SearchMenu(WeSession&);
void Search(WeSession&, const char*&);
const char* SearchTitleorDescription();
void ToLowerLoop(std::string&);
TitleAndDescription FindTitleAndDescription(const std::string&,
	const std::string&);
bool CompareTitles(const std::string_view, const std::string_view);

void AddToWE(const std::string&, const std::string&, WeSession&);
void IsBadInput();
// project == wallpaper

int main() {
	try {
		WeSession paths;
		Start(paths);
	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
		return -1;
	}
	std::cout << "\nPress Enter to finish the program";
	std::cin.get();
}

void Start(WeSession& paths) {
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
	if (!fs::exists(strings::WeTXT)) {
		std::cout << "File does not exist, creating file...\n";
		std::ofstream createWe(strings::WeTXT);
		if (!createWe) {
			throw std::runtime_error("Failed to open WE.txt");
		}
		createWe.close();
	}
}

void ProcessProjects(WeSession& paths) {
	// "Main Menu"
	paths.IsCopyCon();
	std::string newProject{ "" };
	for (const auto& entry : fs::directory_iterator(paths.GetPathToWorkshop())) {
		std::string pathOfNewProject{ entry.path().string() };
		int checkLength{ static_cast<int>(pathOfNewProject.length() -
										 paths.getLengthOfWorkshopPath()) };
		newProject = {
			pathOfNewProject.substr(paths.getLengthOfWorkshopPath(), checkLength) };
		// gets the length and then removes the path except for the project ID
		// and inserts it in newProject, this is used to reduce space since the data is stored in `recordedItems` at run time

		if (paths.GetIsCopy() == Action::record) {
			if (!paths.SearchIfRecorded(newProject)) {
				AddToWE(newProject, FindTitleAndDescription(paths.GetPathToWorkshop(), newProject).title, paths);
			}
		}
		else if (!paths.SearchIfRecorded(newProject))
			CopyProject(newProject, paths);
	}
}

bool CopyProject(const std::string& addProject, WeSession& paths) {

	std::string pathToWorkshopProject{ paths.GetPathToWorkshop() + "\\" +
									  addProject };
	std::string title{
		FindTitleAndDescription(paths.GetPathToWorkshop(), addProject).title };
	std::string newProjectPath{ paths.GetPathToMyProjects() + "\\" + title };
	static std::string ifShow{ "" };
	std::string isCopy{ "" };
	switch (paths.GetIsCopy()) {
	case Action::choose: {
		// the title may sometimes be with some strange characters
		// usually beacuse the terminal only shows ascii ones(basic english and
		// numbers) unless you configured it to show all utf-8

		std::cout << "Would you like to copy the wallpaper: " << title
			<< "\nChoose an option:\n"
			<< "1. 'yes' to copy the wallpaper\n"
			<< "2. 'no' to not copy the wallpaper\n";
		std::getline(std::cin, isCopy);
		IsBadInput();
		ToLowerLoop(isCopy);
		while (WeSession::isBasicAnswerInValidOption(isCopy)) {
			std::cout << "Invalid input, please enter again\n";
			std::getline(std::cin, isCopy);
			IsBadInput();
			ToLowerLoop(isCopy);
		}
		if (isCopy == strings::no || isCopy == strings::two) {
			NotCopying(paths, addProject, title);
			return false;
		}
		else
			break;
	}
	case Action::copy: {
		break;
	}
	case Action::record: {
		throw std::invalid_argument("Invalid ending in record path");
	}
	default: {
		throw std::invalid_argument("Invalid IsCopy value");
	}
	}

	DirExists(newProjectPath);

	UnPKG(pathToWorkshopProject, newProjectPath);

	AddToWE(addProject, title, paths);
	paths.AddProjectsAdded();
	return true;
}

void DirExists(const std::string& Directory) {
	if (fs::exists(Directory)) {

		throw std::runtime_error(
			"This project's:\n" + Directory +
			" Directory already exists, this program does not override any "
			"directories");
	}
}

void UnPKG(const std::string& oldPathProject,
	const std::string& newProjectPath) {
	// gets all the paths to pkg and zip
	// then uses commands to copy the pkg, unpack it to a zip then uzip it
	std::string scenePKG = strings::scenePKG;

	if (!fs::exists(oldPathProject + scenePKG)) {
		if (fs::exists(oldPathProject + strings::gifScenePKG)) {
			scenePKG = strings::gifScenePKG;
		}
		else {
			return;
		}
	}
	std::string pathToWorkshopProjectPKG{ oldPathProject + scenePKG };
	std::string commandToUnPKG{ "RePKG.exe extract -c -o \"" + newProjectPath +
							   "\" " + "\"" + pathToWorkshopProjectPKG + "\"" };
	RunSystemCommands(commandToUnPKG);
	copyPreviewGif(
		oldPathProject,
		newProjectPath); // for some reason RePKG doesnt copy preview.gif
}
void copyPreviewGif(const std::string& oldPathProject,
	const std::string& newProjectPath) {
	fs::path source = oldPathProject + strings::previewGif;
	fs::path destination = newProjectPath + strings::previewGif;

	if (fs::exists(source)) {
		fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
	}
}
void NotCopying(WeSession& paths, const std::string& addProject,
	const std::string& title) {
	static std::string ifShow{ "" };

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

void RunSystemCommands(const std::string& command) {
	// creates a child prossesor to run the unzip and upack commands
	// honestly I barely know what is happening in this function
	// but it works
	std::cout << command << " runs\n";
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	int length = MultiByteToWideChar(CP_ACP, 0, command.c_str(), -1, NULL, 0);
	LPWSTR useForCommands = new wchar_t[length];
	MultiByteToWideChar(CP_ACP, 0, command.c_str(), -1, useForCommands, length);

	// Start the child process with CreateProcessW
	if (!CreateProcessW(NULL,           // No module name (use command line)
		useForCommands, // Wide-character command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFOW structure
		&pi)) { // Pointer to PROCESS_INFORMATION structure
		std::cerr << "CreateProcess failed (" << GetLastError() << ")\n";
	}
	else {
		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);
		// Close process and thread handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}
void SearchMenu(WeSession& paths) {
	std::string option{ "" };
	const char* whereToSearch = nullptr;
	std::cout << "Choose an option:\n"
		"1. 'skip' to skip search and go to Main menu\n"
		"2. 'copy' to copy every project search finds\n"
		"3. 'choose' to choose if to copy or not every project search "
		"finds\n"
		"This is case insensitive Search in ENGLISH (bla == BLA)\n";
	while (WeSession::isStartActionInValidOption(option)) {
		std::getline(std::cin, option);
		IsBadInput();
		ToLowerLoop(option);
		if (option == strings::skip || option == strings::one) {
			std::cout << "Returning to Main menu\n";
			break;
		}
		else if (option == strings::copy || option == strings::choose || option == strings::one || option == strings::two) {

			if (option == strings::copy || option == strings::two)
				paths.SetIsCopy(copy);
			else
				paths.SetIsCopy(choose);

			if (!whereToSearch)
				whereToSearch = SearchTitleorDescription();

			Search(paths, whereToSearch);

			std::cout << "Returning to search menu\n";
			option = "";
			std::cout << "Choose an option:\n"
				"1. 'skip' to skip search\n"
				"2. 'copy' to copy every project search finds\n"
				"3. 'choose' to choose if to copy or not every project "
				"search finds\n"
				"This is case insensitive Search in ENGLISH (bla == BLA)\n";
		}
		else {
			std::cout << "invalid command, please enter again\n";
		}
	}
}

void Search(WeSession& paths, const char*& whereToSearch) {
	std::string newProject{ "" };
	TitleAndDescription tmpcheckProject{ "", "" };
	int found{ 0 };
	int copied{ 0 };
	bool matchFound = false;
	std::cout
		<< "Enter the name of the wallpaper you want to copy and I'll Search "
		"for it, you can any language you want\n"
		"Just make sure that your terminal supports it and it is enabled\n";
	std::string checkProject;
	std::getline(std::cin, checkProject);
	IsBadInput();
	ToLowerLoop(checkProject);
	std::string pathToWorkshop = paths.GetPathToWorkshop();
	int lengthOfWorkshopPath = paths.getLengthOfWorkshopPath();
	for (const auto& entry : fs::directory_iterator(pathToWorkshop)) {
		std::string pathOfNewProject{ entry.path().string() };
		int checkLength{
			static_cast<int>(pathOfNewProject.length() - lengthOfWorkshopPath) };
		newProject = { pathOfNewProject.substr(lengthOfWorkshopPath, checkLength) };
		tmpcheckProject = FindTitleAndDescription(pathToWorkshop, newProject);
		ToLowerLoop(tmpcheckProject.title);
		ToLowerLoop(tmpcheckProject.description);
		if (whereToSearch == strings::title) {
			matchFound = CompareTitles(tmpcheckProject.title, checkProject);
		}
		else if (whereToSearch == strings::description) {
			matchFound = CompareTitles(tmpcheckProject.description, checkProject);
		}
		else if (whereToSearch == strings::both) {
			bool cmpTitle = CompareTitles(tmpcheckProject.title, checkProject);
			matchFound = cmpTitle || CompareTitles(tmpcheckProject.description, checkProject);
		}

		if (matchFound) {
			++found;
			if (paths.GetIsCopy() == Action::choose || paths.GetIsCopy() == Action::copy) {
				if (CopyProject(newProject, paths)) {
					++copied;
				}
			}
		}
	}

	std::cout << "Search finished\nProjects found: " << found
		<< "\nProjects copied: " << copied << "\n";
}
const char* SearchTitleorDescription() {
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
	while (TitleAndDescription::isInValidOption(option)) {
		std::cout << "invalid command, please enter again\n";
		std::getline(std::cin, option);
		IsBadInput();
		ToLowerLoop(option);
	}
	if (option == strings::title)
		return strings::title;
	else if (option == strings::description)
		return strings::description;
	else
		return strings::both;
}

void ToLowerLoop(std::string& toLower) {
	// used for case insensitive search
	for (auto& c : toLower) {
		if (c >= 'A' && c <= 'Z') {
			c = c + ('a' - 'A');
		}
	}
}

TitleAndDescription FindTitleAndDescription(const std::string& path,
	const std::string& project) {
	std::string jsonPath{ path + "\\" + project + "\\project.json" };
	std::ifstream projectName(jsonPath);
	if (!projectName) {
		throw std::runtime_error("Unable to open file: " + jsonPath);
	}

	std::string currentLine{ "" };
	std::string title = { "" };
	std::string description = { "" };

	while (std::getline(projectName, currentLine)) {
		if (currentLine.find("\"description\" :") != std::string::npos ||
			currentLine.find("\"title\" :") != std::string::npos) {
			// Removes all double quotes
			currentLine.erase(
				std::remove(currentLine.begin(), currentLine.end(), '\"'),
				currentLine.end());

			// Removes all backslashes
			currentLine.erase(
				std::remove(currentLine.begin(), currentLine.end(), '\\'),
				currentLine.end());

			// Removes "description :" or "title :"
			size_t colonPos = currentLine.find(":");
			if (colonPos == std::string::npos) {
				throw std::invalid_argument("Invalid char missing: Missing ':' in " +
					jsonPath);
				// shouldn't happen but just in case they change something
			}
			if (currentLine.find("description :") != std::string::npos) {
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
			else {
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
				TitleAndDescription temp(title, description);
				return temp;
			}
		}
	}
	throw std::invalid_argument("Invalid Ending: Title not found in " + jsonPath);
}

bool CompareTitles(std::string_view project, std::string_view toProject) {
	return project.find(toProject) != std::string_view::npos;
}
void IsBadInput() {
	if (std::cin.fail()) {
		throw std::invalid_argument("Invalid input");
	}
}
void AddToWE(const std::string& project, const std::string& title,
	WeSession& paths) {

	if (paths.AddItem(project)) {
		std::fstream addToWe(strings::WeTXT, std::ios::app);
		if (!addToWe) {
			throw std::runtime_error("Failed to open WE.txt");
		}
		addToWe << "\n" << project << " - " << title;
		addToWe.close();
	}
}