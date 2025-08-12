#include "WeSession.h"
namespace fs = std::filesystem;


WeSession::WeSession() {
	CheckPaths(strings::defaultProjectsLocation, strings::projectsTXT);
	CheckPaths(strings::defaultWorkshopocation, strings::workshopTXT);
	lengthOfWorkshopPath = (static_cast<int>(pathToWorkshop.length()) + 1);
	ReserveOrPutInVector(true);
	CheckRePKG();
}
void WeSession::ReserveOrPutInVector(bool RorP) {
	if (!fs::exists(strings::WeTXT)) {
		return;
	}
	std::fstream WeFile(strings::WeTXT);
	std::string currentLine{ "" };
	int i{ 0 };
	if (!WeFile) {
		throw std::runtime_error("Failed to open WE.txt");
	}
	if (!RorP) {
		while (std::getline(WeFile, currentLine)) {
			AddItem(currentLine);
		}
	}
	else {
		while (std::getline(WeFile, currentLine)) {
			++i;
		}
		recordedItems.reserve(i);
		ReserveOrPutInVector(false);
	}
	WeFile.close();
}

void WeSession::CheckPaths(const char* const path, const char* const pathTXT) {

	if (!fs::exists(pathTXT)) {
		std::cout << "file does not exist, creating file " << pathTXT << "...\n";

		if (std::filesystem::exists(path))
			Writepaths(path, pathTXT);
		else {
			setPathsFromUser(pathTXT);
		}
	}
	else {
		std::fstream WePaths(pathTXT, std::ios::in);
		if (!WePaths) {
			throw std::runtime_error("Failed to open " + std::string(pathTXT));
		}
		pathTXT == strings::projectsTXT
			? pathToMyProjects.assign(std::istreambuf_iterator<char>(WePaths),
				std::istreambuf_iterator<char>())
			: pathToWorkshop.assign(std::istreambuf_iterator<char>(WePaths),
				std::istreambuf_iterator<char>());
		WePaths.close();
	}
}
void WeSession::Writepaths(const char* const path, const char* const pathTXT) {
	std::string MpOrWs = pathTXT == strings::projectsTXT ? "Myprojects" : "Workshop";
	std::string DefaultMporWslocation =
		pathTXT == strings::projectsTXT ? strings::defaultProjectsLocation : strings::defaultWorkshopocation;

	std::cout << "We have detected your" << MpOrWs << " content location is in\n"
		<< DefaultMporWslocation << "\nChoose an option:\n"
		<< "1. 'yes' if correct \n"
		<< "2. 'no' if wrong, allows you to enter the path yourself\n";

	std::string option{ "" };
	while (isBasicAnswerInValidOption(option)) {
		std::getline(std::cin, option);
		IsBadInput();
		ToLowerLoop(option);
		if (option == strings::no || option == strings::two) {
			setPathsFromUser(pathTXT);
			return;
		}
		else if (option == strings::yes || option == strings::one) {
			std::ofstream WePaths(pathTXT, std::ios::out);
			if (!WePaths) {
				throw std::runtime_error("Failed to open " + std::string(pathTXT));
			}
			WePaths << path;
			WePaths.close();
			CheckPaths(path, pathTXT);
		}
		else {
			std::cout << "invalid command, please enter again\n";
		}
	}
}
void WeSession::setPathsFromUser(const char* const pathTXT) {
	// gets the path and puts it in PathTo(Myprojects/Workshop).txt
	if (pathTXT == strings::projectsTXT) {
		std::cout
			<< "Enter your projects steam path \nshould look something like "
			"this:\n"
			<< strings::defaultProjectsLocation
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
	}
	else if (pathTXT == strings::workshopTXT) {
		std::cout
			<< "Enter your workshop path it should look something like "
			"this:\n"
			<< strings::defaultWorkshopocation
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
	}
	else {
		throw std::invalid_argument("Invalid txt when writing");
	}
}

void WeSession::CheckRePKG() {
	if (!fs::exists(strings::RePKGEXE))
		throw std::runtime_error((std::string)strings::RePKGEXE + "doesn't exist");
}


bool  WeSession::AddItem(const std::string& addProject) {
	if (SearchIfRecorded(addProject))
		return false;
	recordedItems.emplace_back(addProject);
	return true;
}
void WeSession::IsCopyCon() {
	std::cout << "Choose an option:\n"
		<< "1. 'choose' to manually select what new projects to copy\n"
		<< "2. 'copy' to copy every new project\n"
		<< "3. 'record' to record all new projects\n";
	std::string option{ "" };
	while (isBasicActionInValidOption(option)) {
		std::getline(std::cin, option);
		IsBadInput();
		ToLowerLoop(option);
		if (option == strings::choose || option == strings::one) {
			isCopy = Action::choose;
			break;
		}
		else if (option == strings::copy || option == strings::two) {
			isCopy = Action::copy;
			break;
		}
		else if (option == strings::record || option == strings::three) {
			isCopy = Action::record;
			break;
		}
		std::cout << "Invalid option, please try again.\n";
	}
}

bool WeSession::SearchIfRecorded(const std::string& find) const {
	return std::any_of(recordedItems.begin(), recordedItems.end(),
		[&](const std::string& item) {
			return item.find(find) != std::string::npos;
		});
}

