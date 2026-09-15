/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reyam <reyam@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 22:23:53 by reyam             #+#    #+#             */
/*   Updated: 2026/09/15 23:09:49 by reyam            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdio.h>
#include <sys/types.h>

//  For a static local variable, it keeps its value between function calls
//  instead of being recreated each time.

// Keep reading into a static string,
// check for \n with strchr every loop to see if we need to read again or not.

// EOF + stash has data -> return remaining data
// EOF + stash empty -> return NULL;

// bytes_read = actual useful data length
// BUFFER_SIZE = maximum possible read size

//read() == -1 --> read error: free(stash), stash = NULL, return stash;

/* get_next_line(fd)

1. If stash == NULL
      ↓
   initialize stash as ""

2. Check stash for '\n'

3. If there is NO '\n'
      ↓
   read(fd, buffer, BUFFER_SIZE)

4. Handle read() result

   -1  → error
          free stash
          stash = NULL
          return NULL

    0  → EOF
          if stash is empty:
              free stash
              stash = NULL
              return NULL

          if stash still has characters:
              line = stash
              stash = NULL
              return line

   >0  → buffer[bytes_read] = '\0'
          join stash + buffer
          free old stash
          stash = joined
          loop back and check for '\n'

5. Once stash DOES contain '\n'
      ↓
   extract_line(stash)
      ↓
   "hello\n"

6. extract_leftover(stash)
      ↓
   "world"

7. Free old stash

8. stash = new leftover

9. return line
*/

// 1. Search stash for '\n'
// 2. Append newly read buffer to stash
// 3. Extract the line to return
// 4. Build/update the leftover stash after that line

//   if read > 0 -> buffer[bytes_read] = '\0'
//   join stash + buffer
//   free old stash
//   stash = joined
//   loop back and check for '\n'
//   append logic to stash

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
