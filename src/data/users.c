#include "users.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

UserManager usermanager;
User *logged_in_user = NULL;
User guest = {
    .username = "Guest",
    .email = "guest-email@gmail.com",
    .password = "uwu",
    .highscore = 420
};

User **LoadUsers(char *filename, int *n_users);
int WriteUsers(char *filename, User **users, int n_users);

// TODO: is this valid?
bool IsEmailValid(char *email) {
    //         abc@xyz.com
    // region: 000 111 222

    // TODO:
    int region = 0;
    for (; *email; email++) {
        // printf("'%c', %d\n", *email, region);
        if (!isalnum(*email) && *email != '_' && !(region == 0 && *email == '.')) {
            char expecting;
            if (region == 0) expecting = '@';
            else /*if (region == 1)*/ expecting = '.';
            // else return 0;
            // printf("'%c', %d ex=%c\n", *email, region, expecting);

            region++;
            if (*email != expecting)
                return false;
        }
    }

    // printf("Finally\n");

    return region == 2;
}

int UserManagerInit(UserManager *userman) {
    userman->users = LoadUsers(USERS_FILE, &userman->n_users);
    return userman->users ? 0 : -1;
}

int UserManagerFree(UserManager *userman) {
    free(userman->users);
    return 0;
}

int UserManagerFlush(UserManager *userman) {
    return WriteUsers(USERS_FILE, userman->users, userman->n_users);
}

int UserManagerRegister(UserManager *userman, char *username, char *email, char *password) {
    // TODO: Also check for duplicate email?
    assert(strlen(username) < 50);
    assert(strlen(email) < 50);
    assert(strlen(password) < 50);
    assert(IsEmailValid(email));

    for (int i = 0; i < userman->n_users; i++) {
        if (strcmp(userman->users[i]->username, username) == 0)
            return -1;
    }

    userman->users = realloc(userman->users, (userman->n_users + 1) * sizeof(User *));
    User *user = malloc(sizeof(User));
    userman->users[userman->n_users++] = user;
    strncpy(user->username, username, 50);
    strncpy(user->email, email, 50);
    strncpy(user->password, password, 50);
    user->highscore = 0;

    UserManagerFlush(userman);

    return 0;
}

User *UserManagerLogin(UserManager *userman, char *username, char *password) {
    User *out = NULL;

    // Do a full iteration to avoid timing attacks
    for (int i = 0; i < userman->n_users; i++) {
        User *user = userman->users[i];
        int usercmp = strcmp(user->username, username);
        int passcmp = strcmp(user->password, password);
        if (usercmp == 0 && passcmp == 0)
            out = user;
    }

    return out;
}

User **LoadUsers(char *filename, int *n_users) {
    int user = 0;
    int field = 0;
    int offset = 0;
    *n_users = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return NULL;

    User **users = malloc(1 * sizeof(User *));
    users[user] = malloc(sizeof(User));

    char buffer[51];
    int read_count = 0;
    char highscore_buffer[10];

    while ((read_count = fread(buffer, 1, 50, file))) {
        for (int i = 0; i < read_count; i++) {
            char *target;
            if (field == 0)
                target = users[user]->username;
            else if (field == 1)
                target = users[user]->email;
            else if (field == 2)
                target = users[user]->password;
            else if (field == 3)
                target = highscore_buffer;

            if (buffer[i] == '\t') {
                if (field == 3) {
                    // TODO: Handle memory leaks
                    free(users);
                    return NULL;
                }
                target[offset] = 0;
                field++;
                offset = 0;
            } else if (buffer[i] == '\n') {
                if (field != 3) {
                    free(users);
                    return NULL;
                }
                target[offset] = 0;
                sscanf(target, "%d", &users[user]->highscore);
                fprintf(stdout, "User { u='%s', e='%s', p='%s', s=%d }\n", users[user]->username, users[user]->email, users[user]->password, users[user]->highscore);
                user++;
                users[user] = malloc(sizeof(User));
                users = realloc(users, (user + 1) * sizeof(User));
                field = 0;
                offset = 0;
            } else {
                target[offset++] = buffer[i];
            }
        }
    }

    printf("%i, %i, %i\n", user, field, offset);

    if (field) {
        if (field != 2) {
            // TODO: ...
            free(users);
            return NULL;
        }
        users[user++]->password[offset] = 0;
    }

    // char *target;
    // if (field == 0)
    //     target = users[user].username;
    // else if (field == 1)
    //     target = users[user].email;
    // else if (field == 2)
    //     target = users[user].password;
    // target[offset] = 0;
    // fprintf(stdout, "User { u='%s', e='%s', p='%s' }\n", users[user].username, users[user].email, users[user].password);
    *n_users = user;

    // buffer[fread(buffer, 1, 50, file)] = 0;
    // fprintf(stdout, "%s", buffer);
    return users;
}

int WriteUsers(char *filename, User **users, int n_users) {
    FILE *file = fopen(filename, "w");
    if (file == NULL)
        return -1;

    for (int i = 0; i < n_users; i++) {
        User *u = users[i];
        fprintf(file, "%s\t%s\t%s\t%d\n", u->username, u->email, u->password, u->highscore);
    }

    fflush(file);
    fclose(file);

    return 0;
}
