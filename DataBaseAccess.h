#pragma once
#include "IDataAccess.h"
#include "sqlite3.h"
#include <io.h>

#define DB_PATH_NAME "galleryDB.sqlite"

class DatabaseAccess : public IDataAccess {
public:
	DatabaseAccess() = default;
	~DatabaseAccess() = default;

	// album related
	virtual const std::list<Album> getAlbums() { return std::list<Album>(); };
	virtual const std::list<Album> getAlbumsOfUser(const User& user) { return std::list<Album>(); };
	virtual void createAlbum(const Album& album) {};
	virtual void deleteAlbum(const std::string& albumName, int userId) {};
	virtual bool doesAlbumExists(const std::string& albumName, int userId) { return true; };
	virtual Album openAlbum(const std::string& albumName) { return Album(); };
	virtual void closeAlbum(Album& pAlbum) {};
	virtual void printAlbums() {};
	
	// picture related
	virtual void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) {};
	virtual void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) {};
	virtual void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) {};
	virtual void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) {};
	 
	// user related
	virtual void printUsers() {};
	virtual void createUser(User& user) {};
	virtual void deleteUser(const User& user) {};
	virtual bool doesUserExists(int userId) { return true; };
	virtual User getUser(int userId) { return User(1,""); };
	
	 // user statistics
	virtual int countAlbumsOwnedOfUser(const User& user) { return 0; };
	virtual int countAlbumsTaggedOfUser(const User& user) { return 0; };
	virtual int countTagsOfUser(const User& user) { return 0; };
	virtual float averageTagsPerAlbumOfUser(const User& user) { return 1.0; };
	
	// queries
	virtual User getTopTaggedUser() { return User(1,""); };
	virtual Picture getTopTaggedPicture() { return Picture(1,""); };
	virtual std::list<Picture> getTaggedPicturesOfUser(const User& user) { return std::list<Picture>(); };
	
	//opens the db, if doesnt exists - creates new one with the tables.
	virtual bool open() override;
	//closes the connection with the db.
	virtual void close() override;
	//deletes allocated memmory.
	virtual void clear() override;
private:
	sqlite3* _db;
};