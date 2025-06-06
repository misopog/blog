#include "md4c.h"
#include "md4c-html.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define TEMPLATE_FOLDER "template/"
#define POSTS_FOLDER "posts/"
#define OUTPUT_FOLDER "output/"
#define TEMP_FOLDER "temp/"
#define MAX_POSTS 128
#define MAX_PATH 512
#define MAX_TITLE 128
#define MAX_DATE 32

typedef struct {
    char filename[MAX_PATH];
    char title[MAX_TITLE];
    char date[MAX_DATE];
    char html_content[8192];
} Post;

Post posts[MAX_POSTS];
int post_count = 0;

// make a folder if it doesn't exist
void make_folder(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

// format html using tidy 
void tidy_html(const char* filepath) {
    char cmd[MAX_PATH + 128];
    snprintf(cmd, sizeof(cmd), "tidy -indent -modify -quiet \"%s\"", filepath);
    system(cmd);
}

// read file into buffer (caller frees)
char* read_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char* buf = malloc(len + 1);
    if (!buf) { fclose(f); return NULL; }
    fread(buf, 1, len, f);
    buf[len] = 0;
    fclose(f);
    return buf;
}

// write buffer to file
void write_file(const char* path, const char* buf) {
    FILE* f = fopen(path, "wb");
    if (!f) return;
    fwrite(buf, 1, strlen(buf), f);
    fclose(f);
}

// copy file
void copy_file(const char* src, const char* dst) {
    char* buf = read_file(src);
    if (buf) {
        write_file(dst, buf);
        free(buf);
    }
}

// parse front matter for title and date
void parse_front_matter(const char* md, char* title, char* date, const char** content_start) {
    title[0] = 0; date[0] = 0;
    if (strncmp(md, "---", 3) == 0) {
        const char* p = md + 3;
        while (*p && (*p == '\r' || *p == '\n')) p++;
        while (*p && strncmp(p, "---", 3) != 0) {
            if (strncmp(p, "title:", 6) == 0) {
                sscanf(p + 6, " %[^\n\r]", title);
            } else if (strncmp(p, "date:", 5) == 0) {
                sscanf(p + 5, " %[^\n\r]", date);
            }
            while (*p && *p != '\n') p++;
            if (*p) p++;
        }
        if (strncmp(p, "---", 3) == 0) {
            p += 3;
            while (*p && (*p == '\r' || *p == '\n')) p++;
            *content_start = p;
            return;
        }
    }
    *content_start = md;
}

// have to put this outside of md2html to stop compiler from bitching and to fix this shit not compiling 
void process_output(const char* text, size_t size, void* userdata) {
    struct buf* b = (struct buf*)userdata;
    if (b->len + size < b->cap) {
        memcpy(b->data + b->len, text, size);
        b->len += size;
    }
}

// convert markdown to html
void md2html(const char* md, char* html, unsigned int html_size) {
    struct buf out = {
        .data = malloc(html_size),
        .len = 0,
        .cap = html_size
    };

    md_html(md, (MD_SIZE)strlen(md), process_output, &out, 0, 0);

    out.data[out.len] = '\0';
    strncpy(html, out.data, html_size - 1);
    html[html_size - 1] = '\0';
    free(out.data);
}


// replace placeholder in template
void replace_placeholder(char* buf, size_t bufsize, const char* placeholder, const char* value) {
    char* pos = strstr(buf, placeholder);
    if (pos) {
        char tmp[8192];
        size_t before = pos - buf;
        snprintf(tmp, sizeof(tmp), "%.*s%s%s", (int)before, buf, value, pos + strlen(placeholder));
        strncpy(buf, tmp, bufsize - 1);
        buf[bufsize - 1] = 0;
    }
}

// load templates
char* header_html = NULL;
char* footer_html = NULL;
char* post_template = NULL;
char* index_template = NULL;

void load_templates() {
    if (!header_html) header_html = read_file(TEMPLATE_FOLDER "header.html");
    if (!footer_html) footer_html = read_file(TEMPLATE_FOLDER "footer.html");
    if (!post_template) post_template = read_file(TEMPLATE_FOLDER "post_temp.html");
    if (!index_template) index_template = read_file(TEMPLATE_FOLDER "index.html");
}

