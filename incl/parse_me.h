/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parse_me.h                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/04 23:56:34 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/10/05 00:16:10 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_ME_H
# define PARSE_ME_H

# include <stddef.h>
# include <unistd.h>
# include <stdbool.h>
# include <libext.h>

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

void			process(char *line, t_list *environment);
void			state_squote(char c, t_parser* parser, t_vec* tokens);
void			state_in(char c, t_parser* parser, t_vec* tokens);
int				next_state(t_parser* parse, char c);
void			env_init(t_list **env);
void			print_mem(char *mem, size_t size);
void			state_special(char c, t_parser* parser, t_vec *tokens);
void			print_env(t_list *env);
void			run_state_function(char c, t_parser* parser, t_vec* tokens);
bool			is_valid_env(char c, size_t size);
void			state_dquote(char c, t_parser *parser, t_vec *tokens);
t_keyval		*env_keyval(char *raw);
void			tokenizer(t_vec* tokens, char *line, t_list *environment);
void			env_substitute(t_parser *parser, t_vec *tokens);
void			env_destroy(t_list *env);

#endif
