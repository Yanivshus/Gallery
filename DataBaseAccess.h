#pragma once
#include "IDataAccess.h"
#include "sqlite3.h"
#include <io.h>
#include <string>
#include "MyException.h"
#include <vector>



#define USER_ID "USER_ID"
#define NAME "NAME"
#define CREATION_DATE "CREATION_DATE"
#define ID "ID"
#define LOCATION "LOCATION"
#define ALBUM_ID "ALBUM_ID"
#define PICTURE_ID "PICTURE_ID"


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
	virtual void printAlbums() override; // done
	
	// picture related
	virtual void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override; // done
	virtual void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override; //done
	virtual void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;//done
	virtual void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;//done
	 
	// user related
	virtual void printUsers() override;// done
	virtual void createUser(User& user) override; // done
	virtual void deleteUser(const User& user) override; // done
	virtual bool doesUserExists(int userId) override; // done
	virtual User getUser(int userId) override; // done
	
	 // user statistics
	virtual int countAlbumsOwnedOfUser(const User& user) override; // done
	virtual int countAlbumsTaggedOfUser(const User& user) override; // done
	virtual int countTagsOfUser(const User& user) override; // done
	virtual float averageTagsPerAlbumOfUser(const User& user) override; // done
	
	// queries
	virtual User getTopTaggedUser() override; // done
	virtual Picture getTopTaggedPicture() override; // done
	virtual std::list<Picture> getTaggedPicturesOfUser(const User& user) override; // done
	

private:

	//for callbacks returns.
	std::list<Album> _albums;
	std::list<Picture> _pictures;
	std::list<int> _tags;
	std::list<User> _users;
	
	std::vector<std::pair<int, int>> _tagAndUser;
	

	//runs a given query, if succeeded return true, else false.
	bool runQuery(const std::string& query);
	sqlite3* _db;
};