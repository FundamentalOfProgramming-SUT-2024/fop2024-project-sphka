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
    .score_sum = 0,
    .gold_sum = 0,
    .game_count = 0,
    .first_game_time = 0,
};

User **LoadUsers(char *filename, int *n_users);
int WriteUsers(char *filename, User **users, int n_users);

bool IsValidEmailChar(char c) {
    // return isalnum(c) || c == '.' || c == '_' || c == '-';
    return isalnum(c) || c == '_' || c == '-';
}

bool IsEmailValid(char *email) {
    //         abc@xyz.com
    // region: 000 1111111

    int len = strlen(email);
    // Explicitly check that the last char is not '.'
    if (email[len - 1] == '.')
        return false;

    int region = 0;
    bool has_dot = false;
    for (int i = 0; i < len; i++) {
        char c = email[i];
        
        if (region == 0) {
            if (IsValidEmailChar(c) || c == '.')
                continue;
            else if (c == '@') {
                if (i == 0)
                    return false;

                region++;
            }
            else
                return false;
        } else if (region == 1) {
            if (IsValidEmailChar(c))
                continue;
            else if (c == '.') {
                if (email[i - 1] == '@')
                    return false;

                has_dot = true;
            }
            else
                return false;
        }
    }

    // printf("Finally\n");

    return region == 1 && has_dot;
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
    // assert(IsEmailValid(email));

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
    user->score_sum = 0;
    user->gold_sum = 0;
    user->game_count = 0;
    user->first_game_time = 0;

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
    *n_users = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return NULL;

    User **users = malloc(sizeof(User *));

    char *buffer = malloc(1000);
    while (fgets(buffer, 1000, file)) {
        users = realloc(users, (user + 1) * sizeof(User *));
        users[user] = malloc(sizeof(User));

        int field = 0;
        for (char *tok = strtok(buffer, "\t"); tok; tok = strtok(NULL, "\t")) {
            switch (field)
            {
            case 0: strcpy(users[user]->username, tok); break;
            case 1: strcpy(users[user]->email, tok); break;
            case 2: strcpy(users[user]->password, tok); break;

            case 3: sscanf(tok, "%d", &users[user]->score_sum); break;
            case 4: sscanf(tok, "%d", &users[user]->gold_sum); break;
            case 5: sscanf(tok, "%d", &users[user]->game_count); break;
            case 6: sscanf(tok, "%llu", &users[user]->first_game_time); break;

            default:
                break;
            }

            field++;
        }

        fprintf(stdout, "User { u='%s', e='%s', p='%s', s=%d }\n", users[user]->username, users[user]->email, users[user]->password, users[user]->score_sum);

        user++;
    }

    *n_users = user;
    return users;
}

int WriteUsers(char *filename, User **users, int n_users) {
    FILE *file = fopen(filename, "w");
    if (file == NULL)
        return -1;

    for (int i = 0; i < n_users; i++) {
        User *u = users[i];
        fprintf(file, "%s\t%s\t%s\t", u->username, u->email, u->password);

        fprintf(file, "%d\t", u->score_sum);
        fprintf(file, "%d\t", u->gold_sum);
        fprintf(file, "%d\t", u->game_count);
        fprintf(file, "%llu\n", u->first_game_time);
    }

    fflush(file);
    fclose(file);

    return 0;
}
