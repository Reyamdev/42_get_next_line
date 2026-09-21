/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reyam <reyam@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 18:53:25 by reyam             #+#    #+#             */
/*   Updated: 2026/09/21 19:31:55 by reyam            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

/*
 * Flow of get_next_line bonus:
 *
 * get_next_line() keeps unread data inside the static array 'stash'.
 * Each file descriptor uses its own stash[fd], so leftover characters
 * from different files remain completely separate between calls.
 *
 * For example:
 * stash[3] stores leftover data belonging to fd 3,
 * stash[4] stores leftover data belonging to fd 4, and so on.
 *
 * If stash[fd] does not exist yet, get_next_line() allocates an empty
 * string for that file descriptor. It then passes stash[fd] to
 * read_loop().
 *
 * read_loop() allocates a BUFFER_SIZE + 1 buffer and keeps calling read()
 * on the given fd while its stash does not already contain a '\n'.
 *
 * After every successful read(), append_buffer() adds the newly read
 * characters to that fd's stash. append_buffer() first terminates the
 * buffer with '\0', joins stash and buffer with ft_strjoin(), frees the
 * old stash, and returns the newly allocated joined string.
 *
 * cleanup_read() is used by read_loop() when a read or allocation fails.
 * It frees the current fd's stash and the temporary read buffer, then
 * returns NULL.
 *
 * When read_loop() finishes, there are three possible situations:
 *
 * 1. A '\n' exists in stash[fd]:
 *    get_next_line() calls extract_lines() for that specific fd.
 *
 * 2. EOF was reached but stash[fd] still contains characters:
 *    get_next_line() returns the whole stash[fd] as the final line and
 *    resets only stash[fd] to NULL.
 *
 * 3. EOF was reached and stash[fd] is empty:
 *    read_loop() frees it and returns NULL.
 *
 * extract_lines() receives &stash[fd] because it must modify the actual
 * pointer stored in the static stash array for that file descriptor.
 *
 * extract_lines() calls extract_line() to allocate the line that will be
 * returned. extract_line() finds the first '\n', allocates enough memory
 * for all characters through that '\n' plus the terminating '\0', copies
 * them, and returns the new line.
 *
 * extract_lines() then calls extract_leftover() to allocate everything
 * after the first '\n'. The old stash[fd] is freed and stash[fd] is
 * updated to point to the leftover string for the next call using the
 * same file descriptor.
 *
 * Because every fd has its own stash[fd], calls can be interleaved:
 * get_next_line(fd1), get_next_line(fd2), get_next_line(fd1), etc.,
 * without mixing the unread data belonging to different files.
 *
 * Therefore, each call returns exactly one line from the requested fd
 * while preserving that fd's unread characters for its following call.
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
	static char	*stash[MAX_FD];
	char		*line;

	if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE <= 0)
		return (NULL);
	if (!stash[fd])
	{
		stash[fd] = malloc(1);
		if (!stash[fd])
			return (NULL);
		stash[fd][0] = '\0';
	}
	stash[fd] = read_loop(stash[fd], fd);
	if (!stash[fd])
		return (NULL);
	if (!ft_strchr(stash[fd], '\n'))
	{
		line = stash[fd];
		stash[fd] = NULL;
		return (line);
	}
	return (extract_lines(&stash[fd]));
}
