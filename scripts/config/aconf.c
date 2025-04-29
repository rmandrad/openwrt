// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Copyright (C) 2024 Weijie Gao <weijie.gao@mediatek.com>
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

#include "lkc.h"

#define CONF_SYM_HASH_SIZE		1024

struct conf_sym_info {
	struct list_head node;
	char *name;
	char *val;
};

struct conf_sym_map {
	struct list_head hash_table[CONF_SYM_HASH_SIZE];
};

static bool merge_mode, diff_mode, min_mode;
static char *out_file, *kconfig_file, *new_file;

static const struct option long_opts[] = {
	{"help",          no_argument,       NULL,            'h'},
	{"merge",         no_argument,       NULL,            'm'},
	{"diff",          no_argument,       NULL,            'd'},
	{"min",           no_argument,       NULL,            'M'},
	{"out",           required_argument, NULL,            'o'},
	{"kconfig",       required_argument, NULL,            'k'},
	{"new",           required_argument, NULL,            'n'},
	{NULL, 0, NULL, 0}
};

static void conf_usage(const char *progname)
{
	printf("Kconfig merging and diff tool\n");
	printf("Usage: %s [options] <config file [config file ...]>\n", progname);
	printf("\n");
	printf("Options:\n");
	printf("  -h, --help                   Print this message and exit.\n");
	printf("  -m, --merge                  Merge config files\n");
	printf("  -d, --diff                   Create config diff file\n");
	printf("  -M, --min                    Write minimum defconfig (merge mode only)\n");
	printf("  -o <file>, --out <file>      Specify output config file\n");
	printf("  -k <file>, --kconfig <file>  Specify Kconfig file\n");
	printf("  -n <file>, --new <file>      Specify new config file for diff\n");
	printf("\n");
	printf("Merge mode: merge all config files to output config file\n");
	printf("Diff mode:  merge all config files as \"old\" config file,\n");
	printf("            then write diff configs between \"old\" and new config file\n");
	printf("            to output config file.\n");
	printf("            The output config file can be merged with \"old\" config file\n");
	printf("            to generate the new config file.\n");
}

static void list_head_init(struct list_head *head)
{
	head->next = head;
	head->prev = head;
}

static struct conf_sym_map *conf_map_new(void)
{
	struct conf_sym_map *csm;
	int i;

	csm = calloc(1, sizeof(struct conf_sym_map));
	if (!csm)
		return NULL;

	for (i = 0; i < CONF_SYM_HASH_SIZE; i++)
		list_head_init(&csm->hash_table[i]);

	return csm;
}

static void conf_map_free(struct conf_sym_map *csm)
{
	struct conf_sym_info *csi, *tmp;
	int i;

	if (!csm)
		return;

	for (i = 0; i < CONF_SYM_HASH_SIZE; i++) {
		list_for_each_entry_safe(csi, tmp, &csm->hash_table[i], node) {
			list_del(&csi->node);
			free(csi->name);
			if (csi->val)
				free(csi->val);
			free(csi);
		}
	}

	free(csm);
}

static struct conf_sym_info *conf_map_find(struct conf_sym_map *csm,
					   const char *name)
{
	struct conf_sym_info *csi;
	int hash;

	if (!csm || !name)
		return NULL;

	hash = strhash(name) % CONF_SYM_HASH_SIZE;

	list_for_each_entry(csi, &csm->hash_table[hash], node) {
		if (!strcmp(csi->name, name))
			return csi;
	}

	return NULL;
}

static void conf_map_set(struct conf_sym_map *csm, const char *name,
			 const char *val)
{
	struct conf_sym_info *csi;
	int hash;

	if (!csm || !name)
		return;

	if (!val)
		val = "";

	csi = conf_map_find(csm, name);
	if (csi) {
		if (csi->val) {
			free(csi->val);
			csi->val = NULL;
		}

		if (val)
			csi->val = xstrdup(val);
		return;
	}

	csi = xcalloc(1, sizeof(*csi));
	list_head_init(&csi->node);
	csi->name = xstrdup(name);
	if (val)
		csi->val = xstrdup(val);

	hash = strhash(name) % CONF_SYM_HASH_SIZE;

	list_add_tail(&csi->node, &csm->hash_table[hash]);
}

