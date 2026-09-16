/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reyam <reyam@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 22:23:53 by reyam             #+#    #+#             */
/*   Updated: 2026/09/16 19:19:01 by reyam            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/*
 * Flow of get_next_line:
 *
 * get_next_line() keeps unread data inside the static variable 'stash',
 * so leftover characters survive between consecutive function calls.
 *
 * If stash does not exist yet, get_next_line() allocates an empty string.
 * It then passes stash to read_loop().
 *
 * read_loop() allocates a BUFFER_SIZE + 1 buffer and keeps calling read()
 * while stash does not already contain a '\n'.
 *
 * After every successful read(), append_buffer() adds the newly read
 * characters to stash. append_buffer() first terminates the read buffer
 * with '\0', joins stash and buffer with ft_strjoin(), frees the old stash,
 * and returns the newly allocated joined string.
 *
 * cleanup_read() is used by read_loop() when an error or allocation failure
 * occurs. It frees both stash and buffer and returns NULL.
 *
 * When read_loop() finishes, there are three possible situations:
 *
 * 1. A '\n' exists in stash:
 *    get_next_line() calls extract_lines().
 *
 * 2. EOF was reached but stash still contains characters:
 *    get_next_line() returns the whole stash as the final line and resets
 *    the static stash to NULL.
 *
 * 3. EOF was reached and stash is empty:
 *    read_loop() frees it and returns NULL.
 *
 * extract_lines() receives &stash because it must modify the actual static
 * stash pointer inside get_next_line().
 *
 * extract_lines() calls extract_line() to allocate the line that will be
 * returned to the caller. extract_line() finds the first '\n', allocates
 * enough memory for all characters through that '\n' plus the terminating
 * '\0', copies them, and returns the new line.
 *
 * extract_lines() then calls extract_leftover() to allocate everything
 * after the first '\n'. The old stash is freed and the static stash is
 * updated to point to this leftover string for the next get_next_line()
 * call.
 *
 * Therefore, each call returns exactly one line while preserving any
 * unread characters for the following call.
 */

static char	*cleanup_read(char *stash, char *buffer)
{
	free(stash);
	free(buffer);
	return (NULL);
}

static char	*append_buffer(char *stash, char *buffer, ssize_t bytes_read)
{
	char	*joined;

	buffer[bytes_read] = '\0';
	joined = ft_strjoin(stash, buffer);
	if (!joined)
		return (NULL);
	free(stash);
	return (joined);
}

static char	*read_loop(char *stash, int fd)
{
	char		*buffer;
	ssize_t		bytes_read;
	char		*temp;

	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (cleanup_read(stash, buffer));
	while (!ft_strchr(stash, '\n'))
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0)
			return (cleanup_read(stash, buffer));
		if (bytes_read == 0)
			break ;
		temp = append_buffer(stash, buffer, bytes_read);
		if (!temp)
			return (cleanup_read(stash, buffer));
		stash = temp;
	}
	if (stash[0] == '\0')
		return (cleanup_read(stash, buffer));
	free(buffer);
	return (stash);
}

static char	*extract_lines(char **stash)
{
	char	*line;
	char	*leftover;

	line = extract_line(*stash);
	if (!line)
	{
		free(*stash);
		*stash = NULL;
		return (NULL);
	}
	leftover = extract_leftover(*stash);
	if (!leftover)
	{
		free(line);
		free(*stash);
		*stash = NULL;
		return (NULL);
	}
	free(*stash);
	*stash = leftover;
	return (line);
}

//since stash is static we only need initialization when its NULL.
char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!stash)
	{
		stash = malloc(1);
		if (!stash)
			return (NULL);
		stash[0] = '\0';
	}
	stash = read_loop(stash, fd);
	if (!stash)
		return (NULL);
	if (!ft_strchr(stash, '\n'))
	{
		line = stash;
		stash = NULL;
		return (line);
	}
	return (extract_lines(&stash));
}
