/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 14:30:55 by vzurera-          #+#    #+#             */
/*   Updated: 2024/08/26 22:35:44 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Log.hpp"
#include "Display.hpp"
#include "Settings.hpp"
#include "Net.hpp"

//  *       service nginx reload

//	*		nc 127.0.0.1 8081	-	telnet 127.0.0.1 8081
//	*		curl -v http://localhost:8081/

//	*		siege -c 255 -t 10S 127.0.0.1:8081			135.000 transactions is a good measure

//	?		X-Content-Type-Options: nosniff

//	TODO	Process_requested_path
//	TODO	Variables $request_uri $uri

//	TODO	Verifica el tipo MIME del archivo y el nombre del archivo para asegurarte de que coincide con el tipo esperado.
//	TODO	Limita el tamaño de los archivos que los usuarios pueden cargar para prevenir ataques de denegación de servicio (DoS) mediante la carga de archivos extremadamente grandes
//	TODO	Limita el tamaño máximo de las solicitudes HTTP. Esto puede prevenir que un atacante agote los recursos del servidor con solicitudes grandes.

//  Entry point
int main(int argc, char **argv) {
	Settings::load_args(argc, argv);

	Log::start(); Display::start();
	Net::epoll__create(); Net::socket_create_all();
	usleep(10000); Display::update();

	while (Display::isTerminate() == -1) Net::epoll_events();
	
	Net::epoll_close(); Net::socket_close_all();
	Log::stop(); Display::stop(); Log::release_mutex();
	Display::disableRawMode();

	return (Settings::terminate);
}