static struct conf_sym_map *conf_map_dup(struct conf_sym_map *csm)
{
	struct conf_sym_info *csi, *csi_new;
	struct conf_sym_map *csm_new;
	int i;

	if (!csm)
		return conf_map_new();

	csm_new = conf_map_new();

	for (i = 0; i < CONF_SYM_HASH_SIZE; i++) {
		list_for_each_entry(csi, &csm->hash_table[i], node) {
			csi_new = xcalloc(1, sizeof(*csi_new));
			list_head_init(&csi_new->node);
			csi_new->name = xstrdup(csi->name);
			if (csi->val)
				csi_new->val = xstrdup(csi->val);
			list_add_tail(&csi_new->node, &csm_new->hash_table[i]);
		}
	}

	return csm_new;
}

static int conf_map_read(struct conf_sym_map *csm, const char *name)
{
	size_t line_asize = 0;
	char *line = NULL;
	char *p, *p2;
	FILE *in;

	in = zconf_fopen(name);
	if (!in)
		return 1;

	while (compat_getline(&line, &line_asize, in) != -1) {
		if (line[0] == '#') {
			if (memcmp(line + 2, CONFIG_, strlen(CONFIG_)))
				continue;
			p = strchr(line + 2 + strlen(CONFIG_), ' ');
			if (!p)
				continue;
			*p++ = 0;
			if (strncmp(p, "is not set", 10))
				continue;

			conf_map_set(csm, line + 2 + strlen(CONFIG_), "n");
		} else if (memcmp(line, CONFIG_, strlen(CONFIG_)) == 0) {
			p = strchr(line + strlen(CONFIG_), '=');
			if (!p)
				continue;
			*p++ = 0;
			p2 = strchr(p, '\n');
			if (p2) {
				*p2-- = 0;
				if (*p2 == '\r')
					*p2 = 0;
			}

			conf_map_set(csm, line + strlen(CONFIG_), p);
		}
	}

	free(line);
	fclose(in);

	return 0;
}

static int conf_map_read_multi(struct conf_sym_map *csm, char *files[],
			       size_t count)
{
	size_t i;

	for (i = 0; i < count; i++) {
		if (conf_map_read(csm, files[i])) {
			fprintf(stderr, "Unable to read config file '%s'\n",
				files[i]);
			return 1;
		}
	}

	return 0;
}

static void conf_map_gen_snapshot(struct conf_sym_map *csm)
{
	struct symbol *sym;
	struct menu *menu;
	int i;

	sym_clear_all_valid();

	for_all_symbols(i, sym)
		sym->flags &= ~SYMBOL_WRITTEN;

	menu = rootmenu.list;
	while (menu) {
		sym = menu->sym;
		if (sym) {
			if (!(sym->flags & SYMBOL_CHOICE) &&
			    !(sym->flags & SYMBOL_WRITTEN)) {
				sym_calc_value(sym);
				if (sym->flags & SYMBOL_WRITE) {
					sym->flags |= SYMBOL_WRITTEN;

					conf_map_set(csm, sym->name,
						     sym_get_string_value(sym));
				}
			}
		}

		if (menu->list) {
			menu = menu->list;
			continue;
		}

end_check:
		if (menu->next) {
			menu = menu->next;
		} else {
			menu = menu->parent;
			if (menu)
				goto end_check;
		}
	}

	for_all_symbols(i, sym)
		sym->flags &= ~SYMBOL_WRITTEN;
}

static void conf_set_all_new_symbols_default(void)
{
	struct symbol *sym, *csym;
	int i;

	sym_clear_all_valid();

	/*
	 * We have different type of choice blocks.
	 * If curr.tri equals to mod then we can select several
	 * choice symbols in one block.
	 * In this case we do nothing.
	 * If curr.tri equals yes then only one symbol can be
	 * selected in a choice block and we set it to yes,
	 * and the rest to no.
	 */
	for_all_symbols(i, csym) {
		if ((sym_is_choice(csym) && !sym_has_value(csym)) ||
			sym_is_choice_value(csym))
			csym->flags |= SYMBOL_NEED_SET_CHOICE_VALUES;
	}

	for_all_symbols(i, csym) {
		if (sym_has_value(csym) || !sym_is_choice(csym))
			continue;

		sym_calc_value(csym);
		set_all_choice_values(csym);
	}
}

