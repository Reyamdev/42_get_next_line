*This project has been created as part of the 42 curriculum by rimayer.*

# 42_get_next_line

42 Core project implementing a function that reads and returns one line at a time from a file descriptor.

## TLDR

`get_next_line()` reads and returns one line at a time from a file descriptor.

It uses a static stash to preserve unread data between calls, supports configurable `BUFFER_SIZE`, and handles EOF and memory cleanup correctly.

The bonus version supports multiple file descriptors simultaneously using a separate stash for each `fd`.

## Description

Get Next Line is a project in the 42 Core Curriculum.

The goal of the project is to implement:

```c
char	*get_next_line(int fd);
```

The function reads from a file descriptor and returns one line per call.

A returned line includes the terminating `\n` character when one is present.

If the end of the file is reached without a final newline, the remaining characters are returned as the last line.

When there is nothing left to read, or when an error occurs, the function returns `NULL`.

The project focuses on concepts such as:

* file descriptors
* the `read()` system call
* static variables
* dynamic memory allocation
* memory ownership
* string manipulation
* pointer handling
* buffer management
* EOF and error handling
* preserving data between function calls

The value of `BUFFER_SIZE` determines how many bytes are requested from `read()` at a time and can be changed during compilation.

## Algorithm

The implementation uses a persistent `stash` to store data that has already been read but has not yet been returned.

### Reading

On the first call, `stash` is initialized as an empty allocated string.

`read_loop()` repeatedly calls:

```c
read(fd, buffer, BUFFER_SIZE);
```

until one of the following happens:

* a newline is found in `stash`
* EOF is reached
* `read()` returns an error

After every successful read, the buffer is terminated with `\0`.

`append_buffer()` joins the existing `stash` with the newly read buffer using `ft_strjoin()`.

The old `stash` is then freed and replaced by the newly allocated joined string.

Conceptually:

```text
stash
  +
buffer
  |
  v
ft_strjoin()
  |
  v
new stash
```

Reading stops as soon as a newline is available, so the function does not read the entire file before returning a line.

### Extracting a line

When `stash` contains a newline, `extract_line()` creates a new allocation containing everything from the beginning of `stash` through the first `\n`.

For example:

```text
stash:
"hello\nworld"

returned line:
"hello\n"
```

The newline is included in the returned line.

### Preserving leftover data

`extract_leftover()` creates another allocation containing everything after the first newline.

Using the same example:

```text
original stash:
"hello\nworld"

returned line:
"hello\n"

leftover:
"world"
```

`extract_lines()` coordinates these two operations.

It receives the address of the `stash` pointer so that it can modify the actual static pointer used by `get_next_line()`.

The old stash is freed and replaced with the newly allocated leftover string.

This allows unread data to survive between calls to `get_next_line()`.

### EOF

If EOF is reached and `stash` still contains characters but no newline, the entire remaining `stash` is returned as the final line.

The static `stash` pointer is then reset to `NULL`.

If EOF is reached and `stash` is empty, the allocated memory is freed and `NULL` is returned.

## Bonus

The bonus implementation supports reading from multiple file descriptors at the same time while still using only one static variable.

Instead of a single stash pointer, the bonus version uses an array:

```c
static char	*stash[MAX_FD];
```

Each file descriptor therefore has its own persistent reading state.

For example:

```text
stash[3] -> leftover data for fd 3
stash[4] -> leftover data for fd 4
stash[5] -> leftover data for fd 5
```

This allows calls to be interleaved:

```text
get_next_line(fd1)
get_next_line(fd2)
get_next_line(fd3)
get_next_line(fd1)
get_next_line(fd2)
```

without losing or mixing the unread data belonging to each file descriptor.

The bonus implementation is contained in:

```text
get_next_line_bonus.c
get_next_line_bonus.h
get_next_line_utils_bonus.c
```

## Instructions

### Clone the repository

```bash
git clone https://github.com/Reyamdev/42_get_next_line.git
cd 42_get_next_line
```

### Compilation

Compile the project together with a program that calls `get_next_line()`:

```bash
cc -Wall -Wextra -Werror \
	-D BUFFER_SIZE=42 \
	get_next_line.c get_next_line_utils.c main.c \
	-o gnl_test
```

`BUFFER_SIZE` can be changed during compilation:

```bash
-D BUFFER_SIZE=1
-D BUFFER_SIZE=42
-D BUFFER_SIZE=9999
```

The implementation also provides a default `BUFFER_SIZE` when no value is supplied.

### Example Usage

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("file.txt", O_RDONLY);
	if (fd == -1)
		return (1);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
```

## Testing

The implementation was tested with several types of input, including:

* empty files
* files containing only newlines
* files with a final newline
* files without a final newline
* single-character files
* mixed empty and non-empty lines
* very long lines
* different `BUFFER_SIZE` values

Tested buffer sizes included:

```text
BUFFER_SIZE=1
BUFFER_SIZE=5
BUFFER_SIZE=42
BUFFER_SIZE=9999
```

A long line containing 20,000 characters was also tested with `BUFFER_SIZE=1`.

Memory and undefined-behavior testing was performed using AddressSanitizer and UndefinedBehaviorSanitizer:

```bash
cc -Wall -Wextra -Werror \
	-fsanitize=address,undefined -g \
	-D BUFFER_SIZE=42 \
	get_next_line.c get_next_line_utils.c test.c \
	-o test_gnl_asan
```

## Resources

Useful references for this project include:

* the 42 Get Next Line subject
* `man 2 read`
* `man 2 open`
* C documentation for dynamic memory allocation
* documentation on file descriptors
* documentation on static variables
* AddressSanitizer documentation
* UndefinedBehaviorSanitizer documentation

AI was used as a learning and review aid during the project.

It was used for:

* explaining `read()`, file descriptors, and EOF behavior
* understanding static variables and persistent state
* reviewing edge cases
* designing terminal-based tests
* reviewing the multiple-file-descriptor bonus design
* improving comments and documentation
