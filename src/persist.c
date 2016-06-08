#include <pebble.h>

void save_generic_data(uint32_t startkey, const void *data, uint16_t size){
	uint16_t offset = 0;
	while(offset < size){
		persist_write_data(startkey + offset / PERSIST_DATA_MAX_LENGTH, data + offset, PERSIST_DATA_MAX_LENGTH);
		offset += PERSIST_DATA_MAX_LENGTH;
	}
}

void load_generic_data(uint32_t startkey, void *data, uint16_t size){
	uint16_t offset = 0;
	while(offset < size){
		if(offset + PERSIST_DATA_MAX_LENGTH < size)
			persist_read_data(startkey + offset / PERSIST_DATA_MAX_LENGTH, data + offset, PERSIST_DATA_MAX_LENGTH);
		else 
			persist_read_data(startkey + offset / PERSIST_DATA_MAX_LENGTH, data + offset, size - offset);
		offset += PERSIST_DATA_MAX_LENGTH;
	}
}
