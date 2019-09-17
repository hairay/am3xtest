#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include "SysDef.h"
#include "read_write_ini.h"

/*****************************************************************
* Function:     read_line()
* Arguments:    <FILE *> fp - a pointer to the file to be read from
*               <char *> bp - a pointer to the copy buffer
* Returns:      TRUE if successful FALSE otherwise
******************************************************************/
static int read_line(FILE *fp, char *bp)
{
    int c = '\0';
    int i = 0;

    /* Read one line from the source file */
    while( (c = getc(fp)) != '\n' )
    {
        if( c == EOF)         /* return FALSE on unexpected EOF */
            return(0);

        bp[i++] = c;
        if(i >= MAX_LINE_LENGTH || c == 0xFF)
            return(0);
    }
    bp[i] = '\0';
    return(1);
}
/************************************************************************
* Function:     get_private_profile_int()
* Arguments:    <char *> section - the name of the section to search for
*               <char *> entry - the name of the entry to find the value of
*               <int> def - the default value in the event of a failed read
*               <char *> file_name - the name of the .ini file to read from
* Returns:      the value located at entry
*************************************************************************/
int get_private_profile_int(char *section, char *entry, int def, char *file_name)
{
    FILE *fp = fopen(file_name,"r");
    char buff[MAX_LINE_LENGTH];
    char *ep;
    char t_section[MAX_LINE_LENGTH];
    char value[12];
    int len = strlen(entry);
    int i;

    if( !fp ) return(0);

    if(section)
    {
        int secLen;
        sprintf(t_section,"[%s]",section); /* Format the section name */
        secLen = strlen(t_section);
        /*  Move through file 1 line at a time until a section is matched or EOF */
        do
        {
            if( !read_line(fp,buff) )
            {
                fclose(fp);
                return(def);
            }
        }
        while( strncmp(buff,t_section,secLen) );
    }
    /* Now that the section has been found, find the entry.
     * Stop searching upon leaving the section's area. */
    do
    {
        //if( !read_line(fp,buff) || buff[0] == '\0' )
        if( !read_line(fp,buff))
        {
            fclose(fp);
            return(def);
        }
    }
    while( strncmp(buff,entry,len) );
    ep = strrchr(buff,'=');    /* Parse out the equal sign */
    ep++;
    if( !strlen(ep) )          /* No setting? */
        return(def);
    /* Copy only numbers fail on characters */

    for(i = 0; isdigit(ep[i]); i++ )
        value[i] = ep[i];
    value[i] = '\0';
    fclose(fp);                /* Clean up and return the value */
    return(atoi(value));
}
/**************************************************************************
* Function:     get_private_profile_string()
* Arguments:    <char *> section - the name of the section to search for
*               <char *> entry - the name of the entry to find the value of
*               <char *> def - default string in the event of a failed read
*               <char *> buffer - a pointer to the buffer to copy into
*               <int> buffer_len - the max number of characters to copy
*               <char *> file_name - the name of the .ini file to read from
* Returns:      the number of characters copied into the supplied buffer
***************************************************************************/
int get_private_profile_string(char *section, char *entry, char *def, char *buffer, int buffer_len, char *file_name)
{
    FILE *fp = fopen(file_name,"r");
    //char buff[MAX_LINE_LENGTH];
    char *buff = NULL;
    size_t length = 0;
    ssize_t nread;
    char *ep;
    char t_section[MAX_LINE_LENGTH];
    int len = strlen(entry), i;

    if( !fp )
    {
        if(buffer_len)
            buffer[0] = 0;
        return(0);
    }
    if(section)
    {
        int secLen;
        sprintf(t_section,"[%s]",section); /* Format the section name */
        secLen = strlen(t_section);
        /*  Move through file 1 line at a time until a section is matched or EOF */
        do
        {
            nread = getline(&buff, &length, fp);
            if(nread == -1)
            {
                fclose(fp);
                strncpy(buffer,def,buffer_len);
                return(strlen(buffer));
            }
        }
        while( strncmp(buff,t_section, secLen) );
    }
    /* Now that the section has been found, find the entry.
     * Stop searching upon leaving the section's area. */
    do
    {
        nread = getline(&buff, &length, fp);
        if(nread == -1)
        {
            fclose(fp);
            strncpy(buffer,def,buffer_len);
            return(strlen(buffer));
        }
    }
    while( strncmp(buff,entry,len) );
    ep = strrchr(buff,'=');    /* Parse out the equal sign */
    ep++;
    /* Copy up to buffer_len chars to buffer */
    strncpy(buffer,ep,buffer_len - 2);
    buffer[buffer_len-1] = '\0';
    len = strlen(buffer);
    for(i=len-1; i>=0; i--)
    {
        if(buffer[i] == '\r' || buffer[i] == '\n' || buffer[i] == 0x20)
            buffer[i] = 0;
        else
            break;
    }
    if(buff)
        free(buff);
    fclose(fp);               /* Clean up and return the amount copied */
    return(strlen(buffer));
}

