#include "DataBaseAccess.h"

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
		std::cout << full_sql_query << std::endl;
		char* errMsg = nullptr;
		res = sqlite3_exec(this->_db, full_sql_query.c_str(), nullptr, nullptr, &errMsg);//execute the query
		//check if succefully created.
		if (res != SQLITE_OK) {
			std::cout << "not wotking" << std::endl;
			return false;
		}
		return true;
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