static int conf_set_raw(const char *name, const char *val, int def)
{
	struct symbol *sym;
	int ret, def_flags;
	char *s;

	def_flags = SYMBOL_DEF << def;

	sym = sym_find(name);
	if (!sym)
		return 0;

	s = xstrdup(val);
	ret = conf_set_sym_val(sym, def, def_flags, s);
	free(s);

	if (ret)
		return 1;

	if (sym && sym_is_choice_value(sym)) {
		struct symbol *cs = prop_get_symbol(sym_get_choice_prop(sym));
		switch (sym->def[def].tri) {
		case no:
			break;
		case mod:
			if (cs->def[def].tri == yes) {
				cs->flags &= ~def_flags;
			}
			break;
		case yes:
			cs->def[def].val = sym;
			break;
		}
		cs->def[def].tri = EXPR_OR(cs->def[def].tri, sym->def[def].tri);
	}

	return 0;
}

static void conf_set_raw_from_map(const struct conf_sym_map *csm)
{
	struct conf_sym_info *csi;
	int i;

	for (i = 0; i < CONF_SYM_HASH_SIZE; i++) {
		list_for_each_entry(csi, &csm->hash_table[i], node) {
			conf_set_raw(csi->name, csi->val, S_DEF_USER);
		}
	}
}

static int conf_finish_merge(void)
{
	int conf_unsaved = 0;
	struct symbol *sym;
	int i;

	sym_calc_value(modules_sym);

	for_all_symbols(i, sym) {
		sym_calc_value(sym);
		if (sym_is_choice(sym) || (sym->flags & SYMBOL_NO_WRITE))
			continue;
		if (sym_has_value(sym) && (sym->flags & SYMBOL_WRITE)) {
			/* check that calculated value agrees with saved value */
			switch (sym->type) {
			case S_BOOLEAN:
			case S_TRISTATE:
				if (sym->def[S_DEF_USER].tri == sym_get_tristate_value(sym))
					continue;
				break;
			default:
				if (!strcmp(sym->curr.val, sym->def[S_DEF_USER].val))
					continue;
				break;
			}
		} else if (!sym_has_value(sym) && !(sym->flags & SYMBOL_WRITE))
			/* no previous value and not saved */
			continue;
		conf_unsaved++;
		/* maybe print value in verbose mode... */
	}

	for_all_symbols(i, sym) {
		if (sym_has_value(sym) && !sym_is_choice_value(sym)) {
			/* Reset values of generates values, so they'll appear
			 * as new, if they should become visible, but that
			 * doesn't quite work if the Kconfig and the saved
			 * configuration disagree.
			 */
			if (sym->visible == no && !conf_unsaved)
				sym->flags &= ~SYMBOL_DEF_USER;
			switch (sym->type) {
			case S_STRING:
			case S_INT:
			case S_HEX:
				/* Reset a string value if it's out of range */
				if (sym_string_within_range(sym, sym->def[S_DEF_USER].val))
					break;
				sym->flags &= ~(SYMBOL_VALID|SYMBOL_DEF_USER);
				conf_unsaved++;
				break;
			default:
				break;
			}
		}
	}

	return 0;
}

static void conf_loaddef_from_map(struct conf_sym_map *csm_list[],
				  size_t count)
{
	size_t i;

	/* Perform load defconfig file */
	conf_set_changed(false);

	conf_reset(S_DEF_USER);

	for (i = 0; i < count; i++)
		conf_set_raw_from_map(csm_list[i]);

	conf_finish_merge();
	conf_set_all_new_symbols_default();
}

static int conf_loaddef(char *files[], size_t count,
			struct conf_sym_map **retcsm)
{
	struct conf_sym_map *csm;

	csm = conf_map_new();

	if (conf_map_read_multi(csm, files, count)) {
		conf_map_free(csm);
		return 1;
	}

	conf_loaddef_from_map(&csm, 1);

	if (retcsm)
		*retcsm = csm;
	else
		conf_map_free(csm);

	return 0;
}

