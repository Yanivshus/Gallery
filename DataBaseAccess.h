#pragma once
#include "IDataAccess.h"
#include "sqlite3.h"
#include <io.h>
#include <string>


#define USER_ID "USER_ID"
#define NAME "NAME"
#define CREATION_DATE "CREATION_DATE"
#define ID "ID"
#define LOCATION "LOCATION"


#define DB_PATH_NAME "galleryDB.sqlite"

class DatabaseAccess : public IDataAccess {
public:
	DatabaseAccess();
	~DatabaseAccess() = default;
	//opens the db, if doesnt exists - creates new one with the tables.
	virtual bool open() override; // done
	//closes the connection with the db.
	virtual void close() override; // done
	//deletes allocated memmory.
	virtual void clear() override; // done

	// album related
	virtual const std::list<Album> getAlbums() override;// done;
	virtual const std::list<Album> getAlbumsOfUser(const User& user) override; //done
	virtual void createAlbum(const Album& album) override; //done
	virtual void deleteAlbum(const std::string& albumName, int userId) override; // done
	virtual bool doesAlbumExists(const std::string& albumName, int userId) override; // done
	virtual Album openAlbum(const std::string& albumName) override;// done
	virtual void closeAlbum(Album& pAlbum) {} // done;
	virtual void printAlbums() {};
	
	// picture related
	virtual void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) {};
	virtual void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) {};
	virtual void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;//done
	virtual void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;//done
	 
	// user related
	virtual void printUsers() {};
	virtual void createUser(User& user) override; // done
	virtual void deleteUser(const User& user) override; // done
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
	

private:

	//for callbacks returns.
	std::list<Album> _albums;
	std::list<Picture> _pictures;
	std::list<int> _tags;

	//runs a given query, if succeeded return true, else false.
	bool runQuery(const std::string& query);
	sqlite3* _db;
};