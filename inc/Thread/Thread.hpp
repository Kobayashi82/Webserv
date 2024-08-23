/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Thread.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 11:01:21 by vzurera-          #+#    #+#             */
/*   Updated: 2024/08/23 12:23:12 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>																						//	For strings and standard input/output like std::cin, std::cout
#include <pthread.h>

class Thread {

	public:

		enum 	e_action { THRD_CREATE, THRD_JOIN, THRD_DETACH, MTX_INIT, MTX_LOCK, MTX_UNLOCK, MTX_DESTROY };

		static int			thread_set	(pthread_t & thread, int action, void * (*func)(void *) = NULL);

		static int			mutex_set	(pthread_mutex_t & mutex, int action);
		static void 		set_string	(pthread_mutex_t & mutex, std::string & value1, const std::string value2);
		static std::string	get_string	(pthread_mutex_t & mutex, const std::string & value1);
		static void			set_bool	(pthread_mutex_t & mutex, bool & value1, bool value2);
		static bool			get_bool	(pthread_mutex_t & mutex, bool & value1);
		static void			set_int		(pthread_mutex_t & mutex, int & value1, int value2);
		static int			get_int		(pthread_mutex_t & mutex, int & value1);
		static void			set_long	(pthread_mutex_t & mutex, long & value1, long value2);
		static long			get_long	(pthread_mutex_t & mutex, long & value1);
};