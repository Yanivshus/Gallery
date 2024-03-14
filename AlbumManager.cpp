#include "AlbumManager.h"
#include <iostream>
#include "Constants.h"
#include "MyException.h"
#include "AlbumNotOpenException.h"




AlbumManager::AlbumManager(IDataAccess& dataAccess) :
    m_dataAccess(dataAccess), m_nextPictureId(100), m_nextUserId(200)
{
	// Left empty
	m_dataAccess.open();
}

void AlbumManager::executeCommand(CommandType command) {
	try {
		AlbumManager::handler_func_t handler = m_commands.at(command);
		(this->*handler)();
	} catch (const std::out_of_range&) {
			throw MyException("Error: Invalid command[" + std::to_string(command) + "]\n");
	}
}

void AlbumManager::printHelp() const
{
	std::cout << "Supported Album commands:" << std::endl;
	std::cout << "*************************" << std::endl;
	
	for (const struct CommandGroup& group : m_prompts) {
		std::cout << group.title << std::endl;
		std::string space(".  ");
		for (const struct CommandPrompt& command : group.commands) {
			space = command.type < 10 ? ".   " : ".  ";

			std::cout << command.type << space << command.prompt << std::endl;
		}
		std::cout << std::endl;
	}
}


// ******************* Album ******************* 
void AlbumManager::createAlbum()
{
	std::string userIdStr = getInputFromConsole("Enter user id: ");
	int userId = std::stoi(userIdStr);
	if ( !m_dataAccess.doesUserExists(userId) ) {
		throw MyException("Error: Can't create album since there is no user with id [" + userIdStr+"]\n");
	}

	std::string name = getInputFromConsole("Enter album name - ");
	if ( m_dataAccess.doesAlbumExists(name,userId) ) {
		throw MyException("Error: Failed to create album, album with the same name already exists\n");
	}

	Album newAlbum(userId,name);
	m_dataAccess.createAlbum(newAlbum);

	std::cout << "Album [" << newAlbum.getName() << "] created successfully by user@" << newAlbum.getOwnerId() << std::endl;
}

void AlbumManager::openAlbum()
{
	if (isCurrentAlbumSet()) {
		closeAlbum();
	}

	std::string userIdStr = getInputFromConsole("Enter user id: ");
	int userId = std::stoi(userIdStr);
	if ( !m_dataAccess.doesUserExists(userId) ) {
		throw MyException("Error: Can't open album since there is no user with id @" + userIdStr + ".\n");
	}

	std::string name = getInputFromConsole("Enter album name - ");
	if ( !m_dataAccess.doesAlbumExists(name, userId) ) {
		throw MyException("Error: Failed to open album, since there is no album with name:"+name +".\n");
	}

	m_openAlbum = m_dataAccess.openAlbum(name);
    m_currentAlbumName = name;
	// success
	std::cout << "Album [" << name << "] opened successfully." << std::endl;
}

void AlbumManager::closeAlbum()
{
	refreshOpenAlbum();

	std::cout << "Album [" << m_openAlbum.getName() << "] closed successfully." << std::endl;
	m_dataAccess.closeAlbum(m_openAlbum);
	m_currentAlbumName = "";
}

void AlbumManager::deleteAlbum()
{
	std::string userIdStr = getInputFromConsole("Enter user id: ");
	int userId = std::stoi(userIdStr);
	if (!m_dataAccess.doesUserExists(userId)) {
		throw MyException("Error: There is no user with id @" + userIdStr +"\n");
	}

	std::string albumName = getInputFromConsole("Enter album name - ");
	if ( !m_dataAccess.doesAlbumExists(albumName, userId) ) {
		throw MyException("Error: Failed to delete album, since there is no album with name:" + albumName + ".\n");
	}

	// album exist, close album if it is opened
	if ( (isCurrentAlbumSet() ) &&
		 (m_openAlbum.getOwnerId() == userId && m_openAlbum.getName() == albumName) ) {

		closeAlbum();
	}

	m_dataAccess.deleteAlbum(albumName, userId);
	std::cout << "Album [" << albumName << "] @"<< userId <<" deleted successfully." << std::endl;
}

