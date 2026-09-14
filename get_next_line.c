/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reyam <reyam@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 22:23:53 by reyam             #+#    #+#             */
/*   Updated: 2026/09/14 22:57:33 by reyam            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdio.h>

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

char *get_next_line(int fd)
{
	static char *stash;
	char		*line;
	char		*joined;
	char		*leftover;
	char		*buffer;
	ssize_t		bytes_read;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (NULL);
	if (!stash)
	{
		//since stash is static we only need initialization when its NULL.
		stash = malloc(1);
		if (!stash)
		{
			free(buffer);
			return (NULL);
		}
		stash[0] = '\0';
	}
	while (!ft_strchr(stash, '\n'))
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read <= 0)
		{
			if (bytes_read == -1)
			{
				free(stash);
				free(buffer);
				stash = NULL;
				return (NULL);
			}
			if (bytes_read == 0 && stash[0] == '\0')
			{
				free(stash);
				free(buffer);
				stash = NULL;
				return (NULL);
			}
			else
			{
				line = stash;
				free(buffer);
				stash = NULL;
				return (line);
			}
		}
		//   if read > 0 -> buffer[bytes_read] = '\0'
        //   join stash + buffer
        //   free old stash
        //   stash = joined
        //   loop back and check for '\n'
		//   append logic to stash
		buffer[bytes_read] = '\0';
		joined = ft_strjoin(stash, buffer);
		if (!joined)
		{
			free(stash);
			free(buffer);
			stash = NULL;
			return(NULL);
		}
		free(stash);
		stash = joined;
	}
	line = extract_line(stash);
	if (!line)
	{
		free(stash);
		free(buffer);
		stash = NULL;
		return (NULL);
	}
	leftover = extract_leftover(stash);
	if (!leftover)
	{
		free(line);
		free(stash);
		free(buffer);
		stash = NULL;
		return (NULL);
	}
	free(stash);
	free(buffer);
	stash = leftover;
	return (line);
}
