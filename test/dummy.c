/* 
 * File:   rb_deploy.c
 * Author: ZeroC0D3 LAB (zeroc0d3)
 *
 * Created on Oct 22nd, 2017, 08:00 
 * 
 */

#include <stdio.h>
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
char STR_FOLDER[512];

/* ======================================= 
        CONFIGURATION 
   ======================================= */
char VERSION[16] = "1.2.6";               // Version 
int NUM_RELEASE  = 10;                    // Maximum Number of Release Folder 
char ENV[64]     = "development";         // Selected Environment (development / production)

char APP_ROOT[512];                       // Root Path
char APP_CURRENT[64] = "current";         // Current Folder
char APP_RELEASE[64] = "release";         // Release Folder
char APP_SHARED[64]  = "shared";          // Shared Folder
char CURRENT_FOLDER[1024];                // CURRENT_FOLDER = APP_ROOT/APP_CURRENT
char SHARED_FOLDER[1024];                 // SHARED_FOLDER  = APP_ROOT/APP_SHARED
char PREINSTALL[64]  = "preinstall.sh";   // Preinstallation Script Before Server-Up

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
char PID_SIDEKIQ[512];               // Path PID Sidekiq
// Binary
char PATH_UNICORN[512];             // Path of Unicorn Binary
char PATH_RAKE[512];                // Path of Rake Binary
char PATH_RAILS[512];               // Path of Rails Binary
char PATH_RACKUP[512];              // Path of Rackup Binary
char PATH_GEM[512];                 // Path of Gem Binary
char PATH_BUNDLE[512];              // Path of Bundle Binary
// Log
char SYS_LOG_PUSHR[512];                                             // Path Log Pushr
char SYS_LOG_SIDEKIQ[512];                                           // Path Log Sidekiq
char SYS_LOG_UNICORN[512];                                           // Path Log Unicorn
char SYS_LOG_NGINX_ERROR[512]  = "/var/log/nginx/error.log";         // Path Log NGINX Error
char SYS_LOG_NGINX_ACCESS[512] = "/var/log/nginx/access.log";        // Path Log NGINX Access
char SYS_LOG_MONGODB[512]      = "/var/log/mongodb.log";             // Path Log MongoDB
char SYS_LOG_MEMCACHED[521]    = "/var/log/memcached.log";           // Path Log Memcached
char SYS_LOG_REDIS[521]        = "/var/log/redis/redis-server.log";  // Path Log Redis

// Repository
char REPO_NAME[1024] = "git@github.com:zeroc0d3/ruby-installation.git";
char REPO_BRANCH[64] = "master";

// DEVELOPMENT CONFIGURATION //
// Development Environment
char DEV_APP_ROOT[512]        = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary";                                // Development Root Path
char DEV_CONFIG_UNICORN[512]  = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/config/unicorn/staging.rb";      // Development Unicorn Config
char DEV_CONFIG_FAYE[512]     = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/faye.ru";                        // Development Faye Config
char DEV_CONFIG_PUSHR[512]    = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/config/pushr-development.yaml";  // Development Pushr Config
char DEV_CONFIG_SIDEKIQ[512]  = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/config/sidekiq.yml";             // Development Sidekiq Config

char DEV_PID_UNICORN[512]     = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/tmp/pids/unicorn.pid";           // Development Path PID Unicorn
char DEV_PID_FAYE[512]        = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/tmp/pids/faye.pid";              // Development Path PID Faye
char DEV_PID_PUSHR[512]       = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/tmp/pids/pushr.pid";             // Development Path PID Pushr
char DEV_PID_SIDEKIQ[512]     = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/tmp/pids/sidekiq.pid";           // Development Path PID Sidekiq

char DEV_LOG_PUSHR[512]       = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/log/pushr.log";                  // Development Path Log Pushr
char DEV_LOG_SIDEKIQ[512]     = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/log/sidekiq.log";                // Development Path Log Sidekiq
char DEV_LOG_UNICORN[512]     = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/log/unicorn.log";                // Development Path Log Unicorn

char DEV_PATH_UNICORN[512]    = "/home/zeroc0d3/.rbenv/shims/unicorn";                 // Development Path of Unicorn Binary
// < Rails v5.0
char DEV_PATH_RAKE[512]       = "/home/zeroc0d3/.rbenv/shims/rake";                    // Development Path of Rake Binary
// >= Rails v5.0
char DEV_PATH_RAILS[512]      = "/home/zeroc0d3/.rbenv/shims/rails";                   // Development Path of Rails Binary
char DEV_PATH_RACKUP[512]     = "/home/zeroc0d3/.rbenv/shims/rackup";                  // Development Path of Rackup Binary
char DEV_PATH_GEM[512]        = "/home/zeroc0d3/.rbenv/shims/gem";                     // Development Path of Gem Binary
char DEV_PATH_BUNDLE[512]     = "/home/zeroc0d3/.rbenv/shims/bundle";                  // Development Path of Bundle Binary

// PRODUCTION CONFIGURATION //
// Production Environment
char PROD_APP_ROOT[512]       = "/home/zeroc0d3/deploy";                               // Production Root Path
char PROD_CONFIG_UNICORN[512] = "/home/zeroc0d3/deploy/config/unicorn/production.rb";  // Production Unicorn Config
char PROD_CONFIG_FAYE[512]    = "/home/zeroc0d3/deploy/faye.ru";                       // Production Faye Config
char PROD_CONFIG_PUSHR[512]   = "/home/zeroc0d3/deploy/config/pushr-production.yaml";  // Production Pushr Config
char PROD_CONFIG_SIDEKIQ[512] = "/home/zeroc0d3/deploy/config/sidekiq.yml";            // Production Sidekiq Config

char PROD_PID_UNICORN[512]    = "/home/zeroc0d3/deploy/tmp/pids/unicorn.pid";          // Production Path PID Unicorn
char PROD_PID_FAYE[512]       = "/home/zeroc0d3/deploy/tmp/pids/faye.pid";             // Production Path PID Faye
char PROD_PID_PUSHR[512]      = "/home/zeroc0d3/deploy/tmp/pids/pushr.pid";            // Production Path PID Pushr
char PROD_PID_SIDEKIQ[512]    = "/home/zeroc0d3/deploy/tmp/pids/sidekiq.pid";          // Production Path PID Sidekiq

char PROD_LOG_PUSHR[512]      = "/home/zeroc0d3/deploy/log/pushr.log";                 // Production Path Log Pushr
char PROD_LOG_SIDEKIQ[512]    = "/home/zeroc0d3/deploy/log/sidekiq.log";               // Production Path Log Sidekiq
char PROD_LOG_UNICORN[512]    = "/home/zeroc0d3/deploy/log/unicorn.log";               // Production Path Log Unicorn

char PROD_PATH_UNICORN[512]   = "/home/zeroc0d3/.rbenv/shims/unicorn";                 // Production Path of Unicorn Binary
// < Rails v5.0
char PROD_PATH_RAKE[512]      = "/home/zeroc0d3/.rbenv/shims/rake";                    // Production Path of Rake Binary
// >= Rails v5.0
char PROD_PATH_RAILS[512]     = "/home/zeroc0d3/.rbenv/shims/rails";                   // Production Path of Rails Binary
char PROD_PATH_RACKUP[512]    = "/home/zeroc0d3/.rbenv/shims/rackup";                  // Production Path of Rackup Binary
char PROD_PATH_GEM[512]       = "/home/zeroc0d3/.rbenv/shims/gem";                     // Production Path of Gem Binary
char PROD_PATH_BUNDLE[512]    = "/home/zeroc0d3/.rbenv/shims/bundle";                  // Production Path of Bundle Binary

char *SHARED_FOLDERS[] = {
    "log",
    "tmp/pids",
    "tmp/cache",
    "tmp/sockets",
    "vendor/bundle",
    "public/uploads",
    "public/system",
    "public/assets"};

char *SHARED_FILES[] = {
    "config/application.yml",
    "config/database.yml",
    "config/mongoid.yml",
    "config/secrets.yml",
    "config/sidekiq.yml"};

/* ======================================= 
        SUB MAIN PROGRAM
   ======================================= */
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
    printf("\033[22;31m# BEGIN PROCESS..... (Please Wait)  \033[0m\n");
    printf("\033[22;31m# Start at: %s  \033[0m\n", DATE_TIME);
}

void footer() {
    printf("\033[22;32m==========================================================================\033[0m\n");
    get_time();
    printf("\033[22;31m# Finish at: %s  \033[0m\n", DATE_TIME);
    printf("\033[22;31m# END PROCESS.....                  \033[0m\n\n");
}

void menu()
{
    system("clear");
    logo();
    printf("\033[22;32m==========================================================================\033[0m\n");
    printf("\033[22;32m  ### NGINX SERVICES ###                                                  \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -nr            --> Restart NGINX                          \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -no            --> Reload NGINX                           \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### ASSETS SERVICES ###                                                 \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -ap            --> Assets Precompile                      \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -ac            --> Assets Clobber (Rollback)              \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### RESTART SERVICES ###                                                \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -ru            --> Restart Unicorn                        \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -rf            --> Restart Faye                           \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -rp            --> Restart Pushr                          \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -rm            --> Restart MongoDB                        \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -rq            --> Restart Sidekiq                        \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -rs            --> Restart Redis                          \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### STOP SERVICES ###                                                   \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -df            --> Stop Faye                              \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -dp            --> Stop Pushr                             \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -dq            --> Stop Sidekiq                           \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -ds            --> Stop Redis                             \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### VIEW LOGS ###                                                       \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -le-nginx      --> View NGINX Error Log                   \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -la-nginx      --> View NGINX Access Log                  \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -l-mongodb     --> View MongoDB Log                       \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -l-memcached   --> View Memcached Log                     \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -l-redis       --> View Redis Log                         \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -l-pushr       --> View Pushr Log                         \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -l-sidekiq     --> View Sidekiq Log                       \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -l-unicorn     --> View Unicorn Log                       \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;32m  ### SERVER ###                                                          \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -up            --> Server Up                              \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -down          --> Server Down                            \033[0m\n");
    printf("\033[22;34m  # ./rb_deploy -deploy / -dep --> Running Deploy                         \033[0m\n");
    printf("\033[22;32m==========================================================================\033[0m\n\n");
}

/* --------------------------------------- 
        Select Environment
   --------------------------------------- */
void select_env()
{
    int environment = strcmp(ENV, "development");
    if (environment == 0) {
        sprintf(APP_ROOT, "%s", DEV_APP_ROOT);
        sprintf(CONFIG_UNICORN, "%s", DEV_CONFIG_UNICORN);
        sprintf(CONFIG_FAYE, "%s", DEV_CONFIG_FAYE);
        sprintf(CONFIG_PUSHR, "%s", DEV_CONFIG_PUSHR);
        sprintf(CONFIG_SIDEKIQ, "%s", DEV_CONFIG_SIDEKIQ);
        sprintf(PID_UNICORN, "%s", DEV_PID_UNICORN);
        sprintf(PID_FAYE, "%s", DEV_PID_FAYE);
        sprintf(PID_PUSHR, "%s", DEV_PID_PUSHR);
        sprintf(PID_SIDEKIQ, "%s", DEV_PID_SIDEKIQ);
        sprintf(SYS_LOG_PUSHR, "%s", DEV_LOG_PUSHR);
        sprintf(SYS_LOG_SIDEKIQ, "%s", DEV_LOG_SIDEKIQ);
        sprintf(SYS_LOG_UNICORN, "%s", DEV_LOG_UNICORN);
        sprintf(PATH_UNICORN, "%s", DEV_PATH_UNICORN);
        sprintf(PATH_RAKE, "%s", DEV_PATH_RAKE);
        sprintf(PATH_RAILS, "%s", DEV_PATH_RAILS);
        sprintf(PATH_RACKUP, "%s", DEV_PATH_RACKUP);
        sprintf(PATH_GEM, "%s", DEV_PATH_GEM);
        sprintf(PATH_BUNDLE, "%s", DEV_PATH_BUNDLE);
    }
    else
    {
        sprintf(APP_ROOT, "%s", PROD_APP_ROOT);
        sprintf(CONFIG_UNICORN, "%s", PROD_CONFIG_UNICORN);
        sprintf(CONFIG_FAYE, "%s", PROD_CONFIG_FAYE);
        sprintf(CONFIG_PUSHR, "%s", PROD_CONFIG_PUSHR);
        sprintf(CONFIG_SIDEKIQ, "%s", PROD_CONFIG_SIDEKIQ);
        sprintf(PID_UNICORN, "%s", PROD_PID_UNICORN);
        sprintf(PID_FAYE, "%s", PROD_PID_FAYE);
        sprintf(PID_PUSHR, "%s", PROD_PID_PUSHR);
        sprintf(PID_SIDEKIQ, "%s", PROD_PID_SIDEKIQ);
        sprintf(SYS_LOG_PUSHR, "%s", PROD_LOG_PUSHR);
        sprintf(SYS_LOG_SIDEKIQ, "%s", PROD_LOG_SIDEKIQ);
        sprintf(SYS_LOG_UNICORN, "%s", PROD_LOG_UNICORN);
        sprintf(PATH_UNICORN, "%s", PROD_PATH_UNICORN);
        sprintf(PATH_RAKE, "%s", PROD_PATH_RAKE);
        sprintf(PATH_RAILS, "%s", PROD_PATH_RAILS);
        sprintf(PATH_RACKUP, "%s", PROD_PATH_RACKUP);
        sprintf(PATH_GEM, "%s", PROD_PATH_GEM);
        sprintf(PATH_BUNDLE, "%s", PROD_PATH_BUNDLE);
    }
}