void AlbumManager::listAlbums()
{
	m_dataAccess.printAlbums();
}

void AlbumManager::listAlbumsOfUser()
{
	std::string userIdStr = getInputFromConsole("Enter user id: ");
	int userId = std::stoi(userIdStr);
	if (!m_dataAccess.doesUserExists(userId)) {
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");
	}

	const User& user = m_dataAccess.getUser(userId);
	const std::list<Album>& albums = m_dataAccess.getAlbumsOfUser(user);

	std::cout << "Albums list of user@" << user.getId() << ":" << std::endl;
	std::cout << "-----------------------" << std::endl;

	for (const auto& album : albums) {
		std::cout <<"   + [" << album.getName() <<"] - created on "<< album.getCreationDate() << std::endl;
	}
}


// ******************* Picture ******************* 
void AlbumManager::addPictureToAlbum()
{
	refreshOpenAlbum();

	std::string picName = getInputFromConsole("Enter picture name: ");
	if (m_openAlbum.doesPictureExists(picName) ) {
		throw MyException("Error: Failed to add picture, picture with the same name already exists.\n");
	}
	
	Picture picture(++m_nextPictureId, picName);
	std::string picPath = getInputFromConsole("Enter picture path: ");
	picture.setPath(picPath);

	m_dataAccess.addPictureToAlbumByName(m_openAlbum.getName(), picture);

	std::cout << "Picture [" << picture.getId() << "] successfully added to Album [" << m_openAlbum.getName() << "]." << std::endl;
}

void AlbumManager::removePictureFromAlbum()
{
	refreshOpenAlbum();

	std::string picName = getInputFromConsole("Enter picture name: ");
	if ( !m_openAlbum.doesPictureExists(picName) ) {
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");
	}
	
	auto picture = m_openAlbum.getPicture(picName);
	m_dataAccess.removePictureFromAlbumByName(m_openAlbum.getName(), picture.getName());
	std::cout << "Picture <" << picName << "> successfully removed from Album [" << m_openAlbum.getName() << "]." << std::endl;
}

void AlbumManager::listPicturesInAlbum()
{
	refreshOpenAlbum();

	std::cout << "List of pictures in Album [" << m_openAlbum.getName() 
			  << "] of user@" << m_openAlbum.getOwnerId() <<":" << std::endl;
	
	const std::list<Picture>& albumPictures = m_openAlbum.getPictures();
	for (auto iter = albumPictures.begin(); iter != albumPictures.end(); ++iter) {
		std::cout << "   + Picture [" << iter->getId() << "] - " << iter->getName() << 
			"\tLocation: [" << iter->getPath() << "]\tCreation Date: [" <<
				iter->getCreationDate() << "]\tTags: [" << iter->getTagsCount() << "]" << std::endl;
	}
	std::cout << std::endl;
}

FILETIME AlbumManager::getLastModOfFile(const std::string& filePath)
{
	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	if (GetFileAttributesEx(filePath.c_str(), GetFileExInfoStandard, &fileInfo)) 
	{
		return fileInfo.ftLastWriteTime;
	}
	else {
		return FILETIME();
	}

}


PROCESS_INFORMATION pi = { 0 };

/// <summary>
/// global function to catch handler of ctrl c, if ctrl c was caught it will terminate the process.
/// </summary>
/// <param name="fdwCtrlType:"> the ctrl type entered, ctrl c/b and so on </param>
/// <returns></returns>
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		TerminateProcess(pi.hProcess, 0);//if detected a ctrl c handker we will close the process.
		return TRUE;
	default:
		return FALSE;
	}
}

