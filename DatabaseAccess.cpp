#include "DataBaseAccess.h"

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
		std::string albums_table = "CREATE TABLE ALBUMS (ID INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , NAME TEXT NOT NULL , USER_ID INTEGER NOT NULL , CREATION_DATE TEXT NOT NULL,  FOREIGN KEY(USER_ID) REFERENCES USERS (ID));";
		std::string pictures_table = "CREATE TABLE PICTURES ( ID  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,  NAME TEXT  NOT NULL, LOCATION TEXT NOT NULL,CREATION_DATE TEXT NOT NULL, ALBUM_ID INTEGER NOT NULL,  FOREIGN KEY(ALBUM_ID ) REFERENCES ALBUMS (ID));";
		std::string tags_table = "CREATE TABLE TAGS(PICTURE_ID INTEGER NOT NULL, USER_ID INTEGER NOT NULL, PRIMARY KEY(PICTURE_ID,USER_ID),  FOREIGN KEY(PICTURE_ID ) REFERENCES PICTURES (ID), FOREIGN KEY(USER_ID ) REFERENCES USERS (ID));";
		std::string users_table = "CREATE TABLE USERS (ID INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , NAME TEXT NOT NULL);";

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

void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	std::string tags_del = "DELETE FROM TAGS WHERE TAGS.PICTURE_ID=(SELECT PICTURES.ID FROM PICTURES WHERE PICTURES.ALBUM_ID=(SELECT ALBUMS.ID FROM ALBUMS WHERE ALBUMS.NAME='" + albumName + "' AND ALBUMS.USER_ID=" + std::to_string(userId) + "));";
	std::string pic_del = "DELETE FROM PICTURES WHERE ALBUM_ID=(SELECT ID FROM ALBUMS WHERE ALBUMS.NAME='" + albumName + "' AND ALBUMS.USER_ID=" + std::to_string(userId) + ");";
	std::string al_del = "DELETE FROM ALBUMS WHERE ALBUMS.NAME='" + albumName + "' AND ALBUMS.USER_ID=" + std::to_string(userId) + ";";

	runQuery(tags_del + pic_del + al_del);
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
