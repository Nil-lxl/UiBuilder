#include "custom.h"
#include "my_custom.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <strings.h>
#include <dirent.h>
#endif

media_list_t player_list;

static char *my_strdup(const char *s)
{
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
}

static int is_video_file(const char *filename)
{
    const char *extensions[] = {".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".webm", ".png", NULL};
    const char **ext = extensions;

    const char *dot = strrchr(filename, '.');
    if (!dot)
        return 0;

    while (*ext)
    {
#ifdef _WIN32
        if (_stricmp(dot, *ext) == 0)
#else
        if (strcasecmp(dot, *ext) == 0)
#endif
        {
            return 1;
        }
        ext++;
    }
    return 0;
}

static char *extract_name(const char *filename)
{
    const char *last_slash = strrchr(filename, '/');
    const char *last_backslash = strrchr(filename, '\\');
    const char *base = filename;

    if (last_slash || last_backslash) {
        const char *last_sep = (last_slash > last_backslash) ? last_slash : last_backslash;
        base = last_sep + 1;
    }

    const char *dot = strrchr(base, '.');
    size_t len = dot ? (size_t)(dot - base) : strlen(base);

    char *name = malloc(len + 1);
    if (name) {
        strncpy(name, base, len);
        name[len] = '\0';
    }
    return name;
}

static int add_file_to_list(const char *path, const char *name)
{
    char **new_paths = realloc(player_list.paths, (player_list.count + 1) * sizeof(char *));
    if (!new_paths) {
        printf("Memory allocation failed for paths\n");
        return 0;
    }

    char **new_names = realloc(player_list.names, (player_list.count + 1) * sizeof(char *));
    if (!new_names) {
        printf("Memory allocation failed for names\n");
        free(new_paths);
        return 0;
    }

    player_list.paths = new_paths;
    player_list.names = new_names;

    player_list.paths[player_list.count] = my_strdup(path);
    if (!player_list.paths[player_list.count]) {
        printf("Memory allocation failed for path string\n");
        return 0;
    }

    player_list.names[player_list.count] = my_strdup(name);
    if (!player_list.names[player_list.count]) {
        printf("Memory allocation failed for name string\n");
        free(player_list.paths[player_list.count]);
        return 0;
    }

    player_list.count++;
    return 1;
}

static int scan_directory(const char *dirpath)
{
    int file_count = 0;

    char *real_path = (char *)dirpath;
    if (real_path[1] == ':' && ((real_path[0] >= 'A' && real_path[0] <= 'Z') || (real_path[0] >= 'a' && real_path[0] <= 'z'))) {
        real_path += 2;
    }

    char normalized_path[256];
    strncpy(normalized_path, real_path, sizeof(normalized_path) - 2);
    normalized_path[sizeof(normalized_path) - 1] = '\0';

    size_t len = strlen(normalized_path);
    if (len > 0) {
        if (normalized_path[len-1] != '/') {
            strcat(normalized_path, "/");
        }
    }

    printf("Scanning directory: %s\n", normalized_path);

#ifdef _WIN32
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s*", normalized_path);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Failed to open directory: %s, error: %lu\n", normalized_path, GetLastError());
        return 0;
    }

    do
    {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            is_video_file(findData.cFileName))
        {
            char fullPath[MAX_PATH];
            snprintf(fullPath, sizeof(fullPath), "%s%s", normalized_path, findData.cFileName);

            char *name = extract_name(findData.cFileName);
            if (name)
            {
                printf("Found video file: %s -> %s\n", fullPath, name);
                if (add_file_to_list(fullPath, name))
                {
                    file_count++;
                }
                free(name);
            }
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);

#else
    DIR *dir = opendir(normalized_path);
    if (!dir)
    {
        printf("Failed to open directory: %s\n", normalized_path);
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG && is_video_file(entry->d_name))
        {
            char fullPath[256];
            snprintf(fullPath, sizeof(fullPath), "%s%s", normalized_path, entry->d_name);

            char *name = extract_name(entry->d_name);
            if (name)
            {
                printf("Found video file: %s -> %s\n", fullPath, name);
                if (add_file_to_list(fullPath, name))
                {
                    file_count++;
                }
                free(name);
            }
        }
    }

    closedir(dir);
#endif

    return file_count;
}

void player_list_init(void)
{
    player_list.paths = NULL;
    player_list.names = NULL;
    player_list.count = 0;
    player_list.current = 0;

    const char *video_dir = LVGL_DIR"video/";
    printf("Video directory: %s\n", video_dir);

    int file_count = scan_directory(video_dir);
    printf("Found %d video files\n", file_count);

    if (file_count == 0) {
        printf("Warning: No video files found. Check if directory exists and contains video files.\n");
    }
}

void player_list_cleanup(void)
{
    for (int i = 0; i < player_list.count; i++)
    {
        free(player_list.paths[i]);
        free(player_list.names[i]);
    }
    free(player_list.paths);
    free(player_list.names);

    player_list.paths = NULL;
    player_list.names = NULL;
    player_list.count = 0;
    player_list.current = 0;
}

int player_list_get_count(void)
{
    return player_list.count;
}

const char *player_list_get_path(int index)
{
    if (index >= 0 && index < player_list.count)
    {
        return player_list.paths[index];
    }
    printf("Invalid path index: %d (count: %d)\n", index, player_list.count);
    return NULL;
}

const char *player_list_get_name(int index)
{
    if (index >= 0 && index < player_list.count)
    {
        return player_list.names[index];
    }
    printf("Invalid name index: %d (count: %d)\n", index, player_list.count);
    return NULL;
}

int player_list_set_current(int index)
{
    if (index >= 0 && index < player_list.count)
    {
        player_list.current = index;
        return 1;
    }
    printf("Invalid current index: %d (count: %d)\n", index, player_list.count);
    return 0;
}

int player_list_get_current(void)
{
    return player_list.current;
}
