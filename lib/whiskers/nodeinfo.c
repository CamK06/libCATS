#include "cats/whisker.h"
#include "cats/packet.h"
#include "cats/error.h"
#include "cats/util.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define CHECK_MASK(bitmap, mask) ((bitmap & mask) == mask)

size_t cats_nodeinfo_encode(const cats_whisker_data_t* data, uint8_t* dest)
{
	const cats_nodeinfo_whisker_t* info = &(data->node_info);
	uint32_t bitmap = 0;
	int ptr = 5;

	if(info->hardware_id.enabled) {
		dest[ptr++] = info->hardware_id.val;
		dest[ptr++] = info->hardware_id.val >> 8;
		bitmap |= CATS_NODEINFO_HARDWARE_ID;
	}
	if(info->software_id.enabled) {
		dest[ptr++] = info->software_id.val;
		bitmap |= CATS_NODEINFO_SOFTWARE_ID;
	}
	if(info->uptime.enabled) {
		dest[ptr++] = info->uptime.val;
		dest[ptr++] = info->uptime.val >> 8;
		dest[ptr++] = info->uptime.val >> 16;
		dest[ptr++] = info->uptime.val >> 24;
		bitmap |= CATS_NODEINFO_UPTIME;
	}
	if(info->ant_height.enabled) {
		dest[ptr++] = info->ant_height.val;
		bitmap |= CATS_NODEINFO_ANTENNA_HEIGHT;
	}
	if(info->ant_gain.enabled) {
		dest[ptr++] = info->ant_gain.val * 4;
		bitmap |= CATS_NODEINFO_ANTENNA_GAIN;
	}
	if(info->tx_power.enabled) {
		dest[ptr++] = info->tx_power.val * 4;
		bitmap |= CATS_NODEINFO_TX_POWER;
	}
	if(info->voltage.enabled) {
		dest[ptr++] = info->voltage.val * 10;
		bitmap |= CATS_NODEINFO_VOLTAGE;
	}
	if(info->temperature.enabled) {
		dest[ptr++] = info->temperature.val;
		bitmap |= CATS_NODEINFO_TEMP;
	}
	if(info->battery_level.enabled) {
		dest[ptr++] = info->battery_level.val * 2.55;
		bitmap |= CATS_NODEINFO_BATTERY;
	}

	dest[4] = bitmap;
	dest[3] = bitmap >> 8;
	dest[2] = bitmap >> 16;
	return ptr - 2;
}

void cats_nodeinfo_decode(const uint8_t* data, size_t len, cats_whisker_data_t* dest)
{
	cats_nodeinfo_whisker_t* info = &(dest->node_info);
	memset(info, 0x00, sizeof(cats_nodeinfo_whisker_t));
	uint32_t bitmap = (data[2] << 16) | (data[3] << 8) | data[4];
	int ptr = 5;

	// This code is beyond horrendous
	if(CHECK_MASK(bitmap, CATS_NODEINFO_HARDWARE_ID)) {
		info->hardware_id.val = (data[ptr + 1] << 8) | data[ptr];
		info->hardware_id.enabled = true;
		ptr += 2;
	}
	if(CHECK_MASK(bitmap, CATS_NODEINFO_SOFTWARE_ID)) {
		info->software_id.val = data[ptr++];
		info->software_id.enabled = true;
	}
	if(CHECK_MASK(bitmap, CATS_NODEINFO_UPTIME)) {
		info->uptime.val = (data[ptr + 3] << 24) | (data[ptr + 2] << 16) | (data[ptr + 1] << 8) | data[ptr];
		info->uptime.enabled = true;
		ptr += 4;
	}
	if(CHECK_MASK(bitmap, CATS_NODEINFO_ANTENNA_HEIGHT)) {
		info->ant_height.val = data[ptr++];
		info->ant_height.enabled = true;
	}
	if(CHECK_MASK(bitmap, CATS_NODEINFO_ANTENNA_GAIN)) {
		info->ant_gain.val = data[ptr++] / 4;
		info->ant_gain.enabled = true;
	}
	if(CHECK_MASK(bitmap, CATS_NODEINFO_TX_POWER)) {
		info->tx_power.val = data[ptr++] / 4;
		info->tx_power.enabled = true;
	}
	if(CHECK_MASK(bitmap, CATS_NODEINFO_VOLTAGE)) {
		info->voltage.val = data[ptr++] / 10;
		info->voltage.enabled = true;
	}
	if(CHECK_MASK(bitmap, CATS_NODEINFO_TEMP)) {
		info->temperature.val = data[ptr++];
		info->temperature.enabled = true;
	}
	if(CHECK_MASK(bitmap, CATS_NODEINFO_BATTERY)) {
		info->battery_level.val = data[ptr++] / 2.55;
		info->battery_level.enabled = true;
	}
}

int cats_packet_get_nodeinfo(const cats_packet_t* pkt, cats_nodeinfo_whisker_t** out)
{
	assert(pkt != NULL);
	cats_whisker_t** whiskers;
	const int whiskers_found = cats_packet_find_whiskers(pkt, WHISKER_TYPE_NODEINFO, &whiskers);
	if(whiskers_found <= CATS_FAIL) {
		throw_msg(WHISKER_NOT_FOUND, "cats_packet_get_nodeinfo: packet has no nodeinfo whisker!");
	}
	assert(out != NULL);
	cats_whisker_t* whisker = *whiskers;
	*out = &whisker->data.node_info;

	free(whiskers);
	return CATS_SUCCESS;
}

int cats_packet_add_nodeinfo(cats_packet_t* pkt, cats_nodeinfo_whisker_t info)
{
	assert(pkt != NULL);
	if(pkt->len + 15 > CATS_MAX_PKT_LEN) {
		throw(PACKET_TOO_BIG);
	}
	if(cats_packet_get_nodeinfo(pkt, NULL) != CATS_FAIL) {
		throw(MAX_WHISKERS_OF_TYPE_EXCEEDED);
	}

	return cats_packet_add_whisker_data(pkt, WHISKER_TYPE_NODEINFO, (cats_whisker_data_t*)&info, 15);
}