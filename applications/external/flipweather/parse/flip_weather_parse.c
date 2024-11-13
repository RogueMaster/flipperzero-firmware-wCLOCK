#include "parse/flip_weather_parse.h"

bool sent_get_request = false;
bool get_request_success = false;
bool got_ip_address = false;
bool geo_information_processed = false;
bool weather_information_processed = false;

bool flip_weather_parse_ip_address() {
    // load the received data from the saved file
    FuriString* returned_data = flipper_http_load_from_file(fhttp.file_path);
    if(returned_data == NULL) {
        FURI_LOG_E(TAG, "Failed to load received data from file.");
        return false;
    }
    char* data_cstr = (char*)furi_string_get_cstr(returned_data);
    if(data_cstr == NULL) {
        FURI_LOG_E(TAG, "Failed to get C-string from FuriString.");
        furi_string_free(returned_data);
        return false;
    }
    char* ip = get_json_value("origin", (char*)data_cstr, MAX_TOKENS);
    if(ip == NULL) {
        FURI_LOG_E(TAG, "Failed to get IP address");
        sent_get_request = true;
        get_request_success = false;
        fhttp.state = ISSUE;
        furi_string_free(returned_data);
        free(data_cstr);
        return false;
    }
    snprintf(ip_address, 16, "%s", ip);
    free(ip);
    furi_string_free(returned_data);
    free(data_cstr);
    return true;
}

// handle the async-to-sync process to get and set the IP address
bool flip_weather_handle_ip_address() {
    if(fhttp.state == INACTIVE) {
        FURI_LOG_E(TAG, "Board is INACTIVE");
        flipper_http_ping(); // ping the device
        return false;
    }
    if(!got_ip_address) {
        got_ip_address = true;
        snprintf(
            fhttp.file_path,
            sizeof(fhttp.file_path),
            STORAGE_EXT_PATH_PREFIX "/apps_data/flip_weather/ip.txt");

        fhttp.save_received_data = true;
        if(!flipper_http_get_request("https://httpbin.org/get")) {
            FURI_LOG_E(TAG, "Failed to get IP address");
            fhttp.state = ISSUE;
            return false;
        } else {
            fhttp.state = RECEIVING;
            furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        }
        while(fhttp.state == RECEIVING && furi_timer_is_running(fhttp.get_timeout_timer) > 0) {
            // Wait for the feed to be received
            furi_delay_ms(10);
        }
        furi_timer_stop(fhttp.get_timeout_timer);
        if(!flip_weather_parse_ip_address()) {
            FURI_LOG_E(TAG, "Failed to get IP address");
            sent_get_request = true;
            get_request_success = false;
            fhttp.state = ISSUE;
            return false;
        }
    }
    return true;
}

bool send_geo_location_request() {
    if(fhttp.state == INACTIVE) {
        FURI_LOG_E(TAG, "Board is INACTIVE");
        flipper_http_ping(); // ping the device
        return false;
    }
    if(!sent_get_request && fhttp.state == IDLE) {
        sent_get_request = true;
        char* headers = jsmn("Content-Type", "application/json");
        get_request_success =
            flipper_http_get_request_with_headers("https://ipwhois.app/json/", headers);
        free(headers);
        if(!get_request_success) {
            FURI_LOG_E(TAG, "Failed to send GET request");
            fhttp.state = ISSUE;
            return false;
        }
        fhttp.state = RECEIVING;
    }
    return true;
}

void process_geo_location() {
    if(!geo_information_processed && fhttp.last_response != NULL) {
        geo_information_processed = true;
        char* city = get_json_value("city", fhttp.last_response, MAX_TOKENS);
        char* region = get_json_value("region", fhttp.last_response, MAX_TOKENS);
        char* country = get_json_value("country", fhttp.last_response, MAX_TOKENS);
        char* latitude = get_json_value("latitude", fhttp.last_response, MAX_TOKENS);
        char* longitude = get_json_value("longitude", fhttp.last_response, MAX_TOKENS);

        snprintf(city_data, 64, "City: %s", city);
        snprintf(region_data, 64, "Region: %s", region);
        snprintf(country_data, 64, "Country: %s", country);
        snprintf(lat_data, 64, "Latitude: %s", latitude);
        snprintf(lon_data, 64, "Longitude: %s", longitude);
        snprintf(ip_data, 64, "IP Address: %s", ip_address);

        fhttp.state = IDLE;
        free(city);
        free(region);
        free(country);
        free(latitude);
        free(longitude);
    }
}

void process_weather() {
    if(!weather_information_processed && fhttp.last_response != NULL) {
        weather_information_processed = true;
        char* current_data = get_json_value("current", fhttp.last_response, MAX_TOKENS);
        char* temperature = get_json_value("temperature_2m", current_data, MAX_TOKENS);
        char* precipitation = get_json_value("precipitation", current_data, MAX_TOKENS);
        char* rain = get_json_value("rain", current_data, MAX_TOKENS);
        char* showers = get_json_value("showers", current_data, MAX_TOKENS);
        char* snowfall = get_json_value("snowfall", current_data, MAX_TOKENS);
        char* time = get_json_value("time", current_data, MAX_TOKENS);

        // replace the "T" in time with a space
        char* ptr = strstr(time, "T");
        if(ptr != NULL) {
            *ptr = ' ';
        }

        snprintf(temperature_data, 64, "Temperature (C): %s", temperature);
        snprintf(precipitation_data, 64, "Precipitation: %s", precipitation);
        snprintf(rain_data, 64, "Rain: %s", rain);
        snprintf(showers_data, 64, "Showers: %s", showers);
        snprintf(snowfall_data, 64, "Snowfall: %s", snowfall);
        snprintf(time_data, 64, "Time: %s", time);

        fhttp.state = IDLE;
        free(current_data);
        free(temperature);
        free(precipitation);
        free(rain);
        free(showers);
        free(snowfall);
        free(time);
    } else if(!weather_information_processed && fhttp.last_response == NULL) {
        FURI_LOG_E(TAG, "Failed to process weather data");
        // store error message
        snprintf(temperature_data, 64, "Failed. Update WiFi settings.");
        fhttp.state = ISSUE;
    }
}