/*************************************************************************
 * Function:    write_private_profile_string()
 * Arguments:   <char *> section - the name of the section to search for
 *              <char *> entry - the name of the entry to find the value of
 *              <char *> buffer - pointer to the buffer that holds the string
 *              <char *> file_name - the name of the .ini file to read from
 * Returns:     TRUE if successful, otherwise FALSE
 *************************************************************************/
int write_private_profile_string(char *section, char *entry, char *buffer, char *file_name)

{
    struct stat st;
    FILE *rfp, *wfp;
    int handle;
    char tmp_name[PATH_MAX];
    char buff[MAX_LINE_LENGTH];
    char t_section[MAX_LINE_LENGTH];
    int len = strlen(entry);

    sprintf(tmp_name, "%sXXXXXX", file_name);
    if(section)
        sprintf(t_section,"[%s]",section);/* Format the section name */
    if( !(rfp = fopen(file_name,"r")) )  /* If the .ini file doesn't exist */
    {
        if( !(wfp = fopen(file_name,"w")) ) /*  then make one */
        {
            return(0);
        }
        fprintf(wfp,"%s\n",t_section);
        fprintf(wfp,"%s=%s\n",entry,buffer);
        fclose(wfp);
        return(1);
    }
    fstat(fileno(rfp), &st);
    handle = mkstemp(tmp_name); /* Get a temporary file name to copy to */
    fchmod(handle, st.st_mode);
    if( !(wfp = fdopen(handle,"w")) )
    {
        fclose(rfp);
        return(0);
    }

    /* Move through the file one line at a time until a section is
     * matched or until EOF. Copy to temp file as it is read. */
    if(section)
    {
        do
        {
            if( !read_line(rfp,buff) )
            {
                /* Failed to find section, so add one to the end */
                fprintf(wfp,"\n%s\n",t_section);
                fprintf(wfp,"%s=%s\n",entry,buffer);
                /* Clean up and rename */
                fclose(rfp);
                fclose(wfp);
                unlink(file_name);
                rename(tmp_name,file_name);
                return(1);
            }
            fprintf(wfp,"%s\n",buff);
        }
        while( strcmp(buff,t_section) );
    }
    /* Now that the section has been found, find the entry. Stop searching
     * upon leaving the section's area. Copy the file as it is read
     * and create an entry if one is not found.  */
    while( 1 )
    {
        if( !read_line(rfp,buff) )
        {
            /* EOF without an entry so make one */
            fprintf(wfp,"%s=%s\n",entry,buffer);
            /* Clean up and rename */
            fclose(rfp);
            fclose(wfp);
            unlink(file_name);
            rename(tmp_name,file_name);
            return(1);

        }

        //if( !strncmp(buff,entry,len) || buff[0] == '\0' )
        if( !strncmp(buff,entry,len))
            break;
        fprintf(wfp,"%s\n",buff);
    }

    if( buff[0] == '\0' )
    {
        fprintf(wfp,"%s=%s\n",entry,buffer);
        do
        {
            fprintf(wfp,"%s\n",buff);
        }
        while( read_line(rfp,buff) );
    }
    else
    {
        fprintf(wfp,"%s=%s\n",entry,buffer);
        while( read_line(rfp,buff) )
        {
            fprintf(wfp,"%s\n",buff);
        }
    }

    /* Clean up and rename */
    fclose(wfp);
    fclose(rfp);
    unlink(file_name);
    rename(tmp_name,file_name);
    return(1);
}

parser_t* FAST_FUNC config_open2(const char *filename, FILE* FAST_FUNC (*fopen_func)(const char *path))
{
    FILE* fp;
    parser_t *parser;

    fp = fopen_func(filename);
    if (!fp)
        return NULL;
    parser = calloc(sizeof(*parser), 1);
    parser->fp = fp;
    return parser;
}

FILE* FAST_FUNC fopen_for_read(const char *path)
{
    return fopen(path, "r");
}

void FAST_FUNC config_close(parser_t *parser)
{
    if (parser)
    {
        if (PARSE_KEEP_COPY) /* compile-time constant */
            free(parser->data);
        fclose(parser->fp);
        free(parser->line);
        free(parser->nline);
        free(parser);
    }
}

