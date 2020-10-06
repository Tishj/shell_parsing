/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parse_me.h                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/04 23:56:34 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/10/06 14:37:21 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_ME_H
# define PARSE_ME_H

# include <stddef.h>
# include <unistd.h>
# include <stdbool.h>
# include <libext.h>

enum			e_tok
{
	DEFAULT,
	OUTPUT,
	INPUT,
	APP,
	PIPE,
	SEMI,
};

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