// read all posts from posts/
void process_posts() {
    DIR* dir = opendir(POSTS_FOLDER);
    struct dirent* ent;
    post_count = 0;
    while ((ent = readdir(dir)) != NULL && post_count < MAX_POSTS) {
        if (strstr(ent->d_name, ".md")) {
            char path[MAX_PATH];
            snprintf(path, sizeof(path), "%s%s", POSTS_FOLDER, ent->d_name);
            char* md = read_file(path);
            if (!md) continue;
            char title[MAX_TITLE], date[MAX_DATE];
            const char* content;
            parse_front_matter(md, title, date, &content);
            char html[8192];
            md2html(content, html, sizeof(html));
            strncpy(posts[post_count].filename, ent->d_name, MAX_PATH - 1);
            strncpy(posts[post_count].title, title, MAX_TITLE - 1);
            strncpy(posts[post_count].date, date, MAX_DATE - 1);
            strncpy(posts[post_count].html_content, html, sizeof(posts[post_count].html_content) - 1);
            post_count++;
            free(md);
        }
    }
    closedir(dir);
}

// make html for each post
void process_post() {
    make_folder(OUTPUT_FOLDER "posts/");
    for (int i = 0; i < post_count; ++i) {
        char outname[MAX_PATH];
        strncpy(outname, posts[i].filename, MAX_PATH - 1);
        char* ext = strstr(outname, ".md");
        if (ext) *ext = '\0';

        char outpath[MAX_PATH], buf[16384];
        snprintf(outpath, sizeof(outpath), OUTPUT_FOLDER "posts/%s.html", outname);
        strncpy(buf, post_template, sizeof(buf) - 1);
        replace_placeholder(buf, sizeof(buf), "{$header}", header_html ? header_html : "");
        replace_placeholder(buf, sizeof(buf), "{$footer}", footer_html ? footer_html : "");
        replace_placeholder(buf, sizeof(buf), "{$content}", posts[i].html_content);
        write_file(outpath, buf);
        tidy_html(outpath); 
    }
}

// make html table of posts
void generate_posts_table(char* table_html, size_t size) {
    snprintf(table_html, size,
        "<table class=\"posts-table\">\n"
        "<tr><th>Title</th><th>Date</th></tr>\n");
    for (int i = 0; i < post_count; ++i) {
        char name_no_ext[MAX_PATH];
        strncpy(name_no_ext, posts[i].filename, MAX_PATH - 1);
        char* ext = strstr(name_no_ext, ".md");
        if (ext) *ext = '\0';

        char row[512];
        snprintf(row, sizeof(row),
            "<tr><td><a href=\"posts/%s.html\">%s</a></td><td>%s</td></tr>\n",
            name_no_ext, posts[i].title, posts[i].date);
        strncat(table_html, row, size - strlen(table_html) - 1);
    }
    strncat(table_html, "</table>\n", size - strlen(table_html) - 1);
}

// make index.html
void generate_index() {
    char posts_html[8192] = "";
    generate_posts_table(posts_html, sizeof(posts_html));
    char buf[16384];
    strncpy(buf, index_template, sizeof(buf) - 1);
    replace_placeholder(buf, sizeof(buf), "{$header}", header_html ? header_html : "");
    replace_placeholder(buf, sizeof(buf), "{$footer}", footer_html ? footer_html : "");
    replace_placeholder(buf, sizeof(buf), "{$posts}", posts_html);
    write_file(OUTPUT_FOLDER "index.html", buf);
    tidy_html(OUTPUT_FOLDER "index.html");
}

// copy files to output
void copy_files_to_output() {
    copy_file(TEMPLATE_FOLDER "style.css", OUTPUT_FOLDER "style.css");
    copy_file(TEMPLATE_FOLDER "script.js", OUTPUT_FOLDER "script.js");
}

// delete a directory and its contents
void delete_dir(const char* path) {
    DIR* dir = opendir(path);
    if (!dir) return;
    struct dirent* ent;
    char filepath[MAX_PATH];
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        snprintf(filepath, sizeof(filepath), "%s/%s", path, ent->d_name);
        struct stat st;
        if (stat(filepath, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                delete_dir(filepath);
            } else {
                remove(filepath);
            }
        }
    }
    closedir(dir);
    rmdir(path);
}

int main() {
    delete_dir(OUTPUT_FOLDER);
    make_folder(OUTPUT_FOLDER);
    load_templates();
    process_posts();
    process_post();
    generate_index();
    copy_files_to_output();
    printf("blog generated in '%s'\n", OUTPUT_FOLDER);
    free(header_html); free(footer_html); free(post_template); free(index_template);
    return 0;
}
