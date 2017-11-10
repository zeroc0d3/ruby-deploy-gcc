/* 
 * File:   rb_deploy.c
 * Author: ZeroC0D3 LAB (zeroc0d3)
 *
 * Created on Oct 22nd, 2017, 08:00 
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

int ret;
char cmdRun[2048];
char DATE_TIME[100];
char SNAP_FOLDER[100];
char SNAP_FOLDER_RELEASE[512];
int IS_ROLLBACK = 0;     // Rollback Status           (0 = no rollback migration, 1 = rollback on failed deploy)
int IS_ERROR_DEPLOY = 0; // Error Deployment Status   (0 = no error, 1 = still error)

/* ======================================= 
        USER CONFIGURATION 
   ======================================= */
int NUM_RELEASE   = 10;               // Maximum Number of Release Folder 
char ENV[64]      = "staging";        // Selected Environment (staging / production)
int NUM_LOG_VIEW  = 50;               // Maximum Line Number Viewing Log 
int RAILS_VERSION = 5;                // Rails Version (default: 5)
int ENABLE_MIGRATION = 0;             // Force Enable Migration (0 = disable/default, 1 = enable)
int ENABLE_BUNDLE_INSTALL  = 1;       // Enable Running "bundle install" (0 = disable/default, 1 = enable)
int ENABLE_CLOBBER_ASSETS  = 1;       // Enable Running Clobber/Cleanup Assets (0 = disable/default, 1 = enable)
int ENABLE_COMPILE_ASSETS  = 1;       // Enable Running Assets Precompile (0 = disable/default, 1 = enable)
int ENABLE_FAYE_SERVICE    = 1;       // Enable Running Faye Service (0 = disable/default, 1 = enable)
int ENABLE_MONGODB_SERVICE = 1;       // Enable Running MongoDB Service (0 = disable/default, 1 = enable)
int ENABLE_PUSHR_SERVICE   = 1;       // Enable Running Pushr Service (0 = disable/default, 1 = enable)
int ENABLE_REDIS_SERVICE   = 1;       // Enable Running Redis Service (0 = disable/default, 1 = enable)
int ENABLE_SIDEKIQ_SERVICE = 1;       // Enable Running Sidekiq Service (0 = disable/default, 1 = enable)

// Repository
char REPO_NAME[1024] = "git@github.com:zeroc0d3/ruby-installation.git";
char REPO_BRANCH[64] = "master";

// Shared Folders
char *LIST_SHARED_FOLDERS[] = {
    "log",
    "public/assets",
    "public/system",
    "public/uploads",
    "tmp/cache",
    "tmp/pids",
    "tmp/sockets",
    "vendor/bundle",
    NULL
};

// Shared Files
char *LIST_SHARED_FILES[] = {
    ".env",
    "faye.ru",
    "config/application.yml",
    "config/database.yml",
    "config/mongoid.yml",
    "config/pushr-staging.yml",
    "config/pushr-production.yml",
    "config/secrets.yml",
    "config/sidekiq.yml",
    "config/unicorn/staging.rb",
    "config/unicorn/production.rb",
    NULL
};

/* ======================================= 
        ENVIRONMENT CONFIGURATION 
   ======================================= */
// DEVELOPMENT CONFIGURATION //
// Development Environment
char DEV_APP_ROOT[512]        = "/home/zeroc0d3/zeroc0d3-deploy";                                       // Development Root Path
char DEV_CONFIG_FAYE[512]     = "/home/zeroc0d3/zeroc0d3-deploy/current/faye.ru";                        // Development Faye Config
char DEV_CONFIG_PUSHR[512]    = "/home/zeroc0d3/zeroc0d3-deploy/current/config/pushr-staging.yaml";      // Development Pushr Config
char DEV_CONFIG_SIDEKIQ[512]  = "/home/zeroc0d3/zeroc0d3-deploy/current/config/sidekiq.yml";             // Development Sidekiq Config
char DEV_CONFIG_UNICORN[512]  = "/home/zeroc0d3/zeroc0d3-deploy/current/config/unicorn/staging.rb";      // Development Unicorn Config

char DEV_PID_FAYE[512]        = "/home/zeroc0d3/zeroc0d3-deploy/current/tmp/pids/faye.pid";              // Development Path PID Faye
char DEV_PID_PUSHR[512]       = "/home/zeroc0d3/zeroc0d3-deploy/current/tmp/pids/pushr.pid";             // Development Path PID Pushr
char DEV_PID_SIDEKIQ[512]     = "/home/zeroc0d3/zeroc0d3-deploy/current/tmp/pids/sidekiq.pid";           // Development Path PID Sidekiq
char DEV_PID_UNICORN[512]     = "/home/zeroc0d3/zeroc0d3-deploy/current/tmp/pids/unicorn.pid";           // Development Path PID Unicorn

char DEV_LOG_ENV[512]         = "/home/zeroc0d3/zeroc0d3-deploy/current/log/staging.log";                // Development Path Log Environment
char DEV_LOG_PUSHR[512]       = "/home/zeroc0d3/zeroc0d3-deploy/current/log/pushr.log";                  // Development Path Log Pushr
char DEV_LOG_SIDEKIQ[512]     = "/home/zeroc0d3/zeroc0d3-deploy/current/log/sidekiq.log";                // Development Path Log Sidekiq
char DEV_LOG_UNICORN[512]     = "/home/zeroc0d3/zeroc0d3-deploy/current/log/unicorn.log";                // Development Path Log Unicorn

char DEV_PATH_BUNDLE[512]     = "/home/zeroc0d3/.rbenv/shims/bundle";                   // Development Path of Bundle Binary
char DEV_PATH_GEM[512]        = "/home/zeroc0d3/.rbenv/shims/gem";                      // Development Path of Gem Binary
char DEV_PATH_RACKUP[512]     = "/home/zeroc0d3/.rbenv/shims/rackup";                   // Development Path of Rackup Binary
// >= Rails v5.0
char DEV_PATH_RAILS[512]      = "/home/zeroc0d3/.rbenv/shims/rails";                    // Development Path of Rails Binary
// < Rails v5.0
char DEV_PATH_RAKE[512]       = "/home/zeroc0d3/.rbenv/shims/rake";                     // Development Path of Rake Binary
char DEV_PATH_PUSHR[512]      = "/home/zeroc0d3/.rbenv/shims/pushr";                    // Development Path of Pushr Binary
char DEV_PATH_SIDEKIQ[512]    = "/home/zeroc0d3/.rbenv/shims/sidekiq";                  // Development Path of Sidekiq Binary
char DEV_PATH_UNICORN[512]    = "/home/zeroc0d3/.rbenv/shims/unicorn";                  // Development Path of Unicorn Binary

