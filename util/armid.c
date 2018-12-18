/*
 *    armid - ARM part identifier utility
 *    Copyright (C) 2018 Burt P. <pburt0@gmail.com>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, version 2.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define msg(...) fprintf (stderr, __VA_ARGS__)

/* for **imp_str and **part_str:
 *   null : ignored
 *   ->null : set to newly allocated string, containing result, that must be free()d
 *   ->address : result strcpy()d into buffer at address
 */
int scan_armids_file(const char *armids_file, int implementer, int part, char **imp_str, char **part_str) {
#define ARM_ID_BUFF_SIZE 128
    char buff[ARM_ID_BUFF_SIZE];
    char imp[ARM_ID_BUFF_SIZE] = "", *prt = NULL, *p;
    FILE *fd;
    int tabs, imp_match = 0;
    int id;

    fd = fopen(armids_file, "r");
    if (!fd) return 0;

    while (fgets(buff, ARM_ID_BUFF_SIZE, fd)) {
        /* line ends at comment */
        p = strchr(buff, '#');
        if (p) *p = 0;

        /* trim trailing white space */
        p = buff + strlen(buff) - 1;
        while(p > buff && isspace(*p)) p--;
        *(p+1) = 0;

        /* scan */
        tabs = 0;
        p = buff;
        while(*p == '\t') { tabs++; p++; }
        id = strtol(p, &p, 16);
        while(isspace(*p)) p++;
        if (tabs == 0) {
            /* implementer */
            if (id == implementer) {
                strcpy(imp, p);
                imp_match = 1;
            } else if (id > implementer)
                goto scan_arm_id_done;
        } else if (tabs == 1 && imp_match) {
            /* part */
            if (id == part) {
                prt = p;
                goto scan_arm_id_done;
            } else if (id > part)
                goto scan_arm_id_done;
        }
    }

scan_arm_id_done:
    fclose(fd);

    if (imp_match) {
        if (imp_str) {
            if (*imp_str)
                strcpy(*imp_str, imp);
            else
                *imp_str = strdup(imp);
        }
        if (part_str && prt) {
            if (*part_str)
                strcpy(*part_str, prt);
            else
                *part_str = strdup(prt);
        }
        return 1;
    }
    return 0;
}

static void usage(const char* name) {
    msg("Usage:\n"
        "%s [options] [implementer [part]]\n", name);
    msg("Options:\n"
        "    -h\t\t display usage information\n"
        "    -d <file>\t location of arm.ids file\n"
        );
}

static const char armids_file_loc[] = "../arm.ids";

int main(int argc, char *argv[]) {
    char *imp_str = NULL, *part_str = NULL;
    int found = 0;

    const char *armids_file = armids_file_loc;
    int in_imp = -1, in_part = -1;

    int c;
    int opt_help = 0;
    while ((c = getopt(argc, argv, "d:h")) != -1) {
        switch (c) {
            case 'd':
                armids_file = optarg;
                break;
            case 'h':
                opt_help = 1;
                break;
            default:
                usage(argv[0]);
                return 1;
        }
    }

    if (argc - optind >= 1)
        in_imp = strtol(argv[optind], NULL, 16);
    if (argc - optind >= 2)
        in_part = strtol(argv[optind+1], NULL, 16);

    if (in_imp == -1) {
        usage(argv[0]);
        return 2;
    }

    if (opt_help) {
        usage(argv[0]);
        return 0;
    }

    if (access(armids_file, R_OK)) {
        msg("error: file unreadable: %s\n", armids_file);
        return 2;
    }

    found = scan_armids_file(armids_file, in_imp, in_part, &imp_str, &part_str);
    if (found) {
        if (part_str)
            printf("found: implementer[%02x]='%s' part[%03x]='%s'\n", in_imp, imp_str, in_part, part_str);
        else if (in_part < 0)
            printf("found: implementer[%02x]='%s'\n", in_imp, imp_str);
        else
            printf("found: implementer[%02x]='%s' part[%03x]=unknown\n", in_imp, imp_str, in_part);
    } else
        printf("not found\n");

    free(imp_str);
    free(part_str);

    return (found) ? 0 : 1;
}
