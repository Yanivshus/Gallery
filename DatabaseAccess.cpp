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


int callbackGetPictures(void* data, int argc, char** argv, char** azColName) 
{
	auto& picturesList = *static_cast<std::list<Picture>*>(data);
	Picture newPicture(0, "");

	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == ID) {
			newPicture.setId(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == NAME) {
			newPicture.setName(argv[i]);
		}
		else if (std::string(azColName[i]) == LOCATION) {
			newPicture.setPath(argv[i]);
		}
		else if (std::string(azColName[i]) == CREATION_DATE) {
			newPicture.setCreationDate(argv[i]);
		}
	}

	picturesList.push_back(newPicture);
	return 0;
}


int callbackGetNum(void* data, int argc, char** argv, char** azColName) {
	if (argc == 1 && argv[0] != nullptr) {
		*static_cast<int*>(data) = atoi(argv[0]);
		return 0;
	}
	return 1;
}


int callbackGetTags(void* data, int argc, char** argv, char** azColName) 
{
	auto& tagList = *static_cast<std::list<int>*>(data);

	for (int i = 0; i < argc; i++)
	{
		if(std::string(azColName[i]) == USER_ID){
			tagList.push_back(atoi(argv[i]));
		}
	}
	return 0;
}


int callbackGetTagsAndUserIds(void* data, int argc, char** argv, char** azColName)
{
	auto& tagList = *static_cast<std::vector<std::pair<int, int>>*>(data);

	for (int i = 0; i < argc; i++)
	{
		int pic_id = 0;
		int user_id = 0;

		if (std::string(azColName[i]) == USER_ID) {
			user_id = atoi(argv[i]);
		}
		else if(std::string(azColName[i]) == PICTURE_ID){
			pic_id = atoi(argv[i]);
		}
		tagList.push_back(std::make_pair(pic_id, user_id));
	}
	return 0;
}


int callbackGetUsers(void* data, int argc, char** argv, char** azColName) 
{
	auto& usersList = *static_cast<std::list<User>*>(data);

	User newUser(0, "");
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == ID) {
			newUser.setId(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == NAME) {
			newUser.setName(argv[i]);
		}
	}
	usersList.push_back(newUser);
	return 0;
}


//actual functions
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


//album realated.
const std::list<Album> DatabaseAccess::getAlbums()
{
	this->_albums.clear();//deleting former return values
	std::string query = "SELECT USER_ID, NAME, CREATION_DATE FROM ALBUMS;";
	char* errMsg = nullptr;
	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetAlbums, &this->_albums, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: getAlbums" << std::endl;
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
		std::cout << "Function: getAlbumsOfUser" << std::endl;
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
	std::string query = "SELECT NAME FROM ALBUMS WHERE USER_ID="+std::to_string(userId) + " AND NAME='"+albumName+"'";

	char* errMsg = nullptr;
	//if qeury didnt worked we will print why.
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetAlbums, &this->_albums, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: doesAlbumExists" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
	}
	return !this->_albums.empty();
}

Album DatabaseAccess::openAlbum(const std::string& albumName)
{
	this->_albums.clear();//deleting former return values
	std::string query = "SELECT USER_ID, NAME, CREATION_DATE FROM ALBUMS WHERE NAME='" + albumName + "';";
	char* errMsg = nullptr;
	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetAlbums, &this->_albums, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: getAlbumsStart" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
	}

	//now i will get the id of the album so i can get his pictures.
	query = "SELECT ID FROM ALBUMS WHERE NAME='"+albumName+"';";
	int id_of_album = 0;
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetNum, &id_of_album, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: openAlbumGetInt" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
	}

	// now i will get the pictures from the album.
	this->_pictures.clear();
	query = "SELECT ID, NAME, LOCATION, CREATION_DATE FROM PICTURES WHERE ALBUM_ID=" + std::to_string(id_of_album) + ";";
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetPictures, &this->_pictures, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: openAlbumGetPic" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
	}


	
	//run on the pictures, create tags for them.
	for (const auto& i : this->_pictures)
	{
		//add the picture to the albums.
		this->_albums.back().addPicture(Picture(i.getId(), i.getName(), i.getPath(), i.getCreationDate()));

		query = "SELECT USER_ID FROM TAGS WHERE PICTURE_ID=" + std::to_string(i.getId()) + ";";
		this->_tags.clear();
		if (sqlite3_exec(this->_db, query.c_str(), callbackGetTags, &this->_tags, &errMsg) != SQLITE_OK) {
			std::cout << "sql err" << std::endl;
			std::cout << "Function: openAlbumGetTags" << std::endl;
			std::cout << "reason: " << errMsg << std::endl;
		}
		//adding all the tags to the picture.
		std::string currPic = i.getName();
		for (const auto& g : this->_tags)
		{
			this->_albums.back().tagUserInPicture(g, currPic);
		}

	}
	return this->_albums.back();
}

