/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/08 21:30:57 by vzurera-          #+#    #+#             */
/*   Updated: 2024/08/21 14:51:15 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Settings.hpp"

#include <unistd.h>																						//	For access() to checks the accessibility of a file or directory
#include <sys/stat.h>																					//	For stat() to retrieves information about a file or directory

#pragma region Variables

	enum e_section { ROOT, GLOBAL, SERVER, LOCATION, METHOD };

#pragma endregion

#pragma region Directives

	#pragma region Path

		int Settings::parse_path(const std::string & firstPart, std::string & str, bool isFile = false, bool check_path = false, bool check_write = false) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] "; struct stat info;

			if (str.empty()) {										Log::log(RD + n_line + "Empty value for " Y + firstPart + NC, Log::BOTH_ERROR); return (1); }
			if (str[0] != '/') str = program_path + str;

			if (isFile && check_path) {
				std::string dir_path = str.substr(0, str.find_last_of('/'));
				if (stat(dir_path.c_str(), &info) != 0) {
					if (errno == ENOENT)							Log::log(RD + n_line + "The " Y + firstPart + RD " path " Y + dir_path + RD " does not exist" NC, Log::BOTH_ERROR);
					else if (errno == EACCES)						Log::log(RD + n_line + "No permission to access " Y + dir_path + NC, Log::BOTH_ERROR);
					else											Log::log(RD + n_line + "Cannot access " Y + dir_path + NC, Log::BOTH_ERROR);
					return (1);
				} else {
					if (!(info.st_mode & S_IFDIR)) {				Log::log(RD + n_line + dir_path + RD " is not a valid directory" NC, Log::BOTH_ERROR); return (1); }
					else if (firstPart != "access_log" && firstPart != "error_log" && access(str.c_str(), F_OK) != 0) {
																	Log::log(RD + n_line + "The " Y + firstPart + RD " path " Y + str + RD " does not exist" NC, Log::BOTH_ERROR); return (1); }
					else if (check_write &&
						access(dir_path.c_str(), W_OK) != 0) {		Log::log(RD + n_line + "No write permission for " Y + dir_path + NC, Log::BOTH_ERROR); return (1); }
					else if (check_write && access(str.c_str(), F_OK) == 0 &&
						access(str.c_str(), W_OK) != 0) {			Log::log(RD + n_line + "No write permission for " Y + str + NC, Log::BOTH_ERROR); return (1); }
				}
			} else if (stat(str.c_str(), &info) != 0) {
				if (errno == ENOENT)								Log::log(RD + n_line + "The " Y + firstPart + RD " path " Y + str + RD " does not exist" NC, Log::BOTH_ERROR);
				else if (errno == EACCES)							Log::log(RD + n_line + "No permission to access " Y + str + NC, Log::BOTH_ERROR);
				else 												Log::log(RD + n_line + "Cannot access " Y + str + NC, Log::BOTH_ERROR);
				return (1);
			} else {
				if (isFile && !(info.st_mode & S_IFREG)) { 			Log::log(RD + n_line + Y + str + RD " is not a valid file" NC, Log::BOTH_ERROR); return (1); return (1); }
				else if (!isFile && !(info.st_mode & S_IFDIR)) { 	Log::log(RD + n_line + Y + str + RD " is not a valid directory" NC, Log::BOTH_ERROR); return (1); }
				else if (access(str.c_str(), R_OK) != 0) {			Log::log(RD + n_line + "No permission to access " Y + str + NC, Log::BOTH_ERROR); return (1); }
			}
			return (0);
		}

	#pragma endregion

	#pragma region Log Days

		int Settings::parse_log_days(std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";

			if (str.empty()) { 								Log::log(RD + n_line + "There is no value for " Y "log_days" NC, Log::BOTH_ERROR); return (1); }
			long number; if (Utils::stol(str, number)) { 	Log::log(RD + n_line + "Invalid value " Y + str + RD " for " Y "log_days" NC, Log::BOTH_ERROR); return (1); }
			if (number < 0) { 								Log::log(RD + n_line + "Invalid value " Y + str + RD " for " Y "log_days" RD " cannot be " Y "lower" RD " than " Y "0" NC, Log::BOTH_ERROR); return (1); }
			if (number > 365) { 							Log::log(RD + n_line + "Invalid value " Y + str + RD " for " Y "log_days" RD " cannot be " Y "greater" RD " than " Y "365" NC, Log::BOTH_ERROR); return (1); }

			return (0);
		}

	#pragma endregion

	#pragma region Body Size

		int Settings::parse_body_size(std::string & str) {
			long multiplier = 1; std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";

			if (str.empty()) { Log::log(RD + n_line + "Empty value for " Y "client_max_body_size" NC, Log::BOTH_ERROR); return (1); }
			if (str.size() > 1 && !std::isdigit(str[str.size() - 1]) && !std::isdigit(str[str.size() - 2]) && std::tolower(str[str.size() - 1]) == 'b') str.erase(str.size() - 1);
			if (!std::isdigit(str[str.size() - 1])) {
				switch ( std::tolower(str[str.size() - 1])) {
					case 'k': multiplier = 1024; break;
					case 'm': multiplier = 1024 * 1024; break;
					case 'g': multiplier = 1024 * 1024 * 1024; break;
					case 'b' : break;
					default : { Log::log(RD + n_line + "Invalid value for " Y "client_max_body_size" NC, Log::BOTH_ERROR); return (1); }
				} str.erase(str.size() - 1);
			}

			long number; if (Utils::stol(str, number) || (str = Utils::ltos(number * multiplier)) == "") { Log::log(RD + n_line + "Invalid value for '" Y "client_max_body_size" RD "'" NC, Log::BOTH_ERROR); return (1); }
			if (number < 1) { Log::log(RD + n_line + "Value for " Y "client_max_body_size" RD " cannot be " Y "lower" RD " than " Y "1 byte" NC, Log::BOTH_ERROR); return (1); }
			if (number > 1024 * 1024 * 1024) { Log::log(RD + n_line + "Value for " Y "client_max_body_size" RD " cannot be " Y "greater" RD " than " Y "1GB" NC, Log::BOTH_ERROR); return (1); }
			return (0);
		}

	#pragma endregion

	#pragma region Error Codes

		int Settings::parse_errors(const std::string & firstPart, const std::string & secondPart) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::istringstream stream(secondPart);
			std::vector<std::string> errors; std::string error;

			while (stream >> error) errors.push_back(error);
			if (errors.size() < 2) { Log::log(RD + n_line + "Empty value for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; return (1); }
			std::string filePath = errors.back(); errors.pop_back();
			if (filePath.empty() || filePath[0] != '/') { Log::log(RD + n_line + "Invalid path for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; return (1); }
			if (errors.size() > 1 && errors.back()[0] == '=') {
				long code; if (Utils::stol(errors.back().substr(1), code) || (error_codes.find(code) == error_codes.end())) {
					Log::log(RD + n_line + "Invalid status code " Y + errors.back().substr(1) + RD " for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; }
				filePath = errors.back() + filePath; errors.pop_back();
			}

			for (std::vector<std::string>::iterator it = errors.begin(); it != errors.end(); ++it) {
				long code; if (Utils::stol(*it, code) || (error_codes.find(code) == error_codes.end())) {
					Log::log(RD + n_line + "Invalid status code " Y +  *it + RD " for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; }
				else global.add(firstPart + " " + *it, filePath);
			}
			return (0);
		}

		int Settings::parse_errors(const std::string & firstPart, const std::string & secondPart, VServer & VServ) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::istringstream stream(secondPart);
			std::vector<std::string> errors; std::string error;

			while (stream >> error) errors.push_back(error);
			if (errors.size() < 2) { Log::log(RD + n_line + "Empty value for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; return (1); }
			std::string filePath = errors.back(); errors.pop_back();
			if (filePath.empty() || filePath[0] != '/') { Log::log(RD + n_line + "Invalid path for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; return (1); }
			if (errors.size() > 1 && errors.back()[0] == '=') {
				long code; if (Utils::stol(errors.back().substr(1), code) || (error_codes.find(code) == error_codes.end())) {
					Log::log(RD + n_line + "Invalid status code " Y + errors.back().substr(1) + RD " for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; }
				filePath = errors.back() + filePath; errors.pop_back();
			}

			for (std::vector<std::string>::iterator it = errors.begin(); it != errors.end(); ++it) {
				long code; if (Utils::stol(*it, code) || (error_codes.find(code) == error_codes.end())) {
					Log::log(RD + n_line + "Invalid status code " Y +  *it + RD " for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; }
				else VServ.add(firstPart + " " + *it, filePath);
			}
			return (0);
		}

		int Settings::parse_errors(const std::string & firstPart, const std::string & secondPart, Location & Loc) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::istringstream stream(secondPart);
			std::vector<std::string> errors; std::string error;

			while (stream >> error) errors.push_back(error);
			if (errors.size() < 2) { Log::log(RD + n_line + "Empty value for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; return (1); }
			std::string filePath = errors.back(); errors.pop_back();
			if (filePath.empty() || filePath[0] != '/') { Log::log(RD + n_line + "Invalid path for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; return (1); }
			if (errors.size() > 1 && errors.back()[0] == '=') {
				long code; if (Utils::stol(errors.back().substr(1), code) || (error_codes.find(code) == error_codes.end())) {
					Log::log(RD + n_line + "Invalid status code " Y + errors.back().substr(1) + RD " for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; }
				filePath = errors.back() + filePath; errors.pop_back();
			}

			for (std::vector<std::string>::iterator it = errors.begin(); it != errors.end(); ++it) {
				long code; if (Utils::stol(*it, code) || (error_codes.find(code) == error_codes.end())) {
					Log::log(RD + n_line + "Invalid status code " Y +  *it + RD " for " Y "error_page" NC, Log::BOTH_ERROR); BadConfig = true; }
				else Loc.add(firstPart + " " + *it, filePath);
			}
			return (0);
		}

	#pragma endregion

	#pragma region Autoindex

		int Settings::parse_autoindex(std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";

			if (str.empty()) {					Log::log(RD + n_line + "Empty value for " Y "autoindex" NC, Log::BOTH_ERROR); return (1); }
			if (str != "on" && str != "off") {	Log::log(RD + n_line + "Invalid value for " Y "autoindex" NC, Log::BOTH_ERROR); return (1); }
				
			return (0);
		}

	#pragma endregion

	#pragma region Index

		int Settings::parse_index(std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";

			if (str.empty()) {		Log::log(RD + n_line + "Empty value for " Y "index" NC, Log::BOTH_ERROR); return (1); }
			if (str[0] == '/') {	Log::log(RD + n_line + "Invalid value for " Y "index" NC, Log::BOTH_ERROR); return (1); }

			return (0);
		}

	#pragma endregion

	#pragma region Listen

		int Settings::parse_listen(std::string & str, VServer & VServ) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::string temp; std::istringstream stream(str); stream >> temp;
			std::string IP; std::string port; std::string::size_type slashPos = temp.find(':');	
			
			if (temp.empty()) { 																		Log::log(RD + n_line + "Empty value for " Y "listen" NC, Log::BOTH_ERROR); return (1); }

			if (slashPos != std::string::npos) {
				port = temp.substr(slashPos + 1);
				temp = temp.substr(0, slashPos);

				if (temp.empty()) {																		Log::log(RD + n_line + "Invalid IP for " Y "listen" NC, Log::BOTH_ERROR); return (1); }
				slashPos = temp.find('/');
				if (slashPos != std::string::npos) {
					std::string ip = temp.substr(0, slashPos);
					std::string mask = temp.substr(slashPos + 1);
					if (ip.empty()) {																	Log::log(RD + n_line + "Invalid IP for " Y "listen" NC, Log::BOTH_ERROR); return (1); }
					if (Utils::isValidIP(temp.substr(0, slashPos)) == false) {							Log::log(RD + n_line + "Invalid IP " Y + temp.substr(0, slashPos) + RD " for " Y "listen" NC, Log::BOTH_ERROR); return (1); }
					if (mask.empty()) {																	Log::log(RD + n_line + "Invalid mask for " Y "listen" NC, Log::BOTH_ERROR); return (1); }
					if (mask.find('.') != std::string::npos) { if (Utils::isValidIP(mask) == false) {	Log::log(RD + n_line + "Invalid mask " Y + temp.substr(slashPos + 1) + RD " for " Y "listen" NC, Log::BOTH_ERROR); return (1); }}
					else { long number; if (Utils::stol(mask, number) || number < 0 || number > 32) {	Log::log(RD + n_line + "Invalid mask " Y + temp.substr(slashPos + 1) + RD " for " Y "listen" NC, Log::BOTH_ERROR); return (1); }}
					IP = temp;
				} else {
					if (Utils::isValidIP(temp) == false) {												Log::log(RD + n_line + "Invalid IP " Y + temp + RD " for " Y "listen" NC, Log::BOTH_ERROR); return (1); }
					IP = temp;
				}
			} else port = temp;

			if (port.empty()) { 																		Log::log(RD + n_line + "There is no port for " Y "listen" NC, Log::BOTH_ERROR); return (1); }
			long number; if (Utils::stol(port, number)) { 												Log::log(RD + n_line + "Invalid port " Y + port + RD " for " Y "listen" NC, Log::BOTH_ERROR); return (1); }
			if (number < 1) { 																			Log::log(RD + n_line + "Invalid port " Y + port + RD " for " Y "listen" RD " cannot be " Y "lower" RD " than " Y "1" NC, Log::BOTH_ERROR); return (1); }
			if (number > 65535) { 																		Log::log(RD + n_line + "Invalid port " Y + port + RD " for " Y "listen" RD " cannot be " Y "greater" RD " than " Y "65535" NC, Log::BOTH_ERROR); return (1); }
			if (IP.empty()) Utils::add_address("0.0.0.0", number, VServ);
			else Utils::add_address(IP, number, VServ);
		
			return (0);
		}

	#pragma endregion

	#pragma region Return

		int Settings::parse_return(std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::istringstream stream(str); std::string code, path;

			stream >> code; stream >> path;
			if (code.empty()) { Log::log(RD + n_line + "Empty value for " Y "return" NC, Log::BOTH_ERROR); return (1); }
			if (!(code[0] != '/' && !(code.size() >= 7 && code.compare(0, 7, "http://") == 0) && !(code.size() >= 8 && code.compare(0, 8, "https://") == 0))) {
				Log::log(RD + n_line + "Missing status code for " Y "return" NC, Log::BOTH_ERROR); return (1); }

			long ncode; if (Utils::stol(code, ncode) || (error_codes.find(ncode) == error_codes.end())) {
				Log::log(RD + n_line + "Invalid status code " Y + code + RD " for " Y "return" NC, Log::BOTH_ERROR); return (1); }

			if (!path.empty() && (path[0] != '/' && !(path.size() >= 7 && path.compare(0, 7, "http://") == 0) && !(path.size() >= 8 && path.compare(0, 8, "https://") == 0))) {
				Log::log(RD + n_line + "Invalid path for " Y "return" NC, Log::BOTH_ERROR); return (1); }

			return (0);
		}

	#pragma endregion

	#pragma region Alias

		int Settings::parse_alias(std::string & firstPart, std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			
			if (str.empty()) {			Log::log(RD + n_line + "Empty value for " Y + "alias" + NC, Log::BOTH_ERROR); return (1); }

			if (Utils::isFile(str)) return (parse_path(firstPart, str, true, true));
			else if (Utils::isDirectory(str)) return (parse_path(firstPart, str));
			else if (str[0] == '/') {	Log::log(RD + n_line + "The " Y "alias" RD " path " Y + str + RD " does not exist" NC, Log::BOTH_ERROR); return (1); }
			else {						Log::log(RD + n_line + "Invalid value for " Y "alias" NC, Log::BOTH_ERROR); return (1); }

			return (0);
		}

	#pragma endregion

	#pragma region Try_Files

		int Settings::parse_try_files(std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::istringstream stream(str); std::string code;
			if (str.empty()) {			Log::log(RD + n_line + "Empty value for " Y + "try_files" + NC, Log::BOTH_ERROR); return (1); }

			while (stream >> code) {
				if (!code.empty() && code[0] == '=') {
					long ncode; if (Utils::stol(code.substr(1), ncode) || (error_codes.find(ncode) == error_codes.end())) {
					Log::log(RD + n_line + "Invalid status code " Y + code.substr(1) + RD " for " Y "try_files" NC, Log::BOTH_ERROR); return (1); }
				}
			}

			return (0);
		}

	#pragma endregion

	#pragma region CGI

		int Settings::parse_cgi(const std::string & firstPart, const std::string & secondPart) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::istringstream stream(secondPart); std::vector<std::string> values; std::string value;

			while (stream >> value) values.push_back(value);
			if (values.size() < 2) { Log::log(RD + n_line + "Empty value for " Y "cgi" NC, Log::BOTH_ERROR); BadConfig = true; return (1); }
			std::string filePath = values.back(); values.pop_back();
			if (filePath.empty() || filePath[0] != '/') { Log::log(RD + n_line + "Invalid path for " Y "cgi" NC, Log::BOTH_ERROR); BadConfig = true; }
			if (parse_path(firstPart, filePath, true, true)) { BadConfig = true; }

			for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); ++it) {
				value = *it; if (value.empty() || value[0] != '.') { Log::log(RD + n_line + "Invalid extension " Y + value + RD " for " Y "cgi" NC, Log::BOTH_ERROR); BadConfig = true; }
				else global.add(firstPart + " " + *it, filePath);
			}
			return (0);
		}

		int Settings::parse_cgi(const std::string & firstPart, const std::string & secondPart, VServer & VServ) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::istringstream stream(secondPart); std::vector<std::string> values; std::string value;

			while (stream >> value) values.push_back(value);
			if (values.size() < 2) { Log::log(RD + n_line + "Empty value for " Y "cgi" NC, Log::BOTH_ERROR); BadConfig = true; return (1); }
			std::string filePath = values.back(); values.pop_back();
			if (filePath.empty() || filePath[0] != '/') { Log::log(RD + n_line + "Invalid path for " Y "cgi" NC, Log::BOTH_ERROR); BadConfig = true; }
			if (parse_path(filePath, value, true, true)) { BadConfig = true; }

			for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); ++it) {
				value = *it; if (value.empty() || value[0] != '.') { Log::log(RD + n_line + "Invalid extension " Y + value + RD " for " Y "cgi" NC, Log::BOTH_ERROR); BadConfig = true; }
				else VServ.add(firstPart + " " + *it, filePath);
			}
			return (0);
		}

		int Settings::parse_cgi(const std::string & firstPart, const std::string & secondPart, Location & Loc) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::istringstream stream(secondPart); std::vector<std::string> values; std::string value;

			while (stream >> value) values.push_back(value);
			if (values.size() < 2) { Log::log(RD + n_line + "Empty value for " Y "cgi" NC, Log::BOTH_ERROR); BadConfig = true; return (1); }
			std::string filePath = values.back(); values.pop_back();
			if (filePath.empty() || filePath[0] != '/') { Log::log(RD + n_line + "Invalid path for " Y "cgi" NC, Log::BOTH_ERROR); BadConfig = true; }
			if (parse_path(firstPart, filePath, true, true)) { BadConfig = true; }

			for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); ++it) {
				value = *it; if (value.empty() || value[0] != '.') { Log::log(RD + n_line + "Invalid extension " Y + value + RD " for " Y "cgi" NC, Log::BOTH_ERROR); BadConfig = true; }
				else Loc.add(firstPart + " " + *it, filePath);
			}
			return (0);
		}
	#pragma endregion

	#pragma region Allow

		int Settings::parse_allow(std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::string temp; std::istringstream stream(str); stream >> temp;
			std::string::size_type slashPos;
			
			if (temp.empty()) { 																	Log::log(RD + n_line + "Empty value for " Y "allow" NC, Log::BOTH_ERROR); return (1); }
			if (temp == "all") return (0);
			slashPos = temp.find('/');
			if (slashPos != std::string::npos) {
				std::string ip = temp.substr(0, slashPos);
				std::string mask = temp.substr(slashPos + 1);
				if (ip.empty()) {																	Log::log(RD + n_line + "Invalid IP for " Y "allow" NC, Log::BOTH_ERROR); return (1); }
				if (Utils::isValidIP(temp.substr(0, slashPos)) == false) {							Log::log(RD + n_line + "Invalid IP " Y + temp.substr(0, slashPos) + RD " for " Y "allow" NC, Log::BOTH_ERROR); return (1); }
				if (mask.empty()) {																	Log::log(RD + n_line + "Invalid mask for " Y "allow" NC, Log::BOTH_ERROR); return (1); }
				if (mask.find('.') != std::string::npos) { if (Utils::isValidIP(mask) == false) {	Log::log(RD + n_line + "Invalid mask " Y + temp.substr(slashPos + 1) + RD " for " Y "allow" NC, Log::BOTH_ERROR); return (1); }}
				else { long number; if (Utils::stol(mask, number) || number < 0 || number > 32) {	Log::log(RD + n_line + "Invalid mask " Y + temp.substr(slashPos + 1) + RD " for " Y "allow" NC, Log::BOTH_ERROR); return (1); }}
			} else {
				if (Utils::isValidIP(temp) == false) {												Log::log(RD + n_line + "Invalid IP " Y + temp + RD " for " Y "allow" NC, Log::BOTH_ERROR); return (1); }
			}
			return (0);
		}

	#pragma endregion

	#pragma region Deny

		int Settings::parse_deny(std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			std::string temp; std::istringstream stream(str); stream >> temp;
			std::string::size_type slashPos;
			
			if (temp.empty()) { 																	Log::log(RD + n_line + "Empty value for " Y "deny" NC, Log::BOTH_ERROR); return (1); }
			if (temp == "all") return (0);
			slashPos = temp.find('/');
			if (slashPos != std::string::npos) {
				std::string ip = temp.substr(0, slashPos);
				std::string mask = temp.substr(slashPos + 1);
				if (ip.empty()) {																	Log::log(RD + n_line + "Invalid IP for " Y "deny" NC, Log::BOTH_ERROR); return (1); }
				if (Utils::isValidIP(temp.substr(0, slashPos)) == false) {							Log::log(RD + n_line + "Invalid IP " Y + temp.substr(0, slashPos) + RD " for " Y "deny" NC, Log::BOTH_ERROR); return (1); }
				if (mask.empty()) {																	Log::log(RD + n_line + "Invalid mask for " Y "deny" NC, Log::BOTH_ERROR); return (1); }
				if (mask.find('.') != std::string::npos) { if (Utils::isValidIP(mask) == false) {	Log::log(RD + n_line + "Invalid mask " Y + temp.substr(slashPos + 1) + RD " for " Y "deny" NC, Log::BOTH_ERROR); return (1); }}
				else { long number; if (Utils::stol(mask, number) || number < 0 || number > 32) {	Log::log(RD + n_line + "Invalid mask " Y + temp.substr(slashPos + 1) + RD " for " Y "deny" NC, Log::BOTH_ERROR); return (1); }}
			} else {
				if (Utils::isValidIP(temp) == false) {												Log::log(RD + n_line + "Invalid IP " Y + temp + RD " for " Y "deny" NC, Log::BOTH_ERROR); return (1); }
			}
			return (0);
		}

	#pragma endregion

	#pragma region Limit_Except

		int Settings::parse_limit_except(std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			
			if (str.empty()) {			Log::log(RD + n_line + "Empty value for " Y + "limit_except" NC, Log::BOTH_ERROR); return (1); }

			std::istringstream stream(str); std::string method;

			while (stream >> method) {
				if (method.empty()) {	Log::log(RD + n_line + "Empty method for " Y + "limit_except" NC, Log::BOTH_ERROR); return (1); }
				if (method != "GET" && method != "POST" && method != "DELETE" && method != "PUT" && method != "HEAD") {
					Log::log(RD + n_line + "Invalid method " Y + method + RD " for " Y + "limit_except" NC, Log::BOTH_ERROR); return (1); }
			}

			return (0);
		}

	#pragma endregion

	#pragma region Location

		int Settings::parse_location(std::string & str) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
			
			if (str.empty()) {										Log::log(RD + n_line + "Empty value for " Y "Location" NC, Log::BOTH_ERROR); return (1); }

			std::istringstream stream(str); std::string exact, path;

			stream >> exact; stream >> path;

			if (exact == "=" && path.empty()) {						Log::log(RD + n_line + "Empty value for " Y "Location" NC, Log::BOTH_ERROR); return (1); }
			if (exact != "=" && !path.empty()) {					Log::log(RD + n_line + "Invalid value " Y + exact + RD " for " Y + "Location" NC, Log::BOTH_ERROR); return (1); }
			if (exact.empty()) {									Log::log(RD + n_line + "Empty value for " Y "Location" NC, Log::BOTH_ERROR); return (1); }
			if (exact == "=" && !path.empty() && path[0] != '/') {	Log::log(RD + n_line + "Invalid path " Y + path + RD " for " Y + "Location" NC, Log::BOTH_ERROR); return (1); }
			if (exact != "=" && exact[0] != '/') {					Log::log(RD + n_line + "Invalid value " Y + exact + RD " for " Y + "Location" NC, Log::BOTH_ERROR); return (1); }

			return (0);
		}

	#pragma endregion

	#pragma region Repeated

		static int repeated_directive(const std::string & str, const std::vector<std::pair<std::string, std::string> > & data, int line_count) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";

			if (str.empty() || data.size() == 0 || str == "listen" || str == "allow" || str == "deny") return (0);

			for (std::vector <std::pair<std::string, std::string> >::const_iterator it = data.begin(); it != data.end(); ++it)
				if (it->first == str) { Log::log(RD + n_line + "Directive " Y + str + RD " repeated" NC, Log::BOTH_ERROR); return (1); }

			return (0);
		}

	#pragma endregion

	#pragma region Invalid

		static int invalid_directive(std::string firstPart, int line_count, int section) {
			std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";

			if (firstPart.empty()) return (0);
			if (section == GLOBAL || section == SERVER || section == LOCATION) {
				if (firstPart == "access_log") return (0);
				if (firstPart == "error_log") return (0);
				if (firstPart == "log_days") return (0);
				if (firstPart == "root") return (0);
				if (firstPart == "index") return (0);
				if (firstPart == "uploads") return (0);
				if (firstPart == "client_max_body_size") return (0);
				if (firstPart == "autoindex") return (0);
				if (firstPart == "allow") return (0);
				if (firstPart == "deny") return (0);
				if (firstPart == "error_page") return (0);
				if (firstPart == "cgi") return (0);
			}
			if (section == GLOBAL) {
				if (firstPart == "http") return (0);
			} else if (section == SERVER) {
				if (firstPart == "server") return (0);
				if (firstPart == "server_name") return (0);
				if (firstPart == "listen") return (0);
				if (firstPart == "return") return (0);
			} else if (section == LOCATION) {
				if (firstPart == "location") return (0);
				if (firstPart == "try_files") return (0);
				if (firstPart == "alias") return (0);
				if (firstPart == "internal") return (0);
				if (firstPart == "return") return (0);
			} else if (section == METHOD) {
				if (firstPart == "allow") return (0);
				if (firstPart == "deny") return (0);
				if (firstPart == "limit_except") return (0);
			}
			Log::log(RD + n_line + "Invalid directive " Y + firstPart + NC, Log::BOTH_ERROR); return (1);
		}

	#pragma endregion

#pragma endregion

#pragma region Parser

	void Settings::parser(std::ifstream & infile) {
		std::string line; std::string oline; line_count = 0; VServer VServ; Location Loc; Method Met;
		bool is_http = false; int section = 0; int section_bracket_lvl[4] = {0, 0, 0, 0};

		while (getline(infile, line)) {
			bool NoAdd = false; std::string temp; oline = line; ++line_count; global.config.push_back(oline);
			Utils::trim(line); if (line.empty()) { if (section != ROOT && section != GLOBAL) VServ.config.push_back(oline); continue; }

			while (!line.empty()) { int bracket_mode = 0;
				size_t pos = line.find_first_of("{};");
				if (pos != std::string::npos) {
					temp = line.substr(0, pos + 1);
					line = line.substr(pos + 1);
				} else {
					temp = line; line.clear(); if (temp.empty()) continue;
					if (temp.find("http") != 0 && temp.find("server") != 0 && temp.find("location") != 0 && temp.find("limit_except") != 0) {
						std::string n_line = "[" Y + Utils::ltos(line_count - 1) + RD "] ";
						Log::log(RD + n_line + "Missing '" Y ";" RD "' at the end of the statement" NC, Log::BOTH_ERROR); BadConfig = true; continue;
					}
				}

				Utils::trim(line);
				if (temp[temp.size() - 1] == ';') temp.erase(temp.size() - 1);
				if (temp[temp.size() - 1] == '{') { bracket_mode = 1; temp.erase(temp.size() - 1); }
				if (temp[temp.size() - 1] == '}') { bracket_mode = -1; temp.erase(temp.size() - 1); }
				Utils::trim(temp);

				std::string firstPart, secondPart; std::istringstream stream(temp);
				stream >> firstPart; std::getline(stream, secondPart);
				Utils::trim(firstPart); Utils::toLower(firstPart); Utils::trim(secondPart);

				if (firstPart == "http" && section == ROOT && is_http == false) { section = GLOBAL; section_bracket_lvl[0] = bracket_lvl; Settings::clear(); }
				if (firstPart == "server" && section == GLOBAL) { section = SERVER; section_bracket_lvl[1] = bracket_lvl; VServ.clear(); }
				if (firstPart == "location" && section == SERVER) { section = LOCATION; section_bracket_lvl[2] = bracket_lvl; Loc.clear(); }
				if (firstPart == "limit_except" && section == LOCATION) { section = METHOD; section_bracket_lvl[3] = bracket_lvl; Met.clear(); }

				if (section != ROOT && section != GLOBAL) VServ.config.push_back(oline);

				if (firstPart == "http" && is_http == false) is_http = true;
				else if (firstPart == "http" && is_http == true) invalid_directive(firstPart, line_count, ROOT);
				else if (section == GLOBAL && repeated_directive(firstPart, Settings::global.data, line_count)) 							NoAdd = true;
				else if (section == SERVER && repeated_directive(firstPart, VServ.data, line_count)) 										NoAdd = true;
				else if (section == LOCATION && repeated_directive(firstPart, Loc.data, line_count)) 										NoAdd = true;
				else if (section == METHOD && repeated_directive(firstPart, Met.data, line_count)) 											NoAdd = true;
				else if (invalid_directive(firstPart, line_count, section))	{																NoAdd = true; BadConfig = true; }

				if ((section == GLOBAL || section == SERVER || section == LOCATION) && !firstPart.empty()) {
					if (firstPart == "access_log" || firstPart == "error_log") parse_path(firstPart, secondPart, true, true);
					if (!NoAdd && firstPart == "log_days") parse_log_days(secondPart);
					if (!NoAdd && firstPart == "root" && parse_path(firstPart, secondPart, false, false))									BadConfig = true;
					if (!NoAdd && firstPart == "uploads" && parse_path(firstPart, secondPart, false, false))								BadConfig = true;
					if (!NoAdd && firstPart == "client_max_body_size" && parse_body_size(secondPart))										BadConfig = true;
					if (!NoAdd && firstPart == "autoindex" && parse_autoindex(secondPart))													BadConfig = true;
					if (!NoAdd && firstPart == "index" && parse_index(secondPart))															BadConfig = true;
					if (!NoAdd && firstPart == "allow" && parse_allow(secondPart))															BadConfig = true;
					if (!NoAdd && firstPart == "deny" && parse_deny(secondPart))															BadConfig = true;
				}

				if (section == GLOBAL && !firstPart.empty()) {
					if (!NoAdd && firstPart == "error_page") parse_errors(firstPart, secondPart);
					if (!NoAdd && firstPart == "cgi") parse_cgi(firstPart, secondPart);

					if (!NoAdd && (firstPart == "allow" || firstPart == "deny"))															global.add(firstPart, secondPart, true);
					else if (!NoAdd && firstPart != "http" && firstPart != "error_page" && firstPart != "cgi")								global.add(firstPart, secondPart);
				}

				if (section == SERVER && !firstPart.empty()) {
					if (!NoAdd && firstPart == "listen" && parse_listen(secondPart, VServ))													BadConfig = true;
					if (!NoAdd && firstPart == "return" && parse_return(secondPart))														BadConfig = true;
					if (!NoAdd && firstPart == "error_page") parse_errors(firstPart, secondPart, VServ);
					if (!NoAdd && firstPart == "cgi") parse_cgi(firstPart, secondPart, VServ);

					if (!NoAdd && (firstPart == "listen" || firstPart == "allow" || firstPart == "deny"))									VServ.add(firstPart, secondPart, true);
					else if (!NoAdd && firstPart != "server" && firstPart == "error_page" && firstPart != "listen" && firstPart != "cgi")	VServ.add(firstPart, secondPart);
				}

				if (section == LOCATION && !firstPart.empty()) {
					if (!NoAdd && firstPart == "location" && parse_location(secondPart))													BadConfig = true;
					if (!NoAdd && firstPart == "alias" && parse_alias(firstPart, secondPart))												BadConfig = true;
					if (!NoAdd && firstPart == "try_files" && parse_try_files(secondPart))													BadConfig = true;
					if (!NoAdd && firstPart == "return" && parse_return(secondPart))														BadConfig = true;
					if (!NoAdd && firstPart == "error_page") parse_errors(firstPart, secondPart, Loc);
					if (!NoAdd && firstPart == "cgi") parse_cgi(firstPart, secondPart, VServ);

					if (!NoAdd && (firstPart == "allow" || firstPart == "deny"))															Loc.add(firstPart, secondPart, true);
					else if (!NoAdd && firstPart != "error_page" && firstPart != "cgi")														Loc.add(firstPart, secondPart);
				}

				if (section == METHOD && !firstPart.empty()) {
					if (!NoAdd && firstPart == "limit_except" && parse_limit_except(secondPart))											BadConfig = true;
					if (!NoAdd && firstPart == "allow" && parse_allow(secondPart))															BadConfig = true;
					if (!NoAdd && firstPart == "deny" && parse_deny(secondPart))															BadConfig = true;
					if (!NoAdd && firstPart == "return" && parse_return(secondPart))														BadConfig = true;

					if (!NoAdd && (firstPart == "allow" || firstPart == "deny"))															Met.add(firstPart, secondPart, true);
					else	 																												Met.add(firstPart, secondPart);
				}

				if (bracket_mode != 0) { bracket_lvl += bracket_mode;
					if (section == METHOD && section_bracket_lvl[3] == bracket_lvl) { section = LOCATION; section_bracket_lvl[3] = 0; Loc.add(Met); }
					if (section == LOCATION && section_bracket_lvl[2] == bracket_lvl) { section = SERVER; section_bracket_lvl[2] = 0; VServ.add(Loc); }
					if (section == SERVER && section_bracket_lvl[1] == bracket_lvl) { section = GLOBAL; section_bracket_lvl[1] = 0; Settings::add(VServ); }
					if (section == GLOBAL && section_bracket_lvl[0] == bracket_lvl) { section = ROOT; section_bracket_lvl[0] = 0; }
				}
			}
		}
	}

#pragma endregion