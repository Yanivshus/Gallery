﻿#pragma once
#include <vector>
#include "Constants.h"
#include "MemoryAccess.h"
#include "Album.h"
#include <windows.h>
#include <string>
#include <chrono>
#include <thread>
#include <fileapi.h>
#include <fstream>


class AlbumManager
{
public:
	AlbumManager(IDataAccess& dataAccess);

	void executeCommand(CommandType command);
	void printHelp() const;

	using handler_func_t = void (AlbumManager::*)(void);    

private:
    int m_nextPictureId{};
    int m_nextUserId{};
    std::string m_currentAlbumName{};
	IDataAccess& m_dataAccess;
	Album m_openAlbum;

	void help();
	// albums management
	void createAlbum();
	void openAlbum();
	void closeAlbum();
	void deleteAlbum();
	void listAlbums();
	void listAlbumsOfUser();

	// Picture management
	void addPictureToAlbum();
	void removePictureFromAlbum();
	void listPicturesInAlbum();
	void showPicture();

	// tags related
	void tagUserInPicture();
	void untagUserInPicture();
	void listUserTags();

	// users management
	void addUser();
	void removeUser();
	void listUsers();
	void userStatistics();

	void topTaggedUser();
	void topTaggedPicture();
	void picturesTaggedUser();
	void exit();

	//bonuses

	/// <summary>
	/// creates a copy of the picture.
	/// </summary>
	void createPicCopy();
	
	/// <summary>
	/// changes the file premissions to readonly and back.
	/// </summary>
	void changeFilePremissions();


	/// <summary>
	/// return the time a file was written to.
	/// </summary>
	/// <param name="filePath:">path of the file.</param>
	/// <returns>FILETIME object which represents a time.</returns>
	static FILETIME getLastModOfFile(const std::string& filePath);
	


	std::string getInputFromConsole(const std::string& message);
	bool fileExistsOnDisk(const std::string& filename);
	void refreshOpenAlbum();
    bool isCurrentAlbumSet() const;

	static const std::vector<struct CommandGroup> m_prompts;
	static const std::map<CommandType, handler_func_t> m_commands;

};

