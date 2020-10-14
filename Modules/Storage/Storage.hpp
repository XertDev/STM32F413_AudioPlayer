/*
 * Storage.h
 *
 *  Created on: Oct 14, 2020
 *      Author: xert
 */

#ifndef STORAGE_STORAGE_HPP_
#define STORAGE_STORAGE_HPP_

#include "fatfs.h"

enum StorageErrors{
	OK=0,
	UNKNOWN_ERROR,
	CARD_INIT_ERROR,
	MOUNT_ERROR,
	NO_DIRECTORY,
	BUS_WIDTH_ERROR
};

class Storage {
public:
	Storage();
	~Storage();

	StorageErrors init(SD_HandleTypeDef& handle);
	void deInit();

	StorageErrors entriesInDirectoryCount(const char* dir_name, uint8_t& count);

private:

	bool initialized_ = false;
	FATFS SD_fat_fs_;
};

#endif /* STORAGE_STORAGE_HPP_ */
