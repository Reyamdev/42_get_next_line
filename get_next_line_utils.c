/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: reyam <reyam@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 22:23:46 by reyam             #+#    #+#             */
/*   Updated: 2026/09/15 15:41:57 by reyam            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stddef.h>

/*
 * Returns the number of characters in 'str',
 * excluding the terminating '\0'.
 */

size_t	ft_strlen(const char *str)
{
	size_t	len;

	len = 0;
	while (str[len])
		len++;
	return (len);
}

/*
 * Returns a pointer to the first occurrence of 'c' in 's'.
 * The terminating '\0' is also considered part of the string,
 * so searching for '\0' returns a pointer to the terminator.
 * Returns NULL if 'c' is not found.
 */

char	*ft_strchr(const char *s, int c)
{
	while (1)
	{
		if (*s == (char)c)
			return ((char *)s);
		if (*s == '\0')
			return (NULL);
		s++;
	}
}

/*
 * Allocates and returns a new string containing 's1' followed by 's2'.
 * Returns NULL if allocation fails.
 */

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	str = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!str)
		return (NULL);
	i = 0;
	j = 0;
	while (s1[j])
		str[i++] = s1[j++];
	j = 0;
	while (s2[j])
		str[i++] = s2[j++];
	str[i] = '\0';
	return (str);
}

/*
 * Creates and returns a new string containing the first line from 'stash',
 * including the '\n' when present, and adds a terminating '\0'.
 */
char	*extract_line(char *stash)
{
	size_t	i;
	size_t	j;
	char	*line;

	i = 0;
	while (stash && stash[i] != '\n')
		i++;
	line = malloc(sizeof(char) * (i + 2));
	if (!line)
		return (NULL);
	line[i + 1] = '\0';
	j = 0;
	while (j <= i)
	{
		line[j] = stash[j];
		j++;
	}
	return (line);
}

/*
 * Creates and returns a new string containing everything after the first '\n'
 * in 'stash'. Returns an empty allocated string if nothing remains.
 */
char	*extract_leftover(char *stash)
{
	char	*n_pos;
	size_t	len;
	char	*leftover;
	size_t	i;

	n_pos = ft_strchr(stash, '\n');
	n_pos++;
	len = ft_strlen(n_pos);
	leftover = malloc(sizeof(char) * (len + 1));
	if (!leftover)
		return (NULL);
	i = 0;
	while (n_pos[i])
	{
		leftover[i] = n_pos[i];
		i++;
	}
	leftover[len] = '\0';
	return (leftover);
}
