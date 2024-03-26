#pragma once

#ifdef __cplusplus

#include <string>
#include <cstring>

extern "C" {
#include "cats/whisker.h"
}

namespace CATS
{

class Whisker
{
public:
    Whisker() {

    }
    ~Whisker() {
        
    }

    size_t get_len() {
        return len;
    }
    cats_whisker_type_t get_type() {
        return type;
    }
    cats_whisker_data_t* get_data() {
        return &data;
    }

protected:
    size_t len;
    cats_whisker_data_t data;
    cats_whisker_type_t type;
};

class Identification : public Whisker
{
public:
    Identification(std::string callsign = "", int icon = 0, uint8_t ssid = 0) {
        this->type = WHISKER_TYPE_IDENTIFICATION;
        this->ident = &(this->data.identification);

        strncpy(this->ident->callsign, callsign.c_str(), 252);
		this->ident->icon = icon;
		this->ident->ssid = ssid;
        this->len += 2 + callsign.length();
    }

    std::string get_callsign() {
        return this->ident->callsign;
    }
    int get_icon() {
        return this->ident->icon;
    }
    uint8_t get_ssid() {
        return this->ident->ssid;
    }

    void set_callsign(std::string callsign) {
        strncpy(this->ident->callsign, callsign.c_str(), 252);
    }
    void set_icon(int icon) {
        this->ident->icon = icon;
    }
    void set_ssid(uint8_t ssid) {
        this->ident->ssid = ssid;
    }

private:
    cats_ident_whisker_t* ident = nullptr;
};

}

#endif // __cplusplus