/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parse_me.h                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/03 19:33:52 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/10/04 15:02:45 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_ME_H
# define PARSE_ME_H

# include <stddef.h>
# include <unistd.h>
# include <stdbool.h>
# include <libext.h>

// enum			e_toktype
// {
// 	REDIR_IN,
// 	REDIR_WRITE,
// 	REDIR_APPEND,
// 	FILE,
// 	PIPE,
// 	SEMI,
// };

enum			e_state
{
	NEUTRAL,
	IN,
	OUT,
	DQUOTE,
	SQUOTE,
	SPECIAL,
};

typedef struct	s_parser
{
	size_t		index;
	int			state;
	int			old_state;
	bool		escape;
	bool		env;
	size_t		env_start;
	size_t		env_size;
	t_list		*environment;
}				t_parser;

typedef struct	s_keyval
{
	char		*key;
	char		*val;
}				t_keyval;

typedef struct	s_tok
{
	t_vec		string;
	int			type;
}				t_tok;

typedef void (t_funct)(char, t_parser *, t_vec *);

#endif