void DatabaseAccess::printAlbums()
{
	this->_albums.clear();
	std::list<Album> albumToPrint = getAlbums();
	if (albumToPrint.empty()) {
		throw MyException("There are no existing albums.");
	}
	else {
		std::cout << "Album list:" << std::endl;
		std::cout << "-----------" << std::endl;
		for (const auto& album : albumToPrint)
		{
			std::cout << std::setw(5) << "* " << album;
			std::cout << "     Created on : " << album.getCreationDate() << std::endl;
		}
	}
}


//picture realated.
void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	int id = 0;
	std::string query = "SELECT ID FROM ALBUMS WHERE NAME='" + albumName + "';";
	char* errMsg = nullptr;
	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetNum, &id, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: addPictureToAlbumByName" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
		return;
	}

	query = "INSERT INTO PICTURES(NAME, LOCATION, CREATION_DATE,ALBUM_ID) VALUES('" + picture.getName() + "', '" + picture.getPath() + "', '" + picture.getCreationDate() + "', " + std::to_string(id) + ");";
	runQuery(query);
}

void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	//first i will get the picture id.
	std::string query = "SELECT ID FROM PICTURES WHERE PICTURES.ALBUM_ID=(SELECT ID FROM ALBUMS WHERE ALBUMS.NAME='" + albumName + "') AND PICTURES.NAME='" + pictureName + "';";
	char* errMsg = nullptr;
	int id = 0;
	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetNum, &id, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: removePictureFromAlbumByName" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
		throw MyException("There are no existing picture in album.");
	}
	//now i will delete from the tags and the picture it self.
	query = "DELETE FROM TAGS WHERE TAGS.PICTURE_ID=" + std::to_string(id) + ";";
	runQuery(query);
	query = "DELETE FROM PICTURES WHERE PICTURES.ID="+ std::to_string(id) +";";
	runQuery(query);
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


//user realted.
void DatabaseAccess::printUsers()
{
	this->_users.clear();
	std::string query = "SELECT * FROM USERS";
	char* errMsg = nullptr;
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetUsers, &this->_users, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: printUsers" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
		return;
	}

	if (this->_users.empty()) {//if there are no users in the db.
		throw MyException("There are no existing users.");
	}
	else {
		std::cout << "Users list:" << std::endl;
		std::cout << "-----------" << std::endl;
		for (const auto& user : this->_users)
		{
			std::cout << user << std::endl;
		}
	}
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

bool DatabaseAccess::doesUserExists(int userId)
{
	this->_users.clear();
	std::string query = "SELECT * FROM USERS WHERE ID=" + std::to_string(userId) + ";";
	char* errMsg = nullptr;
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetUsers, &this->_users, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: doesUserExists" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
		return false;
	}
	//if user wasnt fount we return false.
	if (this->_users.empty()) {
		return false;
	}
	return true;
}

User DatabaseAccess::getUser(int userId)
{
	//checking first if the user exists.
	if (doesUserExists(userId)) {
		// if yes i will get him from the db.
		this->_users.clear();
		std::string query = "SELECT * FROM USERS WHERE ID=" + std::to_string(userId) + ";";
		char* errMsg = nullptr;
		if (sqlite3_exec(this->_db, query.c_str(), callbackGetUsers, &this->_users, &errMsg) != SQLITE_OK) {
			std::cout << "sql err" << std::endl;
			std::cout << "Function: doesUserExists" << std::endl;
			std::cout << "reason: " << errMsg << std::endl;
			throw MyException("There are no existing users.");
		}
		return this->_users.back();//return the user, there is alwayes one.
	}
	else
	{
		return User(-1, "");
	}

}


//user statistics realated.
int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	std::list<Album> albums = getAlbums();
	int count = 0;
	//run on the albums and check which is the user albums.
	for (const auto& album : albums )
	{
		if (album.getOwnerId() == user.getId()) {
			count++;
		}
	}
	return count;
}

int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	// this query joins the albums tags and pictures table toghther and count the distinct number of albums which the user tagged in.
	std::string query = "SELECT COUNT(DISTINCT ALBUMS.ID) "
						"FROM ALBUMS "
						"INNER JOIN PICTURES ON ALBUMS.ID = PICTURES.ALBUM_ID "
						"INNER JOIN TAGS ON PICTURES.ID = TAGS.PICTURE_ID "
						"WHERE TAGS.USER_ID = "+std::to_string(user.getId()) + ";";
	
	char* errMsg = nullptr;
	int count = 0;
	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetNum, &count, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: countAlbumsTaggedOfUser" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
		throw MyException("There are no existing albums or tags.");
	}
	return count;
}

