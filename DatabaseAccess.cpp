#include "DataBaseAccess.h"

//place for callbacks
int callbackGetAlbums(void* data, int argc, char** argv, char** azColName)
{
	//turing the data to static so if there will be a couple of runs of the callback the data will remain.
	auto& albumsList = *static_cast<std::list<Album>*>(data);
	Album newAlbum(0, "");

	//creating an album.
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == USER_ID) {
			newAlbum.setOwner(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == NAME) {
			newAlbum.setName(argv[i]);
		}
		else if (std::string(azColName[i]) == CREATION_DATE) {
			newAlbum.setCreationDate(argv[i]);
		}
	}
	albumsList.push_back(newAlbum);
	return 0;
}

DatabaseAccess::DatabaseAccess()
{
	this->_db = nullptr;
}


bool DatabaseAccess::open()
{
	int fileExists = _access(DB_PATH_NAME, 0);
	int res = sqlite3_open(DB_PATH_NAME, &this->_db);
	if (res != SQLITE_OK) 
	{
		this->_db = nullptr;
		std::cout << "Failes to open db" << std::endl;
	}

	if (fileExists != 0)
	{
		// creating the tables.
		std::string albums_table = "CREATE TABLE IF NOT EXISTS ALBUMS (ID INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , NAME TEXT NOT NULL , USER_ID INTEGER NOT NULL , CREATION_DATE TEXT NOT NULL,  FOREIGN KEY(USER_ID) REFERENCES USERS (ID));";
		std::string pictures_table = "CREATE TABLE IF NOT EXISTS PICTURES ( ID  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,  NAME TEXT  NOT NULL, LOCATION TEXT NOT NULL,CREATION_DATE TEXT NOT NULL, ALBUM_ID INTEGER NOT NULL,  FOREIGN KEY(ALBUM_ID ) REFERENCES ALBUMS (ID));";
		std::string tags_table = "CREATE TABLE IF NOT EXISTS TAGS(PICTURE_ID INTEGER NOT NULL, USER_ID INTEGER NOT NULL, PRIMARY KEY(PICTURE_ID,USER_ID),  FOREIGN KEY(PICTURE_ID ) REFERENCES PICTURES (ID), FOREIGN KEY(USER_ID ) REFERENCES USERS (ID));";
		std::string users_table = "CREATE TABLE IF NOT EXISTS USERS (ID INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , NAME TEXT NOT NULL);";

		//build the query
		std::string full_sql_query = albums_table + pictures_table + tags_table + users_table;

		if (!runQuery(full_sql_query)) 
		{
			return false;
		}
	}
	return true;
}


void DatabaseAccess::close()
{
	sqlite3_close(this->_db);//close the data base;
	this->_db = nullptr;
}


void DatabaseAccess::clear()
{
	delete this->_db;
	this->_db = nullptr;
}

const std::list<Album> DatabaseAccess::getAlbums()
{
	this->_albums.clear();//deleting former return values
	std::string query = "SELECT USER_ID, NAME, CREATION_DATE FROM ALBUMS;";
	char* errMsg = nullptr;
	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetAlbums, &this->_albums, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
	}
	return this->_albums;
}

const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	this->_albums.clear();//deleting former return values
	std::string query = "SELECT USER_ID, NAME, CREATION_DATE FROM ALBUMS WHERE USER_ID='" + std::to_string(user.getId()) + "';";
	char* errMsg = nullptr;
	//if qeury didnt worked we will print why.
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetAlbums, &this->_albums, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
	}
	return this->_albums;
}

void DatabaseAccess::createAlbum(const Album& album)
{
	std::string query = "INSERT INTO ALBUMS(NAME,USER_ID,CREATION_DATE) VALUES('"+album.getName() + "',"+std::to_string(album.getOwnerId()) + ",'"+album.getCreationDate() + "');";
	runQuery(query);
}


void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	std::string tags_del = "DELETE FROM TAGS WHERE TAGS.PICTURE_ID=(SELECT PICTURES.ID FROM PICTURES WHERE PICTURES.ALBUM_ID=(SELECT ALBUMS.ID FROM ALBUMS WHERE ALBUMS.NAME='" + albumName + "' AND ALBUMS.USER_ID=" + std::to_string(userId) + "));";
	std::string pic_del = "DELETE FROM PICTURES WHERE ALBUM_ID=(SELECT ID FROM ALBUMS WHERE ALBUMS.NAME='" + albumName + "' AND ALBUMS.USER_ID=" + std::to_string(userId) + ");";
	std::string al_del = "DELETE FROM ALBUMS WHERE ALBUMS.NAME='" + albumName + "' AND ALBUMS.USER_ID=" + std::to_string(userId) + ";";

	runQuery(tags_del + pic_del + al_del);
}

bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	this->_albums.clear();//deleting former return values
	std::string query = "SELECT NAME FROM ALBUMS WHERE USER_ID='"+std::to_string(userId) + "' AND NAME='"+albumName+"'";

	char* errMsg = nullptr;
	//if qeury didnt worked we will print why.
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetAlbums, &this->_albums, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
	}
	return !this->_albums.empty();
}



void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	std::string query = "INSERT INTO TAGS(PICTURE_ID, USER_ID) VALUES((SELECT ID FROM PICTURES WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "') AND NAME = '" + pictureName + "'), " + std::to_string(userId) + ");";
	runQuery(query);
}

void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	std::string query = "DELETE FROM TAGS WHERE PICTURE_ID = (SELECT ID FROM PICTURES WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '"+albumName+"') AND NAME = '"+pictureName+"') AND USER_ID = "+std::to_string(userId)+"; ";
	runQuery(query);
}

void DatabaseAccess::createUser(User& user)
{
	std::string query = "INSERT INTO USERS(NAME) VALUES('" + user.getName() + "');";
	runQuery(query);
}

void DatabaseAccess::deleteUser(const User& user)
{
	std::string id = std::to_string(user.getId());
	std::string del_tags = "DELETE FROM TAGS WHERE TAGS.USER_ID="+ id + ";";
	std::string del_pics = "DELETE FROM PICTURES WHERE PICTURES.ALBUM_ID=(SELECT ALBUMS.ID FROM ALBUMS WHERE ALBUMS.USER_ID="+id+");";
	std::string del_al = "DELETE FROM ALBUMS WHERE ALBUMS.USER_ID="+id+";";
	std::string del_user = "DELETE FROM USERS WHERE USERS.ID="+id+";";

	runQuery(del_tags + del_pics + del_al + del_user);
}




 bool DatabaseAccess::runQuery(const std::string& query)
{
	char* errMsg = nullptr;
	int res = sqlite3_exec(this->_db, query.c_str(), nullptr, nullptr, &errMsg);//execute the query
	//if qeury didnt worked we will print why.
	if (res != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
		return false;
	}
	return true;
}