/* --------------------------------------- 
        Running Command
   --------------------------------------- */
void run_fastcmd(char STR_COMMAND[1024])
{
    sprintf(cmdRun, "%s", STR_COMMAND);
    ret = system(cmdRun);
}

void run_cmd(char STR_SERVICE[300],
             char STR_DESCRIPTION[300],
             char STR_COMMAND[1024])
{
    get_time();
    printf("\n\033[22;34m[ %s ] ##### %s     \033[0m\n", DATE_TIME, STR_DESCRIPTION);
    sprintf(cmdRun, "%s", STR_COMMAND);
    // ret = system(cmdRun);
    // if (!ret) {
    //     get_time();
    //     printf("\n\033[22;32m[ %s ] :: [ ✔ ] \033[0m", DATE_TIME);
    //     printf("\033[22;32m %s               \033[0m\n", STR_SERVICE);
    // } else {
    //     get_time();
    //     printf("\n\033[22;31m[ %s ] :: [ ✘ ] \033[0m", DATE_TIME);
    //     printf("\033[22;32m %s               \033[0m\n", STR_SERVICE);
    // }
    get_time();
    printf("\n\033[22;32m[ %s ] :: [ ✔ ] \033[0m", DATE_TIME);
    printf("\033[22;32m %s               \033[0m\n", STR_SERVICE);
}

void run_single(char STR_SERVICE[300],
                char STR_DESCRIPTION[300],
                char STR_COMMAND[1024])
{
    get_time();
    printf("\n\033[22;34m[ %s ] ##### %s     \033[0m\n", DATE_TIME, STR_DESCRIPTION);
    sprintf(cmdRun, "%s", STR_COMMAND);
    // system(cmdRun);
    get_time();
    printf("\n\033[22;32m[ %s ] :: [ ✔ ] \033[0m", DATE_TIME);
    printf("\033[22;32m %s               \033[0m\n", STR_SERVICE);
}

/* --------------------------------------- 
        NGINX 
   --------------------------------------- */
void nginx_restart() 
{
    select_env();
    char STR_DESCRIPTION[300] = "Restart NGINX Service";
    char STR_SERVICE[300]     = "NGINX Restarting...";
    char STR_COMMAND[1024]    = "sudo /etc/init.d/nginx restart";
    header();
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
    footer();
}