/* This function reads an entire line from a text file,
 * up to a newline, exclusive.
 * Trailing '\' is recognized as line continuation.
 * Returns -1 if EOF/error.
 */
static int get_line_with_continuation(parser_t *parser)
{
    ssize_t len, nlen;
    char *line;

    len = getline(&parser->line, &parser->line_alloc, parser->fp);
    if (len <= 0)
        return len;

    line = parser->line;
    for (;;)
    {
        parser->lineno++;
        if (line[len - 1] == '\n')
            len--;
        if (len == 0 || line[len - 1] != '\\')
            break;
        len--;

        nlen = getline(&parser->nline, &parser->nline_alloc, parser->fp);
        if (nlen <= 0)
            break;

        if (parser->line_alloc < len + nlen + 1)
        {
            parser->line_alloc = len + nlen + 1;
            line = parser->line = realloc(line, parser->line_alloc);
        }
        memcpy(&line[len], parser->nline, nlen);
        len += nlen;
    }

    line[len] = '\0';
    return len;
}


/*
0. If parser is NULL return 0.
1. Read a line from config file. If nothing to read then return 0.
   Handle continuation character. Advance lineno for each physical line.
   Discard everything past comment character.
2. if PARSE_TRIM is set (default), remove leading and trailing delimiters.
3. If resulting line is empty goto 1.
4. Look for first delimiter. If !PARSE_COLLAPSE or !PARSE_TRIM is set then
   remember the token as empty.
5. Else (default) if number of seen tokens is equal to max number of tokens
   (token is the last one) and PARSE_GREEDY is set then the remainder
   of the line is the last token.
   Else (token is not last or PARSE_GREEDY is not set) just replace
   first delimiter with '\0' thus delimiting the token.
6. Advance line pointer past the end of token. If number of seen tokens
   is less than required number of tokens then goto 4.
7. Check the number of seen tokens is not less the min number of tokens.
   Complain or die otherwise depending on PARSE_MIN_DIE.
8. Return the number of seen tokens.

mintokens > 0 make config_read() print error message if less than mintokens
(but more than 0) are found. Empty lines are always skipped (not warned about).
*/
#undef config_read
int FAST_FUNC config_read(parser_t *parser, char **tokens, unsigned flags, const char *delims)
{
    char *line;
    int ntokens, mintokens;
    int t;

    if (!parser)
        return 0;

    ntokens = (uint8_t)flags;
    mintokens = (uint8_t)(flags >> 8);

again:
    memset(tokens, 0, sizeof(tokens[0]) * ntokens);

    /* Read one line (handling continuations with backslash) */
    if (get_line_with_continuation(parser) < 0)
        return 0;

    line = parser->line;

    /* Skip token in the start of line? */
    if (flags & PARSE_TRIM)
        line += strspn(line, delims + 1);

    if (line[0] == '\0' || line[0] == delims[0])
        goto again;

    if (flags & PARSE_KEEP_COPY)
    {
        free(parser->data);
        parser->data = strdup(line);
    }

    /* Tokenize the line */
    t = 0;
    do
    {
        /* Pin token */
        tokens[t] = line;

        /* Combine remaining arguments? */
        if ((t != (ntokens-1)) || !(flags & PARSE_GREEDY))
        {
            /* Vanilla token, find next delimiter */
            line += strcspn(line, delims[0] ? delims : delims + 1);
        }
        else
        {
            /* Combining, find comment char if any */
            line = (char *)strchrnul(line, PARSE_EOL_COMMENTS ? delims[0] : '\0');

            /* Trim any extra delimiters from the end */
            if (flags & PARSE_TRIM)
            {
                while (strchr(delims + 1, line[-1]) != NULL)
                    line--;
            }
        }

        /* Token not terminated? */
        if (*line == delims[0])
            *line = '\0';
        else if (*line != '\0')
            *line++ = '\0';

#if 0 /* unused so far */
        if (flags & PARSE_ESCAPE)
        {
            strcpy_and_process_escape_sequences(tokens[t], tokens[t]);
        }
#endif
        /* Skip possible delimiters */
        if (flags & PARSE_COLLAPSE)
            line += strspn(line, delims + 1);

        t++;
    }
    while (*line && *line != delims[0] && t < ntokens);

    if (t < mintokens)
    {
        printf("bad line %u: %d tokens found, %d needed\n",
               parser->lineno, t, mintokens);
        //if (flags & PARSE_MIN_DIE)
        //    ASSERT(0);
        goto again;
    }

    return t;
}