// PRODUCTION CONFIGURATION //
// Production Environment
char PROD_APP_ROOT[512]       = "/home/deploy/rb_deploy";                                      // Production Root Path
char PROD_CONFIG_UNICORN[512] = "/home/deploy/rb_deploy/current/config/unicorn/production.rb";  // Production Unicorn Config
char PROD_CONFIG_FAYE[512]    = "/home/deploy/rb_deploy/current/faye.ru";                       // Production Faye Config
char PROD_CONFIG_PUSHR[512]   = "/home/deploy/rb_deploy/current/config/pushr-production.yaml";  // Production Pushr Config
char PROD_CONFIG_SIDEKIQ[512] = "/home/deploy/rb_deploy/current/config/sidekiq.yml";            // Production Sidekiq Config

char PROD_PID_FAYE[512]       = "/home/deploy/rb_deploy/current/tmp/pids/faye.pid";             // Production Path PID Faye
char PROD_PID_PUSHR[512]      = "/home/deploy/rb_deploy/current/tmp/pids/pushr.pid";            // Production Path PID Pushr
char PROD_PID_SIDEKIQ[512]    = "/home/deploy/rb_deploy/current/tmp/pids/sidekiq.pid";          // Production Path PID Sidekiq
char PROD_PID_UNICORN[512]    = "/home/deploy/rb_deploy/current/tmp/pids/unicorn.pid";          // Production Path PID Unicorn

char PROD_LOG_ENV[512]        = "/home/deploy/rb_deploy/current/log/production.log";            // Production Path Log Environment
char PROD_LOG_PUSHR[512]      = "/home/deploy/rb_deploy/current/log/pushr.log";                 // Production Path Log Pushr
char PROD_LOG_SIDEKIQ[512]    = "/home/deploy/rb_deploy/current/log/sidekiq.log";               // Production Path Log Sidekiq
char PROD_LOG_UNICORN[512]    = "/home/deploy/rb_deploy/current/log/unicorn.log";               // Production Path Log Unicorn

char PROD_PATH_BUNDLE[512]    = "/home/deploy/.rbenv/shims/bundle";           // Production Path of Bundle Binary
char PROD_PATH_GEM[512]       = "/home/deploy/.rbenv/shims/gem";              // Production Path of Gem Binary
char PROD_PATH_RACKUP[512]    = "/home/deploy/.rbenv/shims/rackup";           // Production Path of Rackup Binary
// >= Rails v5.0
char PROD_PATH_RAILS[512]     = "/home/deploy/.rbenv/shims/rails";            // Production Path of Rails Binary
// < Rails v5.0
char PROD_PATH_RAKE[512]      = "/home/deploy/.rbenv/shims/rake";             // Production Path of Rake Binary
char PROD_PATH_PUSHR[512]     = "/home/deploy/.rbenv/shims/pushr";            // Production Path of Pushr Binary
char PROD_PATH_SIDEKIQ[512]   = "/home/deploy/.rbenv/shims/sidekiq";          // Production Path of Sidekiq Binary
char PROD_PATH_UNICORN[512]   = "/home/deploy/.rbenv/shims/unicorn";          // Production Path of Unicorn Binary

/* ======================================= 
        SYSTEM CONFIGURATION 
   ======================================= */
char VERSION[16] = "1.2.16";               // Version 
char APP_ROOT[512];                        // Root Path
char APP_CURRENT[64] = "current";          // Current Folder
char APP_RELEASE[64] = "release";          // Release Folder
char APP_SHARED[64]  = "shared";           // Shared Folder
char CURRENT_FOLDER[1024];                 // CURRENT_FOLDER = APP_ROOT/APP_CURRENT
char SHARED_FOLDER[1024];                  // SHARED_FOLDER  = APP_ROOT/APP_SHARED
char PREINSTALL[64]  = "preinstall.sh";    // Preinstallation Script Before Server-Up

// GENERAL CONFIGURATION //
// Config
char CONFIG_UNICORN[512];           // Unicorn Config
char CONFIG_FAYE[512];              // Faye Config
char CONFIG_PUSHR[512];             // Pushr Config
char CONFIG_SIDEKIQ[512];           // Sidekiq Config
// PID
char PID_UNICORN[512];              // Path PID Unicorn
char PID_FAYE[512];                 // Path PID Faye
char PID_PUSHR[512];                // Path PID Pushr
char PID_SIDEKIQ[512];              // Path PID Sidekiq
// Binary
char PATH_BUNDLE[512];              // Path of Bundle Binary
char PATH_GEM[512];                 // Path of Gem Binary
char PATH_RACKUP[512];              // Path of Rackup Binary
char PATH_RAILS[512];               // Path of Rails Binary
char PATH_RAKE[512];                // Path of Rake Binary
char PATH_PUSHR[512];               // Path of Pushr Binary
char PATH_SIDEKIQ[512];             // Path of Sidekiq Binary
char PATH_UNICORN[512];             // Path of Unicorn Binary
// Log
char SYS_LOG_ENV[512];                                               // Path Log Environment
char SYS_LOG_PUSHR[512];                                             // Path Log Pushr
char SYS_LOG_SIDEKIQ[512];                                           // Path Log Sidekiq
char SYS_LOG_UNICORN[512];                                           // Path Log Unicorn
char SYS_LOG_NGINX_ERROR[512]  = "/var/log/nginx/error.log";         // Path Log NGINX Error
char SYS_LOG_NGINX_ACCESS[512] = "/var/log/nginx/access.log";        // Path Log NGINX Access
char SYS_LOG_MONGODB[512]      = "/var/log/mongodb.log";             // Path Log MongoDB
char SYS_LOG_MEMCACHED[521]    = "/var/log/memcached.log";           // Path Log Memcached
char SYS_LOG_REDIS[521]        = "/var/log/redis/redis-server.log";  // Path Log Redis


/* ======================================= 
        SUB MAIN PROGRAM
   ======================================= */
char *trim(char *str)
{
  size_t len = 0;
  char *frontp = str;
  char *endp = NULL;

  if (str == NULL) {
    return NULL;
  }
  if (str[0] == '\0') {
    return str;
  }

  len = strlen(str);
  endp = str + len;

  /* Move the front and back pointers to address the first non-whitespace
   * characters from each end.
   */
  while (isspace((unsigned char)*frontp)) {
    ++frontp;
  }
  if (endp != frontp) {
    while (isspace((unsigned char)*(--endp)) && endp != frontp) {
    }
  }

  if (str + len - 1 != endp)
    *(endp + 1) = '\0';
  else if (frontp != str && endp == frontp)
    *str = '\0';

  /* Shift the string so that it starts at str so that if it's dynamically
   * allocated, we can still free it on the returned pointer.  Note the reuse
   * of endp to mean the front of the string buffer now.
   */
  endp = str;
  if (frontp != str) {
    while (*frontp) {
      *endp++ = *frontp++;
    }
    *endp = '\0';
  }
  return str;
}

void get_time()
{
    char buff[100];
    time_t now = time(0);
    strftime(buff, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));
    sprintf(DATE_TIME, "%s", buff);
}