void nginx_reload()
{
    select_env();
    char STR_DESCRIPTION[300] = "Reload NGINX Service";
    char STR_SERVICE[300]     = "NGINX Reloading...";
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
    char STR_DESCRIPTION[300] = "Stop MongoDB Service";
    char STR_SERVICE[300]     = "MongoDB Stop...";
    char STR_COMMAND[1024]    = "ps aux | grep -i mongod | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_mongodb()
{
    select_env();
    char STR_DESCRIPTION[300] = "Start MongoDB Service";
    char STR_SERVICE[300]     = "MongoDB Start...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "cd %s; sudo mongod --fork --logpath %s", APP_ROOT, SYS_LOG_MONGODB);
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
    char STR_DESCRIPTION[300] = "Stop Redis Service";
    char STR_SERVICE[300]     = "Redis Stop...";
    char STR_COMMAND[1024]    = "ps aux | grep -i redis-server | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_redis()
{
    char STR_DESCRIPTION[300] = "Start Redis Service (Daemonize)";
    char STR_SERVICE[300]     = "Redis Start...";
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
void asset_precompile()
{
    select_env();
    char STR_DESCRIPTION[300] = "Precompile Assets";
    char STR_SERVICE[300]     = "Precompiling All Assets...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "cd %s; %s assets:precompile RAILS_ENV=%s --trace", APP_ROOT, PATH_RAKE, ENV);
    header();
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
    footer();
}

void asset_rollback()
{
    select_env();
    char STR_DESCRIPTION[300] = "Rollback Assets";
    char STR_SERVICE[300]     = "Rollingback (Cleanup) All Assets...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "cd %s; %s assets:clobber RAILS_ENV=%s --trace", APP_ROOT, PATH_RAKE, ENV);
    header();
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
    footer();
}

/* --------------------------------------- 
        Unicorn
   --------------------------------------- */
void kill_unicorn()
{
    select_env();
    char STR_DESCRIPTION[300] = "Stop Unicorn Service";
    char STR_SERVICE[300]     = "Unicorn Terminated...";
    char STR_COMMAND[1024]    = "ps aux | grep -i unicorn | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_unicorn()
{
    select_env();
    char STR_DESCRIPTION[300] = "Run Unicorn Service";
    char STR_SERVICE[300]     = "Unicorn Running...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "cd %s; %s -D -c %s -E %s", APP_ROOT, PATH_UNICORN, CONFIG_UNICORN, ENV);
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
    char STR_DESCRIPTION[300] = "Stop Faye Service";
    char STR_SERVICE[300]     = "Faye Terminated...";
    char STR_COMMAND[1024]    = "ps aux | grep -i faye | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_faye()
{
    select_env();
    char STR_DESCRIPTION[300] = "Run Faye Service";
    char STR_SERVICE[300]     = "Faye Running...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "cd %s; RAILS_ENV=%s %s %s -E %s -o 0.0.0.0 -D -P %s", APP_ROOT, ENV, PATH_RACKUP, CONFIG_FAYE, ENV, PID_FAYE);
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
    char STR_DESCRIPTION[300] = "Stop Pushr Service";
    char STR_SERVICE[300]     = "Pushr Terminated...";
    char STR_COMMAND[1024]    = "ps aux | grep -i pushr | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_pushr()
{
    char STR_DESCRIPTION[256] = "Run Pushr Service";
    char STR_SERVICE[256]     = "Pushr Running...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "cd %s; RAILS_ENV=%s %s exec pushr -c %s -p %s >> %s", APP_ROOT, ENV, PATH_BUNDLE, CONFIG_PUSHR, PID_PUSHR, SYS_LOG_PUSHR);
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
    char STR_DESCRIPTION[300] = "Stop Sidekiq Service";
    char STR_SERVICE[300]     = "Sidekiq Terminated...";
    char STR_COMMAND[1024]    = "ps aux | grep -i sidekiq | awk {'print $2'} | sudo xargs kill -9";
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_sidekiq()
{
    char STR_DESCRIPTION[256] = "Run Sidekiq Service";
    char STR_SERVICE[256]     = "Sidekiq Running...";
    char STR_COMMAND[1024];
    // sprintf(STR_COMMAND, "cd %s; %s exec sidekiq -d -e %s -C %s -L %s", APP_ROOT, PATH_BUNDLE, ENV, CONFIG_SIDEKIQ, SYS_LOG_SIDEKIQ);
    sprintf(STR_COMMAND, "cd %s; %s exec sidekiq --queue default --index 0 --pidfile %s --environment %s --logfile %s --concurrency 10 --daemon", APP_ROOT, PATH_BUNDLE, PID_SIDEKIQ, ENV, SYS_LOG_SIDEKIQ);
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
void log_nginx_error()
{
    char STR_DESCRIPTION[256] = "View NGINX Error Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing NGINX Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f %s", SYS_LOG_NGINX_ERROR);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_nginx_access()
{
    char STR_DESCRIPTION[256] = "View NGINX Access Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing NGINX Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f %s", SYS_LOG_NGINX_ACCESS);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_mongodb()
{
    char STR_DESCRIPTION[256] = "View MongoDB Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing MongoDB Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f %s", SYS_LOG_MONGODB);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_memcached()
{
    char STR_DESCRIPTION[256] = "View Memcached Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Memcached Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f %s", SYS_LOG_MEMCACHED);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_redis()
{
    char STR_DESCRIPTION[256] = "View Redis Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Redis Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f %s", SYS_LOG_REDIS);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_pusher()
{
    char STR_DESCRIPTION[256] = "View Pushr Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Pushr Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f %s", SYS_LOG_PUSHR);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_sidekiq()
{
    char STR_DESCRIPTION[256] = "View Sidekiq Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Sidekiq Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f %s", SYS_LOG_SIDEKIQ);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

void log_unicorn()
{
    char STR_DESCRIPTION[256] = "View Unicorn Log (Ctrl+C to Exit)";
    char STR_SERVICE[256]     = "Viewing Unicorn Log...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "sudo tail -f %s", SYS_LOG_UNICORN);
    header();
    run_single(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    footer();
}

/* --------------------------------------- 
    Deploy Process: 
        1) Clone Repository to Unix DateTime Release Folder
        2) Checkout Branch
        3) Remove Shared Folders
        4) Remove Shared Files
        5) Create Symlink Shared Folders
        6) Create Symlink Shared Files
        7) Install Bundle  (gem install bundle)
        8) Install Package (bundle install)
        9) Compile Assets 
        10) Install NPM
        11) Migrate Database
        12) Seed Database
        13) Create Symlink Release -> Current
        14) Service Unicorn Stop
        15) Service Unicorn Start
        16) FINISH

        Note: On Failed 
        Remove Unix DateTime Release Folder
   --------------------------------------- */
void git_clone()
{
    select_env();
    char STR_DESCRIPTION[300] = "Clone Repository";
    char STR_SERVICE[300]     = "Cloning Process...";
    char STR_COMMAND[1024];
    get_folder();
    sprintf(STR_FOLDER, "%s/%s/%s", APP_ROOT, APP_RELEASE, SNAP_FOLDER);
    sprintf(STR_COMMAND, "cd %s; git clone %s %s", APP_ROOT, REPO_NAME, STR_FOLDER);
    //printf("%s, %s, %s", STR_FOLDER, SNAP_FOLDER, REPO_BRANCHR_COMMAND);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
}

void change_branch()
{
    select_env();
    char STR_DESCRIPTION[300] = "Setup Branch";
    char STR_SERVICE[300]     = "Changing Branch...";
    char STR_COMMAND[1024];
    // Goto App Path Release
    // Checkout Branch
    sprintf(STR_COMMAND, "cd %s; git checkout %s", STR_FOLDER, REPO_BRANCH);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void install_bundle()
{
    select_env();
    char STR_DESCRIPTION[300] = "Install Bundle";
    char STR_SERVICE[300]     = "Running: `gem install bundle`...";
    char STR_COMMAND[1024];
    // Goto App Path Release
    // Run: gem install bundle
    sprintf(STR_COMMAND, "cd %s; %s install bundle", STR_FOLDER, PATH_GEM);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void install_package()
{
    select_env();
    char STR_DESCRIPTION[300] = "Install Ruby Package";
    char STR_SERVICE[300]     = "Running: `bundle install`...";
    char STR_COMMAND[1024];
    // Goto App Path Release
    // Run: bundle install
    sprintf(STR_COMMAND, "cd %s; %s install", STR_FOLDER, PATH_BUNDLE);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void initialize_shared_folder()
{
    select_env();
    char STR_DESCRIPTION[300] = "Initialize Shared Folders";
    char STR_SERVICE[300]     = "Initializing Shared Folders...";
    char STR_COMMAND[1024];
    char LOOP_FOLDER_SHARED[1024];
    char LOOP_PATH_FOLDER_SHARED[1024];
    // Goto Root App
    // Looping Create Shared Folders
    sprintf(SHARED_FOLDER, "%s/%s", APP_ROOT, APP_SHARED);
    sprintf(STR_COMMAND, "cd %s; ln -sfn %s %s", APP_ROOT, LOOP_FOLDER_SHARED, LOOP_PATH_FOLDER_SHARED);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void initialize_shared_files()
{
    select_env();
    char STR_DESCRIPTION[300] = "Initialize Shared Files";
    char STR_SERVICE[300]     = "Initializing Shared Files...";
    char STR_COMMAND[1024];
    char LOOP_FILE_SHARED[1024];
    char LOOP_PATH_FILE_SHARED[1024];
    // Goto Root App
    // Looping Create Shared Files
    sprintf(SHARED_FOLDER, "%s/%s", APP_ROOT, APP_SHARED);
    sprintf(STR_COMMAND, "cd %s; ln -sfn %s %s", APP_ROOT, LOOP_FILE_SHARED, LOOP_PATH_FILE_SHARED);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void initialize_current()
{
    select_env();
    char STR_DESCRIPTION[300] = "Initialize Current Folder";
    char STR_SERVICE[300]     = "Initializing Current Folder...";
    char STR_COMMAND[1024];
    // Goto Root App
    // Symlink Current Folder From Latest Release
    sprintf(CURRENT_FOLDER, "%s/%s", APP_ROOT, APP_CURRENT);
    sprintf(STR_COMMAND, "cd %s; rm -f %s; ln -s %s %s", APP_ROOT, CURRENT_FOLDER, STR_FOLDER, CURRENT_FOLDER);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

/* --------------------------------------- 
        Migration & Seed Data
   --------------------------------------- */
void run_migration()
{
    select_env();
    char STR_DESCRIPTION[300] = "Migration Database";
    char STR_SERVICE[300]     = "Running Migration Database...";
    char STR_COMMAND[1024];
    int FORCE_MIGRATION =  1; // Force Migration on Production Environment
    
    // Goto Root App
    // Running migration database in the newest 'release' folder
    sprintf(STR_FOLDER, "%s/%s/%s", APP_ROOT, APP_RELEASE, SNAP_FOLDER);

    // < Rails v5.0
    sprintf(STR_COMMAND, "cd %s; %s exec %s db:migrate RAILS_ENV=%s DISABLE_DATABASE_ENVIRONMENT_CHECK=%d", STR_FOLDER, PATH_BUNDLE, PATH_RAKE, ENV, FORCE_MIGRATION);
    // >= Rails v5.0
    // sprintf(STR_COMMAND, "cd %s; %s exec %s db:migrate STEP=%s RAILS_ENV=%s DISABLE_DATABASE_ENVIRONMENT_CHECK=%s", STR_FOLDER, PATH_BUNDLE, PATH_RAILS, ENV, FORCE_MIGRATION);

    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_seed()
{
    select_env();
    char STR_DESCRIPTION[300] = "Seed Database";
    char STR_SERVICE[300]     = "Running Seed Database...";
    char STR_COMMAND[1024];
    int FORCE_SEED = 1; // Force Seed on Production Environment

    // Goto Root App
    // Running seed database in the newest 'release' folder
    sprintf(STR_FOLDER, "%s/%s/%s", APP_ROOT, APP_RELEASE, SNAP_FOLDER);

    // < Rails v5.0
    sprintf(STR_COMMAND, "cd %s; %s exec %s db:seed RAILS_ENV=%s DISABLE_DATABASE_ENVIRONMENT_CHECK=%d", STR_FOLDER, PATH_BUNDLE, PATH_RAKE, ENV, FORCE_SEED);
    // >= Rails v5.0
    // sprintf(STR_COMMAND, "cd %s; %s exec %s db:seed STEP=%s RAILS_ENV=%s DISABLE_DATABASE_ENVIRONMENT_CHECK=%s", STR_FOLDER, PATH_BUNDLE, PATH_RAILS, ENV, FORCE_SEED);

    // NOTE: Specific seed database class
    // bundle exec rake db:seed SEED_FILES=[class_seed_name] RAILS_ENV=[environment]
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_migration_rollback()
{
    select_env();
    char STR_DESCRIPTION[300] = "Migration Rollback";
    char STR_SERVICE[300]     = "Running Rollback Migration...";
    char STR_COMMAND[1024];
    int ROLLBACK = 1;  // Number of Rollback Step

    // Goto Root App
    // Running rollback migration in the newest 'release' folder
    sprintf(STR_FOLDER, "%s/%s/%s", APP_ROOT, APP_RELEASE, SNAP_FOLDER);

    // < Rails v5.0 
    sprintf(STR_COMMAND, "cd %s; %s exec %s db:rollback STEP=%d RAILS_ENV=%s", STR_FOLDER, PATH_BUNDLE, PATH_RAKE, ROLLBACK, ENV);
    // >= Rails v5.0
    // sprintf(STR_COMMAND, "cd %s; %s exec %s db:rollback STEP=%s RAILS_ENV=%s", STR_FOLDER, PATH_BUNDLE, PATH_RAILS, ROLLBACK, ENV);

    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

/* --------------------------------------- 
        Preinstall Script
   --------------------------------------- */
void run_preinstall()
{
    select_env();
    char STR_DESCRIPTION[300] = "Preinstallation";
    char STR_SERVICE[300] = "Running Preinstall Configuration...";
    char STR_COMMAND[1024];
    // Goto Root App
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
void server_up()
{
    header();
    restart_unicorn_process();
    restart_faye_process();
    restart_pushr_process();
    restart_sidekiq_process();
    restart_mongodb_process();
    footer();
}

/* --------------------------------------- 
        Shutdown Server
   --------------------------------------- */
void server_down()
{
    header();
    kill_unicorn();
    kill_faye();
    kill_pushr();
    kill_sidekiq();
    kill_mongodb();
    footer();
}

/* --------------------------------------- 
        Deploy
   --------------------------------------- */
void deploy()
{
    header();
    git_clone();
    change_branch();
    install_bundle();
    install_package();
    //initialize_shared_folder();
    //initialize_shared_files();
    initialize_current();
    // run_migration();
    // run_seed();
    run_preinstall();

    /* --------------------------------------- 
            Server Up
       --------------------------------------- */
    // restart_unicorn_process();
    // restart_faye_process();
    // restart_pushr_process();
    // restart_sidekiq_process();
    // restart_mongodb_process();
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
    } else if (strcmp(argv[1], "-ap") == 0) {
        asset_precompile();
    } else if (strcmp(argv[1], "-ac") == 0) {
        asset_rollback(); 

    // Restart Services
    } else if (strcmp(argv[1], "-ru") == 0) {
        restart_unicorn();
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

    // View Log
    } else if (strcmp(argv[1], "-le-nginx") == 0) {
        log_nginx_error();
    } else if (strcmp(argv[1], "-la-nginx") == 0) {
        log_nginx_access();
    } else if (strcmp(argv[1], "-l-mongodb") == 0) {
        log_mongodb();
    } else if (strcmp(argv[1], "-l-memcached") == 0) {
        log_memcached();
    } else if (strcmp(argv[1], "-l-redis") == 0) {
        log_redis();    
    } else if (strcmp(argv[1], "-l-pushr") == 0) {
        log_pusher();
    } else if (strcmp(argv[1], "-l-sidekiq") == 0){
        log_sidekiq();
    } else if (strcmp(argv[1], "-l-unicorn") == 0){
        log_unicorn();    

    // Server
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
