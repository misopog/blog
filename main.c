#include <cmark-gfm.h>
#include <cmark-gfm-core-extensions.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

#define TEMPLATE_FOLDER "template/"
#define POSTS_FOLDER "posts/"
#define OUTPUT_FOLDER "output/"
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

// format html using tidy 
void tidy_html(const char* filepath)
{
    char cmd[MAX_PATH + 128];
    snprintf(cmd, sizeof(cmd), "tidy -indent -modify -quiet \"%s\"", filepath);
    system(cmd);
}

// make a dir if it doesn't exist
void make_dir(const char* path)
{
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

// delete a directory and its contents
void delete_dir(const char* path)
{
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

// copy a dir and its contents
void copy_dir(const char* src, const char* dst)
{
    DIR* dir = opendir(src);
    if (!dir) return;
    make_dir(dst);
    struct dirent* ent;
    char srcpath[MAX_PATH];
    char dstpath[MAX_PATH];
    struct stat st;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        snprintf(srcpath, sizeof(srcpath), "%s/%s", src, ent->d_name);
        snprintf(dstpath, sizeof(dstpath), "%s/%s", dst, ent->d_name);
        if (stat(srcpath, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                copy_dir(srcpath, dstpath);
            } else {
                copy_file(srcpath, dstpath);
            }
        }
    }
    closedir(dir);
}

// read file into buffer (caller frees)
char* read_file(const char* path)
{
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
void write_file(const char* path, const char* buf)
{
    FILE* f = fopen(path, "wb");
    if (!f) return;
    fwrite(buf, 1, strlen(buf), f);
    fclose(f);
}

// copy file
void copy_file(const char* src, const char* dst)
{
    char* buf = read_file(src);
    if (buf) {
        write_file(dst, buf);
        free(buf);
    }
}

// parse front matter for title and date
void parse_front_matter(const char* md, char* title, char* date, const char** content_start)
{
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

// convert markdown to html
void md2html(const char* md, char* html, unsigned int html_size)
{
    cmark_parser* p = cmark_parser_new(CMARK_OPT_DEFAULT);
    cmark_gfm_core_extensions_ensure_registered();
    cmark_parser_attach_syntax_extension(p, cmark_find_syntax_extension("table"));
    cmark_parser_attach_syntax_extension(p, cmark_find_syntax_extension("strikethrough"));
    cmark_parser_attach_syntax_extension(p, cmark_find_syntax_extension("autolink"));
    cmark_parser_attach_syntax_extension(p, cmark_find_syntax_extension("tasklist"));
    cmark_parser_feed(p, md, strlen(md));
    cmark_node* doc = cmark_parser_finish(p);
    char* out = cmark_render_html(doc, CMARK_OPT_DEFAULT, NULL);
    if (!out) {
        html[0] = 0;
        return;
    }
    strncpy(html, out, html_size - 1);
    html[html_size - 1] = 0;
    free(out);
    cmark_node_free(doc);
    cmark_parser_free(p);
}


// replace placeholder in template
void replace_placeholder(char* buf, size_t bufsize, const char* placeholder, const char* value)
{
    char tmp[16384]; 
    tmp[0] = 0;
    const char* p = buf;
    char* out = tmp;
    size_t placeholder_len = strlen(placeholder);
    while (*p && (out - tmp) < (int)(sizeof(tmp) - 1)) {
        const char* match = strstr(p, placeholder);
        if (!match) {
            strncat(out, p, sizeof(tmp) - strlen(tmp) - 1);
            break;
        }
        size_t bytes_to_copy = match - p;
        strncat(out, p, bytes_to_copy);
        strncat(out, value, sizeof(tmp) - strlen(tmp) - 1);
        p = match + placeholder_len;
    }
    strncpy(buf, tmp, bufsize - 1);
    buf[bufsize - 1] = 0;
}


// load templates
char* header_html = NULL;
char* footer_html = NULL;
char* post_template = NULL;
char* index_template = NULL;

void load_templates()
{
    if (!header_html) header_html = read_file(TEMPLATE_FOLDER "header.html");
    if (!footer_html) footer_html = read_file(TEMPLATE_FOLDER "footer.html");
    if (!post_template) post_template = read_file(TEMPLATE_FOLDER "post_temp.html");
    if (!index_template) index_template = read_file(TEMPLATE_FOLDER "index.html");

    if (!header_html || !footer_html || !post_template || !index_template) {
        fprintf(stderr, "err: template files are missing '%s'\n", TEMPLATE_FOLDER);
        exit(1);
    }
}

// read all posts from posts/
void process_posts()
{
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
void process_post()
{
    make_dir(OUTPUT_FOLDER "posts/");
    for (int i = 0; i < post_count; ++i) {
        char outname[MAX_PATH];
        strncpy(outname, posts[i].filename, MAX_PATH - 1);
        char* ext = strstr(outname, ".md");
        if (ext) *ext = '\0';

        char outpath[MAX_PATH], buf[16384];
        snprintf(outpath, sizeof(outpath), OUTPUT_FOLDER "posts/%s.html", outname);
        strncpy(buf, post_template, sizeof(buf) - 1);
	replace_placeholder(buf, sizeof(buf), "{$title}", posts[i].title);
	replace_placeholder(buf, sizeof(buf), "{$date}", posts[i].date);
        replace_placeholder(buf, sizeof(buf), "{$header}", header_html ? header_html : "");
        replace_placeholder(buf, sizeof(buf), "{$footer}", footer_html ? footer_html : "");
	replace_placeholder(buf, sizeof(buf), "{$content}", posts[i].html_content);
        write_file(outpath, buf);
        tidy_html(outpath); 
    }
}

// make html table of posts
void generate_posts_table(char* table_html, size_t size)
{
    snprintf(table_html, size,
        "<table class=\"posts-table\">\n");
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
void generate_index()
{
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

// generate rss feed
void generate_rss(const Post* posts, int post_count)
{
    char path[MAX_PATH];
    snprintf(path, sizeof(path), OUTPUT_FOLDER "rss.xml");
    FILE* f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "failed to write rss: %s\n", path);
        return;
    }
    fprintf(f,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">\n"
        "<channel>\n"
        "  <title>misopog's blog</title>\n"
        "  <link>https://blog.misopog.xyz</link>\n"
        "  <description>random stuffs</description>\n"
	"  <atom:link href=\"https://blog.misopog.xyz/rss.xml\" rel=\"self\" type=\"application/rss+xml\" />\n"
    );
    for (int i = 0; i < post_count; ++i) {
        char name[MAX_PATH];
        strncpy(name, posts[i].filename, MAX_PATH - 1);
        char* ext = strstr(name, ".md");
        if (ext) *ext = 0;
	
    // WARNING - VERY JANKY CODE TO GET OVER RFC-822
    char date[128];
    struct tm t = {0};
    int year, month, day;
    if (sscanf(posts[i].date, "%4d-%2d-%2d", &year, &month, &day) == 3) {
        t.tm_year = year - 1900;
        t.tm_mon = month - 1;
        t.tm_mday = day;
        t.tm_hour = 0;
        t.tm_min = 0;
        t.tm_sec = 0;
        mktime(&t); // normalize
        strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S +0000", &t);
    } else {
        strncpy(date, "Thu, 01 Jan 1970 00:00:00 +0000", sizeof(date)); // fallback
    }


        fprintf(f,
            "  <item>\n"
            "    <title>%s</title>\n"
            "    <link>https://blog.misopog.xyz/posts/%s.html</link>\n"
            "    <pubDate>%s</pubDate>\n"
            "    <guid>https://blog.misopog.xyz/posts/%s.html</guid>\n"
            "    <description><![CDATA[%s]]></description>\n"
            "  </item>\n",
            posts[i].title,
            name,
            date,
            name,
            posts[i].html_content
        );
    }

    fprintf(f, "</channel>\n</rss>\n");
    fclose(f);
}



// copy files to output
void copy_files_to_output()
{
    copy_file(TEMPLATE_FOLDER "style.css", OUTPUT_FOLDER "style.css");
    copy_file(TEMPLATE_FOLDER "script.js", OUTPUT_FOLDER "script.js");
    copy_dir("img", OUTPUT_FOLDER "img");
}

// get and compare all dates from posts 
int compare_post_by_date(const void* a, const void* b)
{
    const Post* pa = (const Post*)a;
    const Post* pb = (const Post*)b;
    return strcmp(pb->date, pa->date); // newest first
}

// then sort
void sort_posts()
{
    qsort(posts, post_count, sizeof(Post), compare_post_by_date);
}

int main()
{
    delete_dir(OUTPUT_FOLDER);
    make_dir(OUTPUT_FOLDER);
    load_templates();
    process_posts();
    sort_posts();
    process_post();
    generate_index();
    generate_rss(posts, post_count);
    copy_files_to_output();
    printf("blog generated in '%s'\n", OUTPUT_FOLDER);
    free(header_html); free(footer_html); free(post_template); free(index_template);
    return 0;
}