void get_folder()
{
    char buff[100];
    int buff_unix;
    time_t now = time(0);
    buff_unix  = (int)time(NULL);
    strftime(buff, 100, "%Y%m%d%H%M", localtime(&now));

    // Unix Timestamp
    // sprintf(SNAP_FOLDER, "%d", buff_unix);

    // Full Timestamp
    sprintf(SNAP_FOLDER, "%s_%d", buff, buff_unix);
}

void get_folder_release()
{
    sprintf(SNAP_FOLDER_RELEASE, "%s/%s/%s", APP_ROOT, APP_RELEASE, SNAP_FOLDER);
}

void get_folder_current()
{
    sprintf(CURRENT_FOLDER, "%s/%s", APP_ROOT, APP_CURRENT);
}

/* --------------------------------------- 
        Select Environment
   --------------------------------------- */
void select_env()
{
    int environment = strcmp(ENV, "production");
    if (environment == 0)
    {
        sprintf(APP_ROOT, "%s", PROD_APP_ROOT);
        sprintf(CONFIG_FAYE, "%s", PROD_CONFIG_FAYE);
        sprintf(CONFIG_PUSHR, "%s", PROD_CONFIG_PUSHR);
        sprintf(CONFIG_SIDEKIQ, "%s", PROD_CONFIG_SIDEKIQ);
        sprintf(CONFIG_UNICORN, "%s", PROD_CONFIG_UNICORN);
        sprintf(PID_FAYE, "%s", PROD_PID_FAYE);
        sprintf(PID_PUSHR, "%s", PROD_PID_PUSHR);
        sprintf(PID_SIDEKIQ, "%s", PROD_PID_SIDEKIQ);
        sprintf(PID_UNICORN, "%s", PROD_PID_UNICORN);
        sprintf(SYS_LOG_ENV, "%s", PROD_LOG_ENV);
        sprintf(SYS_LOG_PUSHR, "%s", PROD_LOG_PUSHR);
        sprintf(SYS_LOG_SIDEKIQ, "%s", PROD_LOG_SIDEKIQ);
        sprintf(SYS_LOG_UNICORN, "%s", PROD_LOG_UNICORN);
        sprintf(PATH_BUNDLE, "%s", PROD_PATH_BUNDLE);
        sprintf(PATH_GEM, "%s", PROD_PATH_GEM);
        sprintf(PATH_PUSHR, "%s", PROD_PATH_PUSHR);
        sprintf(PATH_RACKUP, "%s", PROD_PATH_RACKUP);
        sprintf(PATH_RAILS, "%s", PROD_PATH_RAILS);
        sprintf(PATH_RAKE, "%s", PROD_PATH_RAKE);
        sprintf(PATH_SIDEKIQ, "%s", PROD_PATH_SIDEKIQ);
        sprintf(PATH_UNICORN, "%s", PROD_PATH_UNICORN);
    }
    else
    {
        sprintf(APP_ROOT, "%s", DEV_APP_ROOT);
        sprintf(CONFIG_FAYE, "%s", DEV_CONFIG_FAYE);
        sprintf(CONFIG_PUSHR, "%s", DEV_CONFIG_PUSHR);
        sprintf(CONFIG_SIDEKIQ, "%s", DEV_CONFIG_SIDEKIQ);
        sprintf(CONFIG_UNICORN, "%s", DEV_CONFIG_UNICORN);
        sprintf(PID_FAYE, "%s", DEV_PID_FAYE);
        sprintf(PID_PUSHR, "%s", DEV_PID_PUSHR);
        sprintf(PID_SIDEKIQ, "%s", DEV_PID_SIDEKIQ);
        sprintf(PID_UNICORN, "%s", DEV_PID_UNICORN);
        sprintf(SYS_LOG_ENV, "%s", DEV_LOG_ENV);
        sprintf(SYS_LOG_PUSHR, "%s", DEV_LOG_PUSHR);
        sprintf(SYS_LOG_SIDEKIQ, "%s", DEV_LOG_SIDEKIQ);
        sprintf(SYS_LOG_UNICORN, "%s", DEV_LOG_UNICORN);
        sprintf(PATH_BUNDLE, "%s", DEV_PATH_BUNDLE);
        sprintf(PATH_GEM, "%s", DEV_PATH_GEM);
        sprintf(PATH_PUSHR, "%s", DEV_PATH_PUSHR);
        sprintf(PATH_RACKUP, "%s", DEV_PATH_RACKUP);
        sprintf(PATH_RAILS, "%s", DEV_PATH_RAILS);
        sprintf(PATH_RAKE, "%s", DEV_PATH_RAKE);
        sprintf(PATH_SIDEKIQ, "%s", DEV_PATH_SIDEKIQ);
        sprintf(PATH_UNICORN, "%s", DEV_PATH_UNICORN);
    }
}

void logo()
{
    // printf("\033[22;32m==========================================================================\033[0m\n");
    // printf("\033[22;31m  __________                  _________ _______       .___________        \033[0m\n");
    // printf("\033[22;31m  \____    /___________  ____ \_   ___ \\   _  \    __| _/\_____  \  LAB  \033[0m\n");
    // printf("\033[22;31m    /     // __ \_  __ \/  _ \/    \  \//  /_\  \  / __ |   _(__  <       \033[0m\n");
    // printf("\033[22;31m   /     /\  ___/|  | \(  <_> )     \___\  \_/   \/ /_/ |  /       \      \033[0m\n");
    // printf("\033[22;31m  /_______ \___  >__|   \____/ \______  /\_____  /\____ | /______  /      \033[0m\n");
    // printf("\033[22;31m          \/   \/                     \/       \/      \/        \/       \033[0m\n");
    printf("\033[22;32m==========================================================================\033[0m\n");
    printf("\033[22;34m  ZeroC0D3 Ruby Deploy :: ver-%s                                          \033[0m\n", VERSION);
    printf("\033[22;34m  (c) 2017 ZeroC0D3 Team                                                  \033[0m\n");
}

void header()
{
    system("clear");
    logo();
    printf("\033[22;32m==========================================================================\033[0m\n");
    get_time();
    printf("\033[22;37m# BEGIN PROCESS..... (Please Wait)  \033[0m\n");
    printf("\033[22;33m# Start at: %s  \033[0m\n", DATE_TIME);
}

void footer()
{
    get_time();
    printf("\033[22;32m==========================================================================\033[0m\n");
    printf("\033[22;33m# Finish at: %s  \033[0m\n", DATE_TIME);
    printf("\033[22;37m# END PROCESS.....                  \033[0m\n\n");
}