static int do_conf_merge(int argc, char *argv[])
{
	int ret;

	if (conf_loaddef(argv, argc, NULL))
		return 1;

	if (min_mode) {
		ret = conf_write_defconfig(out_file);
		if (!ret) {
			printf("#\n# ");
			printf("minimum configuration written to %s", out_file);
			printf("\n#\n");
		}
	} else {
		setenv("KCONFIG_OVERWRITECONFIG", "1", 1);
		ret = conf_write(out_file);
	}

	if (ret) {
		fprintf(stderr, "\n*** Error during writing of the configuration.\n\n");
		return 1;
	}

	return 0;
}

static int conf_cmp(const char *s1, const char *s2)
{
	if ((s1 && !s2) || (!s1 && s2))
		return -1;

	if (!s1 && !s2)
		return 0;

	return strcmp(s1, s2);
}

static size_t conf_diff_to_map(struct conf_sym_map *csm,
			       struct conf_sym_map *csm_old)
{
	const char *old_val, *new_val;
	struct conf_sym_info *csi;
	struct symbol *sym;
	struct menu *menu;
	size_t count = 0;

	sym_clear_all_valid();

	/* Traverse all menus to find all relevant symbols */
	menu = rootmenu.list;

	while (menu != NULL) {
		sym = menu->sym;
		if (sym == NULL) {
			if (!menu_is_visible(menu))
				goto next_menu;
		} else if (!sym_is_choice(sym)) {
			sym_calc_value(sym);
			if (!(sym->flags & SYMBOL_WRITE))
				goto next_menu;
			sym->flags &= ~SYMBOL_WRITE;
			/* If we cannot change the symbol - skip */
			if (!sym_is_changeable(sym))
				goto next_menu;
			/* If symbol equals to old value - skip */
			csi = conf_map_find(csm_old, sym->name);
			if (!csi)
				old_val = NULL;
			else
				old_val = csi->val;
			new_val = sym_get_string_value(sym);

			if (!conf_cmp(old_val, new_val))
				goto next_menu;

			if (sym_is_choice_value(sym)) {
				struct symbol *cs;
				struct symbol *ds;

				cs = prop_get_symbol(sym_get_choice_prop(sym));
				ds = sym_choice_default(cs);
				if (!sym_is_optional(cs) && sym == ds) {
					if ((sym->type == S_BOOLEAN) &&
					    sym_get_tristate_value(sym) == yes)
						goto next_menu;
				}
			}

			conf_map_set(csm, sym->name, new_val);
			count++;
		}
next_menu:
		if (menu->list != NULL) {
			menu = menu->list;
		}
		else if (menu->next != NULL) {
			menu = menu->next;
		} else {
			while ((menu = menu->parent)) {
				if (menu->next != NULL) {
					menu = menu->next;
					break;
				}
			}
		}
	}

	return count;
}

static int conf_write_diff(struct conf_sym_map *csm_changed,
			   struct conf_sym_map *csm_old,
			   const char *filename)
{
	struct conf_sym_info *csi;
	const char *val_curr;
	struct symbol *sym;
	struct menu *menu;
	FILE *out;

	out = fopen(filename, "w");
	if (!out)
		return 1;

	sym_clear_all_valid();

	/* Traverse all menus to find all relevant symbols */
	menu = rootmenu.list;

	while (menu != NULL) {
		sym = menu->sym;
		if (sym == NULL) {
			if (!menu_is_visible(menu))
				goto next_menu;
		} else if (!sym_is_choice(sym)) {
			sym_calc_value(sym);
			if (!(sym->flags & SYMBOL_WRITE))
				goto next_menu;
			sym->flags &= ~SYMBOL_WRITE;
			/* If we cannot change the symbol - skip */
			if (!sym_is_changeable(sym))
				goto next_menu;
			/* If symbol not changed - skip */
			if (!conf_map_find(csm_changed, sym->name))
				goto next_menu;

			/* If symbol equals to both default and "old" value - skip */
			val_curr = sym_get_string_value(sym);
			csi = conf_map_find(csm_old, sym->name);
			if (!strcmp(val_curr, sym_get_string_default(sym)) &&
			    (!csi || (csi && !strcmp(val_curr, csi->val))))
				goto next_menu;

			if (sym_is_choice_value(sym)) {
				struct symbol *cs;
				struct symbol *ds;

				cs = prop_get_symbol(sym_get_choice_prop(sym));
				ds = sym_choice_default(cs);
				if (!sym_is_optional(cs) && sym == ds) {
					if ((sym->type == S_BOOLEAN) &&
					    sym_get_tristate_value(sym) == yes)
						goto next_menu;
				}
			}

			print_symbol_for_dotconfig(out, sym);
		}
next_menu:
		if (menu->list != NULL) {
			menu = menu->list;
		}
		else if (menu->next != NULL) {
			menu = menu->next;
		} else {
			while ((menu = menu->parent)) {
				if (menu->next != NULL) {
					menu = menu->next;
					break;
				}
			}
		}
	}
	fclose(out);
	return 0;
}