void AlbumManager::showPicture()
{
	refreshOpenAlbum();

	std::string picName = getInputFromConsole("Enter picture name: ");
	if ( !m_openAlbum.doesPictureExists(picName) ) {
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");
	}
	
	auto pic = m_openAlbum.getPicture(picName);
	if ( !fileExistsOnDisk(pic.getPath()) ) {
		throw MyException("Error: Can't open <" + picName+ "> since it doesnt exist on disk.\n");
	}

	std::string choice = "";
	bool flag = false;
	//running untill the user picks a program.
	while (flag == false) {
		std::cout << "	Enter with which program you want to open the picture (p-mspaint, i-irfanview): ";
		std::cin >> choice;
		//if the user entered p or i the loop will stop.
		if (choice == "i" || choice == "p") {
			flag = true;
		}
	}

	//get the time of change before opening the pic.
	FILETIME before = getLastModOfFile(pic.getPath());

	std::string exePath = "";
	LPCSTR lp = "";
	std::string imagePath = "";
	std::string cmdLine = "";
	LPSTR cmdLinePtr = "";

	//checking which program the user chose.
	if (choice == "p") {
		exePath = "C:\\Windows\\System32\\mspaint.exe";
		lp = exePath.c_str();

		cmdLine += "/p " + pic.getPath();
		cmdLinePtr = const_cast<LPSTR>(cmdLine.c_str());
	}
	else if (choice == "i") {
		exePath = "C:\\IrfanView\\i_view32.exe";
		lp = NULL;

		//creating the path of the irfanview picture.
		cmdLine += exePath + " \"" + pic.getPath() + " \"";
		cmdLinePtr = const_cast<LPSTR>(cmdLine.c_str());
		
	}

	//nescessry veriables to create a process.
	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = true;

	BOOL ret = CreateProcess(lp, cmdLinePtr, NULL, NULL, false, 0, NULL, NULL, &si, &pi);//start the proscess.

	//if the process couldnt be created we will print the error and wait a bit so the user could read it.
	if (ret == FALSE) {
		std::cout << "CreateProcess failed .\n" << GetLastError() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));
		throw MyException("Create process error!");
		
	}
	

	if (SetConsoleCtrlHandler(CtrlHandler, true)) {};
	WaitForSingleObject(pi.hProcess, -1);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	//get the time of change before opening the pic.
	FILETIME end = getLastModOfFile(pic.getPath());

	//bonus- check if file changed.
	if (before.dwLowDateTime != end.dwLowDateTime) {
		std::cout << "The file has changed" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(6));
	}
	
}

void AlbumManager::tagUserInPicture()
{
	refreshOpenAlbum();

	std::string picName = getInputFromConsole("Enter picture name: ");
	if ( !m_openAlbum.doesPictureExists(picName) ) {
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");
	}
	
	Picture pic = m_openAlbum.getPicture(picName);
	
	std::string userIdStr = getInputFromConsole("Enter user id to tag: ");
	int userId = std::stoi(userIdStr);
	if ( !m_dataAccess.doesUserExists(userId) ) {
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");
	}
	User user = m_dataAccess.getUser(userId);

	m_dataAccess.tagUserInPicture(m_openAlbum.getName(), pic.getName(), user.getId());
	std::cout << "User @" << userIdStr << " successfully tagged in picture <" << pic.getName() << "> in album [" << m_openAlbum.getName() << "]" << std::endl;
}

void AlbumManager::untagUserInPicture()
{
	refreshOpenAlbum();

	std::string picName = getInputFromConsole("Enter picture name: ");
	if (!m_openAlbum.doesPictureExists(picName)) {
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");
	}

	Picture pic = m_openAlbum.getPicture(picName);

	std::string userIdStr = getInputFromConsole("Enter user id: ");
	int userId = stoi(userIdStr);
	if (!m_dataAccess.doesUserExists(userId)) {
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");
	}
	User user = m_dataAccess.getUser(userId);

	if (! pic.isUserTagged(user)) {
		throw MyException("Error: The user was not tagged! \n");
	}

	m_dataAccess.untagUserInPicture(m_openAlbum.getName(), pic.getName(), user.getId());
	std::cout << "User @" << userIdStr << " successfully untagged in picture <" << pic.getName() << "> in album [" << m_openAlbum.getName() << "]" << std::endl;

}

