#pragma once

#include <stdlib.h>

#define USERS_FILE "users.data"

typedef struct User {
    char username[50];
    char email[50];
    char password[50];
    int highscore;
} User;

extern User *logged_in_user;
extern User guest;

typedef struct {
    User **users;
    int n_users;
} UserManager;

extern UserManager usermanager;

int UserManagerInit(UserManager *userman);
int UserManagerFree(UserManager *userman);

int UserManagerFlush(UserManager *userman);
int UserManagerRegister(UserManager *userman, char *username, char *email, char *password);
User *UserManagerLogin(UserManager *userman, char *username, char *password);
