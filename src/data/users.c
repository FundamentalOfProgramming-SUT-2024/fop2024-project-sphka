#include "users.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

User *LoadUsers(char *filename, int *n_users);
int WriteUsers(char *filename, User *users, int n_users);

// TODO: is this valid?
int IsEmailValid(char *email) {
    //         abc@xyz.com
    // region: 000 111 222

    // TODO: .
    for (int region = 0; *email; email++) {
        // printf("'%c', %d\n", *email, region);
        if (!isalnum(*email) && !(*email == '_')) {
            char expecting;
            if (region == 0) expecting = '@';
            else if (region == 1) expecting = '.';
            else return 0;
            // printf("'%c', %d ex=%c\n", *email, region, expecting);

            region++;
            if (*email != expecting)
                return 0;
        }
    }

    // printf("Finally\n");

    return 1;
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
        if (strcmp(userman->users[i].username, username) == 0)
            return -1;
    }


    userman->users = realloc(userman->users, (userman->n_users + 1) * sizeof(User));
    User *user = &userman->users[userman->n_users++];
    strncpy(user->username, username, 50);
    strncpy(user->email, email, 50);
    strncpy(user->password, password, 50);

    UserManagerFlush(userman);

    return 0;
}

User *LoadUsers(char *filename, int *n_users) {
    int user = 0;
    int field = 0;
    int offset = 0;
    *n_users = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return NULL;

    User *users = malloc(1 * sizeof(User));

    char buffer[51];
    int read_count = 0;

    while ((read_count = fread(buffer, 1, 50, file))) {
        for (int i = 0; i < read_count; i++) {
            char *target;
            if (field == 0)
                target = users[user].username;
            else if (field == 1)
                target = users[user].email;
            else if (field == 2)
                target = users[user].password;

            if (buffer[i] == '\t') {
                if (field == 2) {
                    free(users);
                    return NULL;
                }
                target[offset] = 0;
                field++;
                offset = 0;
            } else if (buffer[i] == '\n') {
                if (field != 2) {
                    free(users);
                    return NULL;
                }
                target[offset] = 0;
                fprintf(stdout, "User { u='%s', e='%s', p='%s' }\n", users[user].username, users[user].email, users[user].password);
                user++;
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
            free(users);
            return NULL;
        }
        users[user++].password[offset] = 0;
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

int WriteUsers(char *filename, User *users, int n_users) {
    FILE *file = fopen(filename, "w");
    if (file == NULL)
        return -1;

    for (int i = 0; i < n_users; i++) {
        fprintf(file, "%s\t%s\t%s\n", users[i].username, users[i].email, users[i].password);
    }

    return 0;
}
