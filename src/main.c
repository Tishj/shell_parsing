/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2020/10/03 19:19:16 by tbruinem      #+#    #+#                 */
/*   Updated: 2020/10/05 00:25:53 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <libext.h>

#include <parse_me.h>

bool		is_valid_env(char c, size_t size)
{
	if ((c == '?' || c == '$') && size <= 1)
		return (true);
	if (c == '_')
		return (true);
	if(c >= '0' && c <= '9')
		return (true);
	if (c >= 'a' && c <= 'z')
		return (true);
	if (c >= 'A' && c <= 'Z')
		return (true);
	return (false);
}

void		print_mem(char *mem, size_t size)
{
	dprintf(2, "size %ld\n", size);
	for (size_t i = 0; i < size; i++)
		dprintf(2, "%2d%c", mem[i], (i + 1 < size) ? ',' : '\n');
}

void		env_substitute(t_parser *parser, t_vec *tokens)
{
	t_list	*iter;
	char	*string;

	parser->env = false;
	iter = parser->environment;
	string = (((t_tok *)tokens->data) + tokens->len - 1)->string.data + parser->env_start;
	while (iter)
	{
		t_keyval	*keyval;

		keyval =  iter->item;
		if (!ft_strncmp("$", string + 1, parser->env_size - 1) && parser->env_size -1 == 1)
			break ;
		if (!ft_strncmp(keyval->key, string + 1, parser->env_size - 1) && parser->env_size - 1 == ft_strlen(keyval->key)) //because of the strncmp it's too greedy
		{
			vec_erase(&(((t_tok *)tokens->data) + tokens->len - 1)->string, parser->env_start, parser->env_size);
			vec_insert(&(((t_tok *)tokens->data) + tokens->len - 1)->string, keyval->val, ft_strlen(keyval->val), parser->env_start);
			return ;
		}
		iter = iter->next;
	}
	vec_erase(&(((t_tok *)tokens->data) + tokens->len - 1)->string, parser->env_start, parser->env_size);
}

t_keyval	*env_keyval(char *raw)
{
	t_keyval	*new;
	char		*split;

	new = malloc(sizeof(t_keyval));
	if (!new)
		return (NULL); //exit;
	split = ft_strchr(raw, '=');
	if (!split)
	{
		free(new);
		return (NULL);
	}
	new->key = malloc(sizeof(char) * (split - raw + 1));
	if (!new->key)
	{
		free(new);
		return (NULL);
	}
	ft_strncpy(new->key, raw, split - raw);
	new->val = ft_strdup(split + 1);
	if (!new->val)
	{
		free(new->key);
		free(new);
		return (NULL);
	}
	return (new);
}

void	env_destroy(t_list *env)
{
	t_list	*last;
	t_keyval	*keyval;

	while (env)
	{
		last = env;
		keyval = env->item;
		free(keyval->key);
		free(keyval->val);
		free(keyval);
		env = env->next;
		free(last);
	}
}

void	print_env(t_list *env)
{
	while (env)
	{
		char	*key;
		char	*val;

		t_keyval*	keyval = env->item;
		key = keyval->key;
		val = keyval->val;
		env = env->next;
	}
}

void	env_init(t_list **env)
{
	extern char	**environ;
	size_t		i;

	i = 0;
	while (environ[i])
	{
		t_keyval	*keyval = env_keyval(environ[i++]);
		ft_lstaddback(env, ft_lstnew(keyval));
	}
}

int		next_state(t_parser* parse, char c)
{
	parse->old_state = parse->state;
	if (parse->state != DQUOTE && parse->state != SQUOTE &&
		(c == '>' || c == '<' || c == '|' || c == ';'))
		return (SPECIAL);
	if (parse->state == SPECIAL && c == '>')
		return (SPECIAL);
	else if (parse->state == SPECIAL && (c == '<' || c == '|' || c == ';'))
		exit (1);
	else if (parse->state == SPECIAL)
	{
		parse->old_state = OUT;
		parse->state = OUT;
	}
	if (parse->state == OUT && c == ' ')
		return (OUT);
	if (parse->state == IN && c == ' ')
		return (OUT);
	if (parse->state == IN && c == '"')
		return (DQUOTE);
	if (parse->state == IN && c == '\'')
		return (SQUOTE);
	if (parse->state == SQUOTE && c == '\'')
		return (NEUTRAL);
	if (parse->state == NEUTRAL && c == ' ')
		return (OUT);
	if ((parse->state == OUT || parse->state == NEUTRAL) && c == '"')
		return (DQUOTE);
	if ((parse->state == OUT || parse->state == NEUTRAL) && c == '\'')
		return (SQUOTE);
	if (parse->state == NEUTRAL && c == '"')
		return (DQUOTE);
	if (parse->state == NEUTRAL && c != ' ')
		return (IN);
	if (parse->state == OUT && c != ' ')
		return (IN);
	if (parse->state == DQUOTE && (c != '"' || parse->escape))
		return (DQUOTE);
	if (parse->state == DQUOTE && c == '"')
		return (NEUTRAL);
	if (parse->state == SQUOTE && c != '\'')
		return (SQUOTE);
	if (parse->state == OUT && c == '\'')
		return (SQUOTE);
	if (parse->state == OUT && c == '"')
		return (DQUOTE);
	return (parse->state);
}

void	state_in(char c, t_parser* parser, t_vec* tokens)
{
	t_tok	tmp;
	t_vec	*string;

	if (parser->old_state == OUT)
	{
		if (tokens->len)
			if (!vec_nullterm(&(((t_tok *)tokens->data) + tokens->len - 1)->string))
				exit(1);
		if (!vec_add(tokens, &tmp, 1))
			exit(1);
		string = &(((t_tok *)tokens->data) + tokens->len - 1)->string;
		if (!vec_new(string, sizeof(char)))
			exit (1);
	}
	string = &(((t_tok *)tokens->data) + tokens->len - 1)->string;
	if (c == '\\' && parser->escape || c != '\\')
		if (!vec_add(string, &c, 1))
			exit(1);
	if (c == '$' && !parser->escape && !parser->env)
	{
		parser->env = true;
		parser->env_size = 0;
		parser->env_start = string->len - 1;
	}
	if (parser->escape)
		parser->escape = false;
	else if (c == '\\')
		parser->escape = true;
	if (parser->env && is_valid_env(c, parser->env_size) && !parser->escape)
		parser->env_size++;
	else if (parser->env)
	{
		env_substitute(parser, tokens);
		if (c == '$' && !parser->escape)
		{
			parser->env = true;
			parser->env_size = 1;
			parser->env_start = string->len - 1;
		}
	}
}

void	state_dquote(char c, t_parser *parser, t_vec *tokens)
{
	t_tok	tmp;
	t_vec	*string;

	if (parser->old_state == OUT)
	{
		if (tokens->len)
			if (!vec_nullterm(&(((t_tok *)tokens->data) + tokens->len - 1)->string))
				exit(1);
		if (!vec_add(tokens, &tmp, 1))
			exit(1);
		string = &(((t_tok *)tokens->data) + tokens->len - 1)->string;
		if (!vec_new(string, sizeof(char)))
			exit (1);
	}
	string = &(((t_tok *)tokens->data) + tokens->len - 1)->string;
	if (c != '"' && c != '\\' || parser->escape)
		if (!vec_add(string, &c, 1))
			exit(1);
	if (c == '$' && !parser->escape && !parser->env)
	{
		parser->env = true;
		parser->env_size = 0;
		parser->env_start = string->len - 1;
	}
	if (parser->escape)
		parser->escape = false;
	else if (c == '\\')
		parser->escape = true;
	if (parser->env && is_valid_env(c, parser->env_size) && !parser->escape)
		parser->env_size++;
	else if (parser->env)
	{
		env_substitute(parser, tokens);
		if (c == '$' && !parser->escape)
		{
			parser->env = true;
			parser->env_size = 0;
			parser->env_start = string->len - 1;
		}
	}
}

void	state_squote(char c, t_parser* parser, t_vec* tokens)
{
	t_tok	tmp;
	t_vec	*string;

	if (parser->env)
		env_substitute(parser, tokens);
	if (parser->old_state == OUT)
	{
		if (tokens->len)
		{
			if (!vec_nullterm(&(((t_tok *)tokens->data) + tokens->len - 1)->string))
				exit(1);
		}
		if (!vec_add(tokens, &tmp, 1))
			exit(1);
		string = &(((t_tok *)tokens->data) + tokens->len - 1)->string;
		if (!vec_new(string, sizeof(char)))
			exit (1);
	}
	string = &(((t_tok *)tokens->data) + tokens->len - 1)->string;
	if (c != '\'')
		if (!vec_add(string, &c, 1))
			exit(1);
	parser->escape = false;
}

void	state_special(char c, t_parser* parser, t_vec *tokens)
{
	t_tok	tmp;
	t_vec	*string;

	if (parser->old_state != SPECIAL)
	{
		if (parser->env)
			env_substitute(parser, tokens);
		if (tokens->len)
		{
			if (!vec_nullterm(&(((t_tok *)tokens->data) + tokens->len - 1)->string))
				exit(1);
		}
		if (!vec_add(tokens, &tmp, 1))
			exit(1);
		string = &(((t_tok *)tokens->data) + tokens->len - 1)->string;
		if (!vec_new(string, sizeof(char)))
			exit (1);
	}
	string = &(((t_tok *)tokens->data) + tokens->len - 1)->string;
	if (!vec_add(string, &c, 1))
		exit(1);
	parser->escape = false;
}

void	run_state_function(char c, t_parser* parser, t_vec* tokens)
{
	t_funct	*funct;
	static t_funct *functions[] = {
	[SPECIAL] = &state_special,
	[NEUTRAL] = NULL,
	[IN] = &state_in,
	[OUT] = NULL,
	[DQUOTE] = &state_dquote,
	[SQUOTE] = &state_squote,
	};

	funct = functions[parser->state];
	if (funct)
		funct(c, parser, tokens);
}

void	tokenizer(t_vec* tokens, char *line, t_list *environment)
{
	t_parser	parser;
	const char	*states [] = {
	"NEUTRAL",
	"IN",
	"OUT",
	"DQUOTE",
	"SQUOTE",
	"SPECIAL",
	};

	parser.index = 0;
	parser.state = OUT;
	parser.environment = environment;
	parser.escape = false;
	parser.env_size = 0;
	parser.env = false;
	while (line && line[parser.index])
	{
		parser.state = next_state(&parser, line[parser.index]);
		dprintf(2, "STATE: %s | ENV: %s\n", states[parser.state], (parser.env) ? "TRUE" : "FALSE");
		run_state_function(line[parser.index], &parser, tokens);
		parser.index++;
	}
	if (tokens->len)
		if (!vec_nullterm(&(((t_tok *)tokens->data) + tokens->len - 1)->string))
			exit(1);
	if (parser.env && parser.env_size)
		env_substitute(&parser, tokens);
}

void	process(char *line, t_list *environment)
{
	t_vec	tokens;

	if (!vec_new(&tokens, sizeof(t_tok)))
		exit(1);
	tokenizer(&tokens, line, environment);
	for (size_t i = 0; i < tokens.len; i++)
	{
		printf("[%ld] = %s\n", i, (((t_tok *)tokens.data) + i)->string.data);
	}
	for (size_t i = 0; i < tokens.len; i++)
		vec_destroy(&((t_tok *)tokens.data + i)->string);
	vec_destroy(&tokens);
}

int	main(void)
{
	char	*line = NULL;
	t_list	*env;
	int		ret;

	env = NULL;
	env_init(&env);
//	print_env(env);
	ret = ft_fdstrc(STDIN_FILENO, &line, '\n');
	while (1)
	{
//		printf("%s\n", line);
		process(line, env);
		free(line);
		line = NULL;
		if (ret == 0)
			break ;
		ret = ft_fdstrc(STDIN_FILENO, &line, '\n');
	}
	env_destroy(env);
	return (0);
}