void AlbumManager::listUserTags()
{
	refreshOpenAlbum();

	std::string picName = getInputFromConsole("Enter picture name: ");
	if ( !m_openAlbum.doesPictureExists(picName) ) {
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");
	}
	auto pic = m_openAlbum.getPicture(picName); 

	const std::set<int> users = pic.getUserTags();

	if ( 0 == users.size() )  {
		throw MyException("Error: There is no user tegged in <" + picName + ">.\n");
	}

	std::cout << "Tagged users in picture <" << picName << ">:" << std::endl;
	for (const int user_id: users) {
		const User user = m_dataAccess.getUser(user_id);
		std::cout << user << std::endl;
	}
	std::cout << std::endl;

}


// ******************* User ******************* 
void AlbumManager::addUser()
{
	std::string name = getInputFromConsole("Enter user name: ");

	User user(++m_nextUserId,name);
	
	m_dataAccess.createUser(user);
	std::cout << "User " << name << " with id @" << user.getId() << " created successfully." << std::endl;
}


void AlbumManager::removeUser()
{
	// get user name
	std::string userIdStr = getInputFromConsole("Enter user id: ");
	int userId = std::stoi(userIdStr);
	if ( !m_dataAccess.doesUserExists(userId) ) {
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");
	}
	const User& user = m_dataAccess.getUser(userId);
	if (isCurrentAlbumSet() && userId == m_openAlbum.getOwnerId()) {
		closeAlbum();
	}

	m_dataAccess.deleteUser(user);
	std::cout << "User @" << userId << " deleted successfully." << std::endl;
}

void AlbumManager::listUsers()
{
	m_dataAccess.printUsers();	
}

void AlbumManager::userStatistics()
{
	std::string userIdStr = getInputFromConsole("Enter user id: ");
	int userId = std::stoi(userIdStr);
	if ( !m_dataAccess.doesUserExists(userId) ) {
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");
	}

	const User& user = m_dataAccess.getUser(userId);

	std::cout << "user @" << userId << " Statistics:" << std::endl << "--------------------" << std::endl <<
		"  + Count of Albums Tagged: " << m_dataAccess.countAlbumsTaggedOfUser(user) << std::endl <<
		"  + Count of Tags: " << m_dataAccess.countTagsOfUser(user) << std::endl <<
		"  + Avarage Tags per Alboum: " << m_dataAccess.averageTagsPerAlbumOfUser(user) << std::endl <<
		"  + Count of owned albums: " << m_dataAccess.countAlbumsOwnedOfUser(user) << std::endl;
}


// ******************* Queries ******************* 
void AlbumManager::topTaggedUser()
{
	const User& user = m_dataAccess.getTopTaggedUser();

	std::cout << "The top tagged user is: " << user.getName() << std::endl;
}

void AlbumManager::topTaggedPicture()
{
	const Picture& picture = m_dataAccess.getTopTaggedPicture();

	std::cout << "The top tagged picture is: " << picture.getName() << std::endl;
}

void AlbumManager::picturesTaggedUser()
{
	std::string userIdStr = getInputFromConsole("Enter user id: ");
	int userId = std::stoi(userIdStr);
	if ( !m_dataAccess.doesUserExists(userId) ) {
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");
	}

	auto user = m_dataAccess.getUser(userId);

	auto taggedPictures = m_dataAccess.getTaggedPicturesOfUser(user);

	std::cout << "List of pictures that User@" << user.getId() << " tagged :" << std::endl;
	for (const Picture& picture: taggedPictures) {
		std::cout <<"   + "<< picture << std::endl;
	}
	std::cout << std::endl;
}


// ******************* Help & exit ******************* 
void AlbumManager::exit()
{
	std::exit(EXIT_SUCCESS);
}