int DatabaseAccess::countTagsOfUser(const User& user)
{
	//this query count the number of tags user is tagged.
	std::string query = "SELECT COUNT(TAGS.USER_ID) "
						"FROM TAGS "
						"WHERE TAGS.USER_ID = "+ std::to_string(user.getId()) + ";";
	char* errMsg = nullptr;
	int count = 0;
	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetNum, &count, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: countTagsOfUser" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
		throw MyException("There are tags.");
	}
	return count;
}

float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	int albumsTaggedCount = countAlbumsTaggedOfUser(user);

	if (0 == albumsTaggedCount) {
		return 0;
	}

	return static_cast<float>(countTagsOfUser(user)) / albumsTaggedCount;
}


//queries
User DatabaseAccess::getTopTaggedUser()
{
	//this query finds the user who is the most tagged.
	std::string query = "SELECT USERS.ID, USERS.Name "
		"FROM USERS "
		"JOIN( "
		"SELECT TAGS.USER_ID "
		"FROM TAGS "
		"GROUP BY TAGS.USER_ID "
		"ORDER BY COUNT(*) DESC "
		"LIMIT 1 "
		") AS TopUser ON USERS.ID = TopUser.USER_ID;";

	char* errMsg = nullptr;
	this->_users.clear();

	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetUsers, &this->_users, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: getTopTaggedUser" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;
		
		throw MyException("There are no tags.");
	}
	if (this->_users.empty()) {
		return User(0, "");
	}
	return this->_users.back();
}

Picture DatabaseAccess::getTopTaggedPicture()
{
	this->_pictures.clear();
	//this query gets the picture who is tagged the most.
	std::string query = "SELECT PICTURES.ID,PICTURES.NAME,PICTURES.LOCATION,PICTURES.CREATION_DATE "
		"FROM PICTURES "
		"JOIN( "
			"SELECT TAGS.PICTURE_ID "
			"FROM TAGS "
			"GROUP BY TAGS.PICTURE_ID "
			"ORDER BY COUNT(*) DESC "
			"LIMIT 1 "
		") AS TopPic ON PICTURES.ID = TopPic.PICTURE_ID; """;

	char* errMsg = nullptr;
	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetPictures, &this->_pictures, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: getTopTaggedPicture" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;

		throw MyException("There are no tags.");
	}
	if (this->_pictures.empty()) {
		return Picture(0, "");
	}
	return this->_pictures.back();
}

std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	std::string query = "SELECT PICTURES.ID,  PICTURES.NAME, PICTURES.LOCATION, PICTURES.CREATION_DATE "
		"FROM PICTURES "
		"JOIN TAGS ON PICTURES.ID = TAGS.PICTURE_ID "
		"WHERE TAGS.USER_ID="+std::to_string(user.getId()) + ";";
	this->_pictures.clear();

	char* errMsg = nullptr;
	//execute the query
	if (sqlite3_exec(this->_db, query.c_str(), callbackGetPictures, &this->_pictures, &errMsg) != SQLITE_OK) {
		std::cout << "sql err" << std::endl;
		std::cout << "Function: getTaggedPicturesOfUser" << std::endl;
		std::cout << "reason: " << errMsg << std::endl;

		throw MyException("There are no pictures with tagged user.");
	}

	//now i will run on all the pictures and tag the users thet tagged on his pictures.
	for (auto& pic : this->_pictures)
	{
		this->_tagAndUser.clear();
		//query to select the users that are tagged.
		std::string inQuery = "SELECT TAGS.PICTURE_ID, TAGS.USER_ID FROM TAGS WHERE TAGS.PICTURE_ID=" + std::to_string(pic.getId()) + ";";
		//execute the query
		if (sqlite3_exec(this->_db, inQuery.c_str(), callbackGetTagsAndUserIds, &this->_tagAndUser, &errMsg) != SQLITE_OK) {
			std::cout << "sql err" << std::endl;
			std::cout << "Function: getTaggedPicturesOfUserIn" << std::endl;
			std::cout << "reason: " << errMsg << std::endl;

			throw MyException("There are no tags.");
		}
		//run on the tags vector and add to the correct photos.
		for (auto& tags : this->_tagAndUser)
		{
			if (tags.first == pic.getId()) {
				pic.tagUser(tags.second);
			}
		}
	}

	return this->_pictures;
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