void menu()
{
    select_env();
    system("clear");
    logo();
    printf("\033[22;32m==========================================================================\033[0m\n");
    printf("\033[22;32m  ### ENVIRONMENT DEPLOY ###                                              \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ENVIRONMENT   : \033[22;32m%s                                         \033[0m\n", ENV);
    printf("\033[22;34m  # PATH ROOT     : \033[22;32m%s                                         \033[0m\n", APP_ROOT);
    printf("\033[22;34m  # RAILS VERSION : \033[22;32m%d                                         \033[0m\n", RAILS_VERSION);
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### NGINX SERVICES ###                                                  \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -no            --> Reload NGINX                           \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -nr            --> Restart NGINX                          \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### ASSETS SERVICES ###                                                 \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -ac            --> Assets Clobber (Rollback)              \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -ap            --> Assets Precompile                      \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### RESTART SERVICES ###                                                \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    if (ENABLE_FAYE_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -rf            --> Restart Faye                           \033[0m\n");
    }
    if (ENABLE_MONGODB_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -rm            --> Restart MongoDB                        \033[0m\n");
    }
    if (ENABLE_PUSHR_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -rp            --> Restart Pushr                          \033[0m\n");
    }
    if (ENABLE_SIDEKIQ_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -rq            --> Restart Sidekiq                        \033[0m\n");
    }
    if (ENABLE_REDIS_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -rs            --> Restart Redis                          \033[0m\n");
    }
    printf("\033[22;34m  # ./rb_deploy -ru            --> Restart Unicorn                        \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### STOP SERVICES ###                                                   \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    if (ENABLE_FAYE_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -df            --> Stop Faye                              \033[0m\n");
    }
    if (ENABLE_MONGODB_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -dm            --> Stop MongoDB                           \033[0m\n");
    }
    if (ENABLE_PUSHR_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -dp            --> Stop Pushr                             \033[0m\n");
    }
    if (ENABLE_SIDEKIQ_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -dq            --> Stop Sidekiq                           \033[0m\n");
    }
    if (ENABLE_REDIS_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -ds            --> Stop Redis                             \033[0m\n");
    }
    printf("\033[22;34m  # ./rb_deploy -du            --> Stop Unicorn                           \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### VIEW LOGS ###                                                       \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -l-env         --> View Environment's Log                 \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -l-memcached   --> View Memcached Log                     \033[0m\n");
    if (ENABLE_MONGODB_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -l-mongodb     --> View MongoDB Log                       \033[0m\n");
    }
    if (ENABLE_PUSHR_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -l-pushr       --> View Pushr Log                         \033[0m\n");
    }
    if (ENABLE_REDIS_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -l-redis       --> View Redis Log                         \033[0m\n");
    }
    if (ENABLE_SIDEKIQ_SERVICE == 1)
    {
        printf("\033[22;34m  # ./rb_deploy -l-sidekiq     --> View Sidekiq Log                       \033[0m\n");
    }
    printf("\033[22;34m  # ./rb_deploy -l-unicorn     --> View Unicorn Log                       \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -la-nginx      --> View NGINX Access Log                  \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -le-nginx      --> View NGINX Error Log                   \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### SERVER ###                                                          \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -key           --> Generate Secret Token                  \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -up            --> Server Up                              \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -down          --> Server Down                            \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -deploy / -dep --> Running Deploy                         \033[0m\n");
    printf("\033[22;32m==========================================================================\033[0m\n\n");
}

/* --------------------------------------- 
        Running Command
   --------------------------------------- */
void message_service(char STR_DESCRIPTION[512])
{
    get_time();
    printf("\n\033[22;34m[ %s ] ##### %s     \033[0m\n", DATE_TIME, STR_DESCRIPTION);
}

void message_ok(char STR_SERVICE[512])
{
    get_time();
    printf("\n\033[22;32m[ %s ] :: [ ✔ ] \033[0m", DATE_TIME);
    printf("\033[22;32m %s               \033[0m\n", STR_SERVICE);
}

void message_error(char STR_SERVICE[512])
{
    get_time();
    printf("\n\033[22;31m[ %s ] :: [ ✘ ] \033[0m", DATE_TIME);
    printf("\033[22;32m %s               \033[0m\n", STR_SERVICE);
}

void run_fastcmd(char STR_COMMAND[1024])
{
    sprintf(cmdRun, "%s", STR_COMMAND);
    ret = system(cmdRun);
}

void run_cmd(char STR_SERVICE[512],
             char STR_DESCRIPTION[512],
             char STR_COMMAND[1024])
{
    message_service(STR_DESCRIPTION);
    sprintf(cmdRun, "%s", STR_COMMAND);
    ret = system(cmdRun);
    if (!ret) {
        IS_ERROR_DEPLOY = 0;
        message_ok(STR_SERVICE);
    } else {
        IS_ERROR_DEPLOY = 1;
        message_error(STR_SERVICE);
    }
    /* --------------------------------- *
     *          For Test Only            *
     * --------------------------------- */
    //message_ok(STR_SERVICE);
}

void run_single(char STR_SERVICE[512],
                char STR_DESCRIPTION[512],
                char STR_COMMAND[1024])
{
    message_service(STR_DESCRIPTION);
    sprintf(cmdRun, "%s", STR_COMMAND);
    system(cmdRun);
    message_ok(STR_SERVICE);
}

/* --------------------------------------- 
        NGINX 
   --------------------------------------- */
void nginx_restart() 
{
    select_env();
    char STR_DESCRIPTION[512] = "Restart NGINX Service";
    char STR_SERVICE[512]     = "NGINX Restarting...";
    char STR_COMMAND[1024]    = "sudo /etc/init.d/nginx restart";
    header();
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
    footer();
}

void nginx_reload()
{
    select_env();
    char STR_DESCRIPTION[512] = "Reload NGINX Service";
    char STR_SERVICE[512]     = "NGINX Reloading...";
    char STR_COMMAND[1024]    = "sudo /etc/init.d/nginx reload";
    header();
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
    footer();
}

/* --------------------------------------- 
        MONGODB 
   --------------------------------------- */
void kill_mongodb()
{
    char STR_DESCRIPTION[512] = "Stop MongoDB Service";
    char STR_SERVICE[512]     = "MongoDB Stop...";
    char STR_COMMAND[1024]    = "ps aux | grep -i mongod | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_mongodb()
{
    select_env();
    char STR_DESCRIPTION[512] = "Start MongoDB Service";
    char STR_SERVICE[512]     = "MongoDB Start...";
    char STR_COMMAND[1024];
    // Goto App Current Folder
    get_folder_current();
    sprintf(STR_COMMAND, "cd %s; sudo mongod --fork --logpath %s", APP_CURRENT, SYS_LOG_MONGODB);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void restart_mongodb_process()
{
    kill_mongodb();
    run_mongodb();
}

void restart_mongodb()
{
    header();
    restart_mongodb_process();
    footer();
}

void stop_mongodb()
{
    header();
    kill_mongodb();
    footer();
}

/* --------------------------------------- 
        REDIS 
   --------------------------------------- */
void kill_redis()
{
    char STR_DESCRIPTION[512] = "Stop Redis Service";
    char STR_SERVICE[512]     = "Redis Stop...";
    char STR_COMMAND[1024]    = "ps aux | grep -i redis-server | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_redis()
{
    char STR_DESCRIPTION[512] = "Start Redis Service (Daemonize)";
    char STR_SERVICE[512]     = "Redis Start...";
    char STR_COMMAND[1024]    = "redis-server --daemonize yes";
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void restart_redis_process()
{
    kill_redis();
    run_redis();
}

void restart_redis()
{
    header();
    restart_redis_process();
    footer();
}

void stop_redis()
{
    header();
    kill_redis();
    footer();
}

/* --------------------------------------- 
        Assets
   --------------------------------------- */
void asset_precompile_process()
{
    select_env();
    char STR_DESCRIPTION[512] = "Precompile Assets";
    char STR_SERVICE[512]     = "Precompiling All Assets...";
    char STR_COMMAND[1024];
    // Goto App Current Folder
    get_folder_current();
    if (RAILS_VERSION >= 5)
    {
        // Run: bundle exec rake assets:precompile RAILS_ENV=[environment]
        sprintf(STR_COMMAND, "cd %s; %s exec %s assets:precompile RAILS_ENV=%s --trace", CURRENT_FOLDER, PATH_BUNDLE, PATH_RAILS, ENV);
    }
    else
    {
        // Run: bundle exec rails assets:precompile RAILS_ENV=[environment]
        sprintf(STR_COMMAND, "cd %s; %s exec %s assets:precompile RAILS_ENV=%s --trace", CURRENT_FOLDER, PATH_BUNDLE, PATH_RAKE, ENV);
    }
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void asset_precompile()
{
    header();
    asset_precompile_process();
    footer();
}

void asset_rollback_process()
{
    select_env();
    char STR_DESCRIPTION[512] = "Rollback Assets";
    char STR_SERVICE[512]     = "Rollingback (Cleanup) All Assets...";
    char STR_COMMAND[1024];
    // Goto App Current Folder
    get_folder_current();
    if (RAILS_VERSION >= 5)
    {
        // Run: bundle exec rake assets:clobber RAILS_ENV=[environment]
        sprintf(STR_COMMAND, "cd %s; %s exec %s assets:clobber RAILS_ENV=%s --trace", CURRENT_FOLDER, PATH_BUNDLE, PATH_RAILS, ENV);
    }
    else
    {
        // Run: bundle exec rails assets:clobber RAILS_ENV=[environment]
        sprintf(STR_COMMAND, "cd %s; %s exec %s assets:clobber RAILS_ENV=%s --trace", CURRENT_FOLDER, PATH_BUNDLE, PATH_RAKE, ENV);
    }
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void asset_rollback()
{
    header();
    asset_rollback_process();
    footer();
}

/* --------------------------------------- 
        Unicorn
   --------------------------------------- */
void kill_unicorn()
{
    select_env();
    char STR_DESCRIPTION[512] = "Stop Unicorn Service";
    char STR_SERVICE[512]     = "Unicorn Terminated...";
    char STR_COMMAND[1024]    = "ps aux | grep -i unicorn | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_unicorn()
{
    select_env();
    char STR_DESCRIPTION[512] = "Run Unicorn Service";
    char STR_SERVICE[512]     = "Unicorn Running...";
    char STR_COMMAND[1024];
    // Goto App Current Folder
    get_folder_current();
    // Run: bundle exec unicorn -D -c [config_unicorn] -E [environment]
    sprintf(STR_COMMAND, "cd %s; %s exec %s -D -c %s -E %s", CURRENT_FOLDER, PATH_BUNDLE, PATH_UNICORN, CONFIG_UNICORN, ENV);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void restart_unicorn_process()
{
    kill_unicorn();
    run_unicorn();
}

void restart_unicorn()
{
    header();
    restart_unicorn_process();
    footer();
}

void stop_unicorn()
{
    header();
    kill_unicorn();
    footer();
}

/* --------------------------------------- 
        Faye
   --------------------------------------- */
void kill_faye()
{
    select_env();
    char STR_DESCRIPTION[512] = "Stop Faye Service";
    char STR_SERVICE[512]     = "Faye Terminated...";
    char STR_COMMAND[1024]    = "ps aux | grep -i faye | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_faye()
{
    select_env();
    char STR_DESCRIPTION[512] = "Run Faye Service";
    char STR_SERVICE[512]     = "Faye Running...";
    char STR_COMMAND[1024];
    // Goto App Current Folder
    get_folder_current();
    sprintf(STR_COMMAND, "cd %s; RAILS_ENV=%s %s exec %s %s -E %s -o 0.0.0.0 -D -P %s", CURRENT_FOLDER, ENV, PATH_BUNDLE, PATH_RACKUP, CONFIG_FAYE, ENV, PID_FAYE);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void restart_faye_process()
{
    kill_faye();
    run_faye();
}

void restart_faye()
{
    header();
    restart_faye_process();
    footer();
}

void stop_faye()
{
    header();
    kill_faye();
    footer();
}

/* --------------------------------------- 
        Pushr
   --------------------------------------- */
void kill_pushr()
{
    char STR_DESCRIPTION[512] = "Stop Pushr Service";
    char STR_SERVICE[512]     = "Pushr Terminated...";
    char STR_COMMAND[1024]    = "ps aux | grep -i pushr | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_pushr()
{
    char STR_DESCRIPTION[256] = "Run Pushr Service";
    char STR_SERVICE[256]     = "Pushr Running...";
    char STR_COMMAND[1024];
    // Goto App Current Folder
    get_folder_current();
    sprintf(STR_COMMAND, "cd %s; RAILS_ENV=%s %s exec %s -c %s -p %s >> %s", CURRENT_FOLDER, ENV, PATH_BUNDLE, PATH_PUSHR, CONFIG_PUSHR, PID_PUSHR, SYS_LOG_PUSHR);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
}

void restart_pushr_process()
{
    kill_pushr();
    run_pushr();
}

void restart_pushr()
{
    header();
    restart_pushr_process();
    footer();
}

void stop_pushr()
{
    header();
    kill_pushr();
    footer();
}

/* --------------------------------------- 
        Sidekiq
   --------------------------------------- */
void kill_sidekiq()
{
    char STR_DESCRIPTION[512] = "Stop Sidekiq Service";
    char STR_SERVICE[512]     = "Sidekiq Terminated...";
    char STR_COMMAND[1024]    = "ps aux | grep -i sidekiq | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_sidekiq()
{
    char STR_DESCRIPTION[256] = "Run Sidekiq Service";
    char STR_SERVICE[256]     = "Sidekiq Running...";
    char STR_COMMAND[1024];
    // Goto App Current Folder
    get_folder_current();
    // Run: bundle exec sidekiq -d -e [environment] -C [config_sidekiq] -L [log_sidekiq]
    // sprintf(STR_COMMAND, "cd %s; %s exec %s -d -e %s -C %s -L %s", CURRENT_FOLDER, PATH_BUNDLE, PATH_SIDEKIQ, ENV, CONFIG_SIDEKIQ, SYS_LOG_SIDEKIQ);

    // Run: bundle exec sidekiq --queue default --index 0 --pidfile [pid_sidekiq] --environment [environment] --logfile [log_sidekiq] --concurrency 10 --daemon
    sprintf(STR_COMMAND, "cd %s; %s exec %s --queue default --index 0 --pidfile %s --environment %s --logfile %s --concurrency 10 --daemon", CURRENT_FOLDER, PATH_BUNDLE, PATH_SIDEKIQ, PID_SIDEKIQ, ENV, SYS_LOG_SIDEKIQ);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
}

void restart_sidekiq_process()
{
    kill_sidekiq();
    run_sidekiq();
}

void restart_sidekiq()
{
    header();
    restart_sidekiq_process();
    footer();
}

void stop_sidekiq()
{
    header();
    kill_sidekiq();
    footer();
}

/* --------------------------------------- 
        View Log
   --------------------------------------- */
void log_env()
{
    select_env();
    char STR_DESCRIPTION[256] = "View Environment Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Environment Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f -n %d %s", NUM_LOG_VIEW, SYS_LOG_ENV);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_nginx_error()
{
    select_env();
    char STR_DESCRIPTION[256] = "View NGINX Error Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing NGINX Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f -n %d %s", NUM_LOG_VIEW, SYS_LOG_NGINX_ERROR);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_nginx_access()
{
    select_env();
    char STR_DESCRIPTION[256] = "View NGINX Access Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing NGINX Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f -n %d %s", NUM_LOG_VIEW, SYS_LOG_NGINX_ACCESS);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_mongodb()
{
    select_env();
    char STR_DESCRIPTION[256] = "View MongoDB Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing MongoDB Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f -n %d %s", NUM_LOG_VIEW, SYS_LOG_MONGODB);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_memcached()
{
    select_env();
    char STR_DESCRIPTION[256] = "View Memcached Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Memcached Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f -n %d %s", NUM_LOG_VIEW, SYS_LOG_MEMCACHED);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_redis()
{
    select_env();
    char STR_DESCRIPTION[256] = "View Redis Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Redis Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f -n %d %s", NUM_LOG_VIEW, SYS_LOG_REDIS);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_pusher()
{
    select_env();
    char STR_DESCRIPTION[256] = "View Pushr Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Pushr Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f -n %d %s", NUM_LOG_VIEW, SYS_LOG_PUSHR);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_sidekiq()
{
    select_env();
    char STR_DESCRIPTION[256] = "View Sidekiq Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Sidekiq Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f -n %d %s", NUM_LOG_VIEW, SYS_LOG_SIDEKIQ);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_unicorn()
{
    select_env();
    char STR_DESCRIPTION[256] = "View Unicorn Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Unicorn Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f -n %d %s", NUM_LOG_VIEW, SYS_LOG_UNICORN);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

/* --------------------------------------- 
        Generate Secret Token
   --------------------------------------- */
void generate_secret_token()
{
    select_env();
    char STR_DESCRIPTION[256] = "Generate Secret Token";
    char STR_SERVICE[256]     = "Secret Token Generated...";
    char STR_COMMAND[1024];
    if (RAILS_VERSION >= 5) {
        sprintf(STR_COMMAND, "%s secret", PATH_RAILS);
    } else {
        sprintf(STR_COMMAND, "%s secret", PATH_RAKE);
    }
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

/* --------------------------------------- 
    Deploy Process: 
        1)  [X] Clone Repository to Unix DateTime Release Folder
        2)  [X] Checkout Branch
        3)  [X] Install Bundle  (gem install bundle)
        4)  [X] Install Package (bundle install)
        5)  [X] Remove Shared Folders
        6)  [X] Remove Shared Files
        7)  [X] Create Symlink Shared Folders
        8)  [X] Create Symlink Shared Files
        9)  [X] Compile Assets 
        10) [ ] Install NPM [**optional**]
        11) [X] Migrate Database
        12) [X] Seed Database
        13) [X] Create Symlink Release -> Current
        14) [X] Service Unicorn Stop
        15) [X] Service Unicorn Start
        16) **FINISH**

        Note: On Failed 
        Remove Unix DateTime Release Folder
   --------------------------------------- */
void git_clone()
{
    select_env();
    char STR_DESCRIPTION[512] = "Clone Repository";
    char STR_SERVICE[512]     = "Cloning Process...";
    char STR_COMMAND[1024];
    get_folder();
    sprintf(SNAP_FOLDER_RELEASE, "%s/%s/%s", APP_ROOT, APP_RELEASE, SNAP_FOLDER);
    sprintf(STR_COMMAND, "cd %s; git clone %s %s", APP_ROOT, REPO_NAME, SNAP_FOLDER_RELEASE);
    //printf("%s, %s, %s", SNAP_FOLDER_RELEASE, SNAP_FOLDER, REPO_BRANCHR_COMMAND);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
}

void change_branch()
{
    select_env();
    char STR_DESCRIPTION[512] = "Setup Branch";
    char STR_SERVICE[512]     = "Changing Branch...";
    char STR_COMMAND[1024];
    // Goto App Path Release
    get_folder_release();
    // Checkout Branch
    sprintf(STR_COMMAND, "cd %s; git checkout %s", SNAP_FOLDER_RELEASE, REPO_BRANCH);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void install_bundle()
{
    select_env();
    char STR_DESCRIPTION[512] = "Install Bundle";
    char STR_SERVICE[512]     = "Running: `gem install bundle`...";
    char STR_COMMAND[1024];
    // Goto App Path Release
    get_folder_release();
    // Run: gem install bundle
    sprintf(STR_COMMAND, "cd %s; %s install bundle", SNAP_FOLDER_RELEASE, PATH_GEM);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void install_package()
{
    select_env();
    char STR_DESCRIPTION[512] = "Install Ruby Package";
    char STR_SERVICE[512]     = "Running: `bundle install`...";
    char STR_COMMAND[1024];
    // Goto App Path Release
    get_folder_release();
    // Run: bundle install
    sprintf(STR_COMMAND, "cd %s; %s install", SNAP_FOLDER_RELEASE, PATH_BUNDLE);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void remove_release_folders(char RELEASE_SHARED_FOLDERS[512])
{
    char STR_COMMAND[1024];
    // Goto App Path Release
    get_folder_release();
    // Run: rm -rf [release_folder]
    sprintf(STR_COMMAND, "cd %s; rm -rf %s", APP_ROOT, RELEASE_SHARED_FOLDERS);
    run_fastcmd(STR_COMMAND);
}

void remove_release_files(char RELEASE_SHARED_FILES[512])
{
    char STR_COMMAND[1024];
    // Goto App Path Release
    get_folder_release();
    // Run: rm -f [release_file]
    sprintf(STR_COMMAND, "cd %s; rm -f %s", APP_ROOT, RELEASE_SHARED_FILES);
    run_fastcmd(STR_COMMAND);
}

void initialize_shared_folder()
{
    int index;
    select_env();
    char STR_DESCRIPTION[512] = "Initialize Shared Folders";
    char STR_SERVICE[512]     = "Initializing Shared Folders...";
    char STR_COMMAND[1024];
    char LOOP_SOURCE_FOLDER_SHARED[1024];
    char LOOP_TARGET_FOLDER_SHARED[1024];
    // Goto Root App
    // Looping Create Shared Folders
    message_service(STR_DESCRIPTION);    
    sprintf(SHARED_FOLDER, "%s/%s", APP_ROOT, APP_SHARED);
    get_folder_release();

    for (index = 0; LIST_SHARED_FOLDERS[index] != NULL; ++index)
    {
        sprintf(LOOP_SOURCE_FOLDER_SHARED, "%s/%s", SHARED_FOLDER, LIST_SHARED_FOLDERS[index]);
        sprintf(LOOP_TARGET_FOLDER_SHARED, "%s/%s", SNAP_FOLDER_RELEASE, LIST_SHARED_FOLDERS[index]);
        
        // Remove Release Folders
        remove_release_folders(LOOP_TARGET_FOLDER_SHARED);
        // Create New Symlink From Shared
        sprintf(STR_COMMAND, "cd %s; ln -sfn %s %s", APP_ROOT, LOOP_SOURCE_FOLDER_SHARED, LOOP_TARGET_FOLDER_SHARED);
        run_fastcmd(STR_COMMAND);
    }
    message_ok(STR_SERVICE);
    sleep(1);
}

void initialize_shared_files()
{
    int index;
    select_env();
    char STR_DESCRIPTION[512] = "Initialize Shared Files";
    char STR_SERVICE[512]     = "Initializing Shared Files...";
    char STR_COMMAND[1024];
    char LOOP_SOURCE_FILES_SHARED[1024];
    char LOOP_TARGET_FILES_SHARED[1024];
    // Goto Root App
    // Looping Create Shared Files
    message_service(STR_DESCRIPTION);
    sprintf(SHARED_FOLDER, "%s/%s", APP_ROOT, APP_SHARED);
    get_folder_release();

    for (index = 0; LIST_SHARED_FILES[index] != NULL; ++index)
    {
        sprintf(LOOP_SOURCE_FILES_SHARED, "%s/%s", SHARED_FOLDER, LIST_SHARED_FILES[index]);
        sprintf(LOOP_TARGET_FILES_SHARED, "%s/%s", SNAP_FOLDER_RELEASE, LIST_SHARED_FILES[index]);

        // Remove Release Files
        remove_release_files(LOOP_TARGET_FILES_SHARED);
        // Create New Symlink From Shared
        sprintf(STR_COMMAND, "cd %s; ln -sfn %s %s", APP_ROOT, LOOP_SOURCE_FILES_SHARED, LOOP_TARGET_FILES_SHARED);
        run_fastcmd(STR_COMMAND);
    }
    message_ok(STR_SERVICE);
    sleep(1);
}

void initialize_current()
{
    select_env();
    char STR_DESCRIPTION[512] = "Initialize Current Folder";
    char STR_SERVICE[512]     = "Initializing Current Folder...";
    char STR_COMMAND[1024];
    // Goto App Current Folder
    get_folder_current();
    // Symlink Current Folder From Latest Release
    sprintf(STR_COMMAND, "cd %s; rm -f %s; ln -s %s %s", APP_ROOT, CURRENT_FOLDER, SNAP_FOLDER_RELEASE, CURRENT_FOLDER);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

/* --------------------------------------- 
        Migration & Seed Data
   --------------------------------------- */
void run_migration()
{
    select_env();
    char STR_DESCRIPTION[512] = "Migration Database";
    char STR_SERVICE[512]     = "Running Migration Database...";
    char STR_COMMAND[1024];
    int FORCE_MIGRATION =  1; // Force Migration on Production Environment

    // Goto App Path Release
    get_folder_release();
    // Running migration database in the newest 'release' folder
    if (RAILS_VERSION >= 5) { 
        // >= Rails v5.0
        sprintf(STR_COMMAND, "cd %s; %s exec %s db:migrate RAILS_ENV=%s DISABLE_DATABASE_ENVIRONMENT_CHECK=%d", SNAP_FOLDER_RELEASE, PATH_BUNDLE, PATH_RAILS, ENV, FORCE_MIGRATION);
    } else {
        // < Rails v5.0
        sprintf(STR_COMMAND, "cd %s; %s exec %s db:migrate RAILS_ENV=%s DISABLE_DATABASE_ENVIRONMENT_CHECK=%d", SNAP_FOLDER_RELEASE, PATH_BUNDLE, PATH_RAKE, ENV, FORCE_MIGRATION);
    }
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_seed()
{
    select_env();
    char STR_DESCRIPTION[512] = "Seed Database";
    char STR_SERVICE[512]     = "Running Seed Database...";
    char STR_COMMAND[1024];
    int FORCE_SEED = 1; // Force Seed on Production Environment

    // Goto App Path Release
    get_folder_release();
    // Running seed database in the newest 'release' folder
    if (RAILS_VERSION >= 5) {
        // >= Rails v5.0
        sprintf(STR_COMMAND, "cd %s; %s exec %s db:seed RAILS_ENV=%s DISABLE_DATABASE_ENVIRONMENT_CHECK=%d", SNAP_FOLDER_RELEASE, PATH_BUNDLE, PATH_RAILS, ENV, FORCE_SEED);
    } else {
        // < Rails v5.0
        sprintf(STR_COMMAND, "cd %s; %s exec %s db:seed RAILS_ENV=%s DISABLE_DATABASE_ENVIRONMENT_CHECK=%d", SNAP_FOLDER_RELEASE, PATH_BUNDLE, PATH_RAKE, ENV, FORCE_SEED);
    }
    // NOTE: Specific seed database class
    // bundle exec rake db:seed SEED_FILES=[class_seed_name] RAILS_ENV=[environment]
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_migration_rollback()
{
    select_env();
    char STR_DESCRIPTION[512] = "Migration Rollback";
    char STR_SERVICE[512]     = "Running Rollback Migration...";
    char STR_COMMAND[1024];
    int ROLLBACK = 1;  // Number of Rollback Step
    
    // Goto App Path Release
    get_folder_release();
    // Running rollback migration in the newest 'release' folder
    if (RAILS_VERSION >= 5) {
        // >= Rails v5.0
        sprintf(STR_COMMAND, "cd %s; %s exec %s db:rollback STEP=%d RAILS_ENV=%s", SNAP_FOLDER_RELEASE, PATH_BUNDLE, PATH_RAILS, ROLLBACK, ENV);
    } else {
        // < Rails v5.0
        sprintf(STR_COMMAND, "cd %s; %s exec %s db:rollback STEP=%d RAILS_ENV=%s", SNAP_FOLDER_RELEASE, PATH_BUNDLE, PATH_RAKE, ROLLBACK, ENV);
    }
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

/* --------------------------------------- 
        Preinstall Script
   --------------------------------------- */
void run_preinstall()
{
    select_env();
    char STR_DESCRIPTION[512] = "Preinstallation";
    char STR_SERVICE[512]     = "Running Preinstall Configuration...";
    char STR_COMMAND[1024];
    // Goto Current Path
    get_folder_release();
    // Symlink preinstall script to 'release' folder
    // Running Preinstallation in the newest 'release' folder
    sprintf(CURRENT_FOLDER, "%s/%s", APP_ROOT, APP_CURRENT);
    sprintf(STR_COMMAND, "cd %s; ln -s %s/%s %s; sudo /bin/sh %s", CURRENT_FOLDER, APP_ROOT, PREINSTALL, PREINSTALL, PREINSTALL);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

/* --------------------------------------- 
        Restart Server
   --------------------------------------- */
void server_up_process()
{
    // Production Environment
    if (strcmp(ENV, "production") == 0)
    {
        if (ENABLE_FAYE_SERVICE == 1) { restart_faye_process(); }
        if (ENABLE_PUSHR_SERVICE == 1) { restart_pushr_process(); }
    }
    if (ENABLE_MONGODB_SERVICE == 1) { restart_mongodb_process(); }
    if (ENABLE_SIDEKIQ_SERVICE == 1) { restart_sidekiq_process(); }
    restart_unicorn_process();
}

void server_up()
{
    header();
    server_up_process();
    footer();
}

/* --------------------------------------- 
        Shutdown Server
   --------------------------------------- */
void server_down_process()
{
    // Production Environment
    if (strcmp(ENV, "production") == 0)
    {
        if (ENABLE_FAYE_SERVICE == 1) { kill_faye(); }
        if (ENABLE_PUSHR_SERVICE == 1) { kill_pushr(); }
    }
    if (ENABLE_MONGODB_SERVICE == 1) { kill_mongodb(); }
    if (ENABLE_SIDEKIQ_SERVICE == 1) { kill_sidekiq(); }
    kill_unicorn();
}

void server_down()
{
    header();
    server_down_process();
    footer();
}

/* --------------------------------------- 
        Deploy
   --------------------------------------- */
void deploy_rollback()
{
    if (IS_ROLLBACK != 0)
    {
        run_migration_rollback();
    }
}

void remove_release_clone()
{
    select_env();
    char STR_DESCRIPTION[512] = "Rollback Deploy";
    char STR_SERVICE[512]     = "Rollingback Deployment Process...";
    char STR_COMMAND[1024];
    // Goto Root Path
    get_folder_release();
    // Remove All Cloned Folder (SNAP_FOLDER)
    sprintf(STR_COMMAND, "cd %s; rm -rf %s", APP_ROOT, SNAP_FOLDER_RELEASE);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void deploy()
{
    header();
    git_clone();
    change_branch();
    install_bundle();
    if (ENABLE_BUNDLE_INSTALL == 1) {
        install_package();
    }
    initialize_shared_folder();
    initialize_shared_files();
    if (ENABLE_COMPILE_ASSETS == 1)
    {
        if (ENABLE_CLOBBER_ASSETS == 1) { asset_rollback_process(); }
        asset_precompile_process();
    }
    if (ENABLE_MIGRATION == 1)
    {
        run_migration();
        if (IS_ERROR_DEPLOY == 0) { run_seed(); } 
        else {
            IS_ROLLBACK = 1;
            deploy_rollback();
        }
    }
    if (IS_ERROR_DEPLOY == 0)
    {
        initialize_current();
        run_preinstall();

        /* --------------------------------------- 
                Server Up
           --------------------------------------- */
        server_up_process();
    } else {
        deploy_rollback();
        remove_release_clone();
    }    
    footer();
}

/* ======================================= 
        MAIN PROGRAM
   ======================================= */

int main(int argc, char **argv) {

    if ((argc != 2)) {
        menu();
        return (EXIT_SUCCESS);
    }

    // NGINX
    if (strcmp(argv[1], "-nr") == 0) {
        nginx_restart();
    } else if (strcmp(argv[1], "-no") == 0) {
        nginx_reload();

    // Assets
    } else if (strcmp(argv[1], "-ac") == 0) {
        asset_rollback(); 
    } else if (strcmp(argv[1], "-ap") == 0) {
        asset_precompile();

    // Restart Services
    } else if (strcmp(argv[1], "-rf") == 0) {
        restart_faye();
    } else if (strcmp(argv[1], "-rp") == 0) {
        restart_pushr();
    } else if (strcmp(argv[1], "-rm") == 0) {
        restart_mongodb();
    } else if (strcmp(argv[1], "-rq") == 0) {
        restart_sidekiq();
    } else if (strcmp(argv[1], "-rs") == 0) {
        restart_redis();
    } else if (strcmp(argv[1], "-ru") == 0) {
        restart_unicorn();

    // Stop Services
    } else if (strcmp(argv[1], "-df") == 0) {
        stop_faye();
    } else if (strcmp(argv[1], "-dp") == 0) {
        stop_pushr();
    } else if (strcmp(argv[1], "-dm") == 0) {
        stop_mongodb();
    } else if (strcmp(argv[1], "-dq") == 0) {
        stop_sidekiq();
    } else if (strcmp(argv[1], "-ds") == 0) {
        stop_redis();
    } else if (strcmp(argv[1], "-du") == 0) {
        stop_unicorn();

    // View Log
    } else if (strcmp(argv[1], "-l-env") == 0) {
        log_env();
    } else if (strcmp(argv[1], "-l-memcached") == 0) {
        log_memcached();
    } else if (strcmp(argv[1], "-l-mongodb") == 0) {
        log_mongodb();
    } else if (strcmp(argv[1], "-l-redis") == 0) {
        log_redis();    
    } else if (strcmp(argv[1], "-l-pushr") == 0) {
        log_pusher();
    } else if (strcmp(argv[1], "-l-sidekiq") == 0){
        log_sidekiq();
    } else if (strcmp(argv[1], "-l-unicorn") == 0){
        log_unicorn();    
    } else if (strcmp(argv[1], "-la-nginx") == 0) {
        log_nginx_access();
    } else if (strcmp(argv[1], "-le-nginx") == 0) {
        log_nginx_error();
    
    // Server
    } else if (strcmp(argv[1], "-key") == 0) {
        generate_secret_token(); 
    } else if (strcmp(argv[1], "-up") == 0) {
        server_up(); 
    } else if (strcmp(argv[1], "-down") == 0) {
        server_down();

    // Deploy 
    } else if ( (strcmp(argv[1], "-deploy") == 0) || (strcmp(argv[1], "-dep") == 0) ) {
        deploy();
    } else {
        menu();
    }

    return (EXIT_SUCCESS);
}