void AlbumManager::createPicCopy()
{
	refreshOpenAlbum();

	std::string picName = getInputFromConsole("Enter picture name: ");
	if (!m_openAlbum.doesPictureExists(picName)) {
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");
	}

	auto pic = m_openAlbum.getPicture(picName);
	if (!fileExistsOnDisk(pic.getPath())) {
		throw MyException("Error: Can't open <" + picName + "> since it doesnt exist on disk.\n");
	}

	std::string path = pic.getPath();
	std::string newPath = "";

	// Find the position of the last occurrence of '/'
	size_t pos = path.find_last_of('/');
	if (pos != std::string::npos) {

		// Extract the directory path and the filename
		std::string directory = path.substr(0, pos + 1); // Include the '/' in the directory
		std::string filename = path.substr(pos + 1); // Exclude the '/'

		// Insert "copyOf_" before the filename
		std::string newFilename = "copyOf_" + filename;

		// Construct the new path
		newPath = directory + newFilename;
	}
	else 
	{
		newPath = "copyOf_" + path;
	}

	
	//add the picture to the data base.
	Picture copyPic(-1, "copyOf_" + pic.getName(), newPath, pic.getCreationDate());
	m_dataAccess.addPictureToAlbumByName(m_openAlbum.getName(), copyPic);

	std::ifstream inputfile(pic.getPath(), std::ios::binary);
	
	// Open the output image file
	std::ofstream outputFile(newPath, std::ios::binary);

	if (!outputFile.is_open()) {
		std::cerr << "Error: Unable to open output image file." << std::endl;
	}

	//insert content of file to another.
	outputFile << inputfile.rdbuf();

	// Close the file streams
	inputfile.close();
	outputFile.close();


	std::cout << "Image copied successfully." << std::endl;

}

void AlbumManager::changeFilePremissions()
{
	//get the picture path from user.
	std::string picName = getInputFromConsole("Enter picture path: ");
	if (!fileExistsOnDisk(picName)) {
		throw MyException("Error: Can't open <" + picName + "> since it doesnt exist on disk.\n");
	}

	//get the picture attributes.
	DWORD attributes = GetFileAttributesA(picName.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		DWORD error = GetLastError();
		throw MyException("failed to get file attributes.");
	}

	// run a loop to determine if the user want to make the picture readonly - 1.
	// or make the picture writeable - 0.
	std::string choice = "";
	while (choice != "1" && choice != "0") {
		std::cout << "	Enter 1 if you wan't to make the file readonly or 0 to make it writable: ";
		std::cin >> choice;
	}

	// if the user chose 1
	if (choice == "1")
	{
		//we will check if the picture is already set to readonly.
		if (attributes & FILE_ATTRIBUTE_READONLY)
		{
			std::cout << "file already readonly" << std::endl;
		}
		else // if not i will set it to readonly.
		{
			if (!SetFileAttributesA(picName.c_str(), FILE_ATTRIBUTE_READONLY))
			{
				DWORD error = GetLastError();
				std::cerr << "Failed to set file attributes: " << error << std::endl;
				throw MyException("failed to set file attributes.");
			}
		}
	}
	else if (choice == "0")
	{
		if (attributes & FILE_ATTRIBUTE_READONLY) // if the picture is readonly we will delete the readonly attribue and set it.
		{
			attributes &= ~FILE_ATTRIBUTE_READONLY;
			if (!SetFileAttributesA(picName.c_str(), attributes)) 
			{
				DWORD error = GetLastError();
				std::cerr << "Failed to set file attributes: " << error << std::endl;
				throw MyException("failed to set file attributes.");
			}
			else 
			{
				std::cout << "file already writeable" << std::endl;
			}
		}
	}
}


void AlbumManager::help()
{
	system("CLS");
	printHelp();
}



std::string AlbumManager::getInputFromConsole(const std::string& message)
{
	std::string input;
	do {
		std::cout << message;
		std::getline(std::cin, input);
	} while (input.empty());
	
	return input;
}

bool AlbumManager::fileExistsOnDisk(const std::string& filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0); 
}