static int do_conf_diff(int argc, char *argv[])
{
	struct conf_sym_map *csm_old, *csm_old_orig, *csm_old_min, *csm_new, *csm_changed, *list[2];
	size_t n;
	int ret;

	csm_changed = conf_map_new();

	/* Merge all config files as original "old" config */
	if (conf_loaddef(argv, argc, &csm_old_min))
		return 1;

	/* Record all "old" config values */
	csm_old = conf_map_new();
	conf_map_gen_snapshot(csm_old);
	csm_old_orig = conf_map_dup(csm_old);

	/* Perform load defconfig for "new" config */
	if (conf_loaddef(&new_file, 1, &csm_new))
		return 1;

	/* Calculates incremental symbols from original "old" to "new" config */
	while (true) {
		n = conf_diff_to_map(csm_changed, csm_old);
		if (!n) {
			/* Until there's no new changed symbol */
			break;
		}

		/* Perform load defconfig for original "min" + current "diff" configs */
		list[0] = csm_old_min;
		list[1] = csm_changed;
		conf_loaddef_from_map(list, 2);

		/* Record all current "old" config values */
		conf_map_free(csm_old);
		csm_old = conf_map_new();
		conf_map_gen_snapshot(csm_old);

		/* Repeat performing load defconfig for "new" config */
		conf_loaddef_from_map(&csm_new, 1);
	}

	/* Generate final defconfig file */
	conf_write_diff(csm_changed, csm_old_orig, out_file);

	printf("#\n# ");
	printf("incremental configuration written to %s", out_file);
	printf("\n#\n");

	return 0;
}

int main(int argc, char *argv[])
{
	char *progname;
	int opt;

	progname = strrchr(argv[0], '/');
	if (progname)
		progname++;
	else
		progname = argv[0];

	while ((opt = getopt_long(argc, argv, "hmdMo:k:n:", long_opts, NULL)) != -1) {
		switch (opt) {
		case 'h':
			conf_usage(progname);
			return 0;

		case 'm':
			merge_mode = true;
			break;

		case 'd':
			diff_mode = true;
			break;

		case 'M':
			min_mode = true;
			break;

		case 'o':
			out_file = optarg;
			break;

		case 'k':
			kconfig_file = optarg;
			break;

		case 'n':
			new_file = optarg;
			break;

		default:;
		}
	}

	if (optind == argc) {
		fprintf(stderr, "%s: Missing config file\n", progname);
		goto err_usage;
	}

	if (!merge_mode && !diff_mode) {
		fprintf(stderr, "%s: Operation mode (merge or diff) not specified\n", progname);
		goto err_usage;
	}

	if (merge_mode && diff_mode) {
		fprintf(stderr, "%s: Only one operation mode can be specified\n", progname);
		goto err_usage;
	}

	if (diff_mode && !new_file) {
		fprintf(stderr, "%s: New config file not specified in diff mode\n", progname);
		goto err_usage;
	}

	if (!out_file) {
		fprintf(stderr, "%s: Output file not specified\n", progname);
		goto err_usage;
	}

	if (!kconfig_file) {
		fprintf(stderr, "%s: Kconfig file not specified\n", progname);
		goto err_usage;
	}

	conf_parse(kconfig_file);

	if (merge_mode)
		return do_conf_merge(argc - optind, argv + optind);

	return do_conf_diff(argc - optind, argv + optind);

err_usage:
	conf_usage(progname);

	return 1;
}
