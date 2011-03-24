/*
 * tinysh.h
 *
 * Header for minimal portable shell
 *
 * Copyright (C) 2001 Michel Gutierrez <mig@nerim.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

typedef void (*tinysh_fnt_t)(int argc, char **argv);

typedef struct tinysh_cmd_t {
  struct tinysh_cmd_t *parent; /* 0 if top level command */
  char *name;                  /* command input name, not 0 */
  char *help;                  /* help string, can be 0 */
  char *usage;                 /* usage string, can be 0 */
  tinysh_fnt_t function;       /* function to launch on cmd, can be 0 */
  void *arg;                   /* current argument when function called */
  struct tinysh_cmd_t *next;   /* must be set to 0 at init */
  struct tinysh_cmd_t *child;  /* must be set to 0 at init */
} tinysh_cmd_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * function void tinysh_char_out(unsigned char) must be provided by
 * the application
 */
void tinysh_char_out(unsigned char c);

/*
 * Functions below are provided by the tinysh module
 */

/* new character input */
void tinysh_char_in(unsigned char c);

/* add a new command */
void tinysh_add_command(tinysh_cmd_t *cmd);

/* change tinysh prompt */
void tinysh_set_prompt(char *str);

/* get command argument back */
void *tinysh_get_arg();

/* provide conversion string to scalar (decimal or hexadecimal) */
unsigned long tinysh_atoxi(char *s);

#ifdef __cplusplus
}
#endif