void AlbumManager::refreshOpenAlbum() {
	if (!isCurrentAlbumSet()) {
		throw AlbumNotOpenException();
	}
    m_openAlbum = m_dataAccess.openAlbum(m_currentAlbumName);
}

bool AlbumManager::isCurrentAlbumSet() const
{
    return !m_currentAlbumName.empty();
}

const std::vector<struct CommandGroup> AlbumManager::m_prompts  = {
	{
		"Supported Albums Operations:\n----------------------------",
		{
			{ CREATE_ALBUM        , "Create album" },
			{ OPEN_ALBUM          , "Open album" },
			{ CLOSE_ALBUM         , "Close album" },
			{ DELETE_ALBUM        , "Delete album" },
			{ LIST_ALBUMS         , "List albums" },
			{ LIST_ALBUMS_OF_USER , "List albums of user" }
		}
	},
	{
		"Supported Album commands (when specific album is open):",
		{
			{ ADD_PICTURE    , "Add picture." },
			{ REMOVE_PICTURE , "Remove picture." },
			{ SHOW_PICTURE   , "Show picture." },
			{ LIST_PICTURES  , "List pictures." },
			{ TAG_USER		 , "Tag user." },
			{ UNTAG_USER	 , "Untag user." },
			{ LIST_TAGS		 , "List tags." }
		}
	},
	{
		"Supported Users commands: ",
		{
			{ ADD_USER         , "Add user." },
			{ REMOVE_USER      , "Remove user." },
			{ LIST_OF_USER     , "List of users." },
			{ USER_STATISTICS  , "User statistics." },
		}
	},
	{
		"Supported Queries:",
		{
			{ TOP_TAGGED_USER      , "Top tagged user." },
			{ TOP_TAGGED_PICTURE   , "Top tagged picture." },
			{ PICTURES_TAGGED_USER , "Pictures tagged user." },
		}
	},
	{
		"Supported Operations:",
		{
			{COPY_PICTURE, "Create a copy of picture in album."},
			{CHANGE_PREM, "Change file to readonly or backwards."},
			{ HELP , "Help (clean screen)" },
			{ EXIT , "Exit." },
		}
	}
};

const std::map<CommandType, AlbumManager::handler_func_t> AlbumManager::m_commands = {
	{ CREATE_ALBUM, &AlbumManager::createAlbum },
	{ OPEN_ALBUM, &AlbumManager::openAlbum },
	{ CLOSE_ALBUM, &AlbumManager::closeAlbum },
	{ DELETE_ALBUM, &AlbumManager::deleteAlbum },
	{ LIST_ALBUMS, &AlbumManager::listAlbums },
	{ LIST_ALBUMS_OF_USER, &AlbumManager::listAlbumsOfUser },
	{ ADD_PICTURE, &AlbumManager::addPictureToAlbum },
	{ REMOVE_PICTURE, &AlbumManager::removePictureFromAlbum },
	{ LIST_PICTURES, &AlbumManager::listPicturesInAlbum },
	{ SHOW_PICTURE, &AlbumManager::showPicture },
	{ TAG_USER, &AlbumManager::tagUserInPicture, },
	{ UNTAG_USER, &AlbumManager::untagUserInPicture },
	{ LIST_TAGS, &AlbumManager::listUserTags },
	{ ADD_USER, &AlbumManager::addUser },
	{ REMOVE_USER, &AlbumManager::removeUser },
	{ LIST_OF_USER, &AlbumManager::listUsers },
	{ USER_STATISTICS, &AlbumManager::userStatistics },
	{ TOP_TAGGED_USER, &AlbumManager::topTaggedUser },
	{ TOP_TAGGED_PICTURE, &AlbumManager::topTaggedPicture },
	{ PICTURES_TAGGED_USER, &AlbumManager::picturesTaggedUser },
	{COPY_PICTURE, &AlbumManager::createPicCopy },
	{CHANGE_PREM, &AlbumManager::changeFilePremissions},
	{ HELP, &AlbumManager::help },
	{ EXIT, &AlbumManager::exit }
};
;