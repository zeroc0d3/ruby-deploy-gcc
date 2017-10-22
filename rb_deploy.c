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

char* SHARED_FOLDERS[] = {
    "log", 
    "tmp/pids", 
    "tmp/cache", 
    "tmp/sockets", 
    "vendor/bundle",
    "public/uploads", 
    "public/system", 
    "public/assets"
};

char* SHARED_FILES[] = {
    "config/application.yml",
    "config/database.yml",
    "config/mongoid.yml",
    "config/secrets.yml",
    "config/sidekiq.yml"
};

/* ======================================= 
        CONFIGURATION 
   ======================================= */

int NUM_RELEASE = 5;
char ENV[64] = "development";       // Selected Environment (development / production)
char APP_ROOT[512];                 // Root Path
char APP_CURRENT[64] = "current";   // Current Folder
char APP_RELEASE[64] = "release";   // Release Folder
char APP_SHARED[64]  = "shared";    // Shared Folder
char CONFIG_UNICORN[512];           // Unicorn Config
char CONFIG_FAYE[512];              // Faye Config
char PID_UNICORN[512];              // Path PID Unicorn
char PID_FAYE[512];                 // Path PID Faye
char PATH_UNICORN[512];             // Path of Unicorn Binary
char PATH_RAKE[512];                // Path of Rake Binary
char PATH_RACKUP[512];              // Path of Rackup Binary
char PATH_GEM[512];                 // Path of Gem Binary
char PATH_BUNDLE[512];              // Path of Bundle Binary

char REPO_NAME[1024] = "git@github.com:zeroc0d3/ruby-installation.git";
char REPO_BRANCH[64] = "master";

// Development Environment
char DEV_APP_ROOT[512]        = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary";                            // Development Root Path
char DEV_CONFIG_UNICORN[512]  = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/config/unicorn/staging.rb";  // Development Unicorn Config
char DEV_CONFIG_FAYE[512]     = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/faye.ru";                    // Development Faye Config
char DEV_PID_UNICORN[512]     = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/tmp/pids/unicorn.pid";       // Development Path PID Unicorn
char DEV_PID_FAYE[512]        = "/home/zeroc0d3/ZEROC0D3LAB/ruby-deploy/deploy-binary/tmp/pids/faye.pid";          // Development Path PID Faye
char DEV_PATH_UNICORN[512]    = "/home/zeroc0d3/.rbenv/shims/unicorn";                 // Development Path of Unicorn Binary
char DEV_PATH_RAKE[512]       = "/home/zeroc0d3/.rbenv/shims/rake";                    // Development Path of Rake Binary
char DEV_PATH_RACKUP[512]     = "/home/zeroc0d3/.rbenv/shims/rackup";                  // Development Path of Rackup Binary
char DEV_PATH_GEM[512]        = "/home/zeroc0d3/.rbenv/shims/gem";                     // Development Path of Gem Binary
char DEV_PATH_BUNDLE[512]     = "/home/zeroc0d3/.rbenv/shims/bundle";                  // Development Path of Bundle Binary

// Production Environment
char PROD_APP_ROOT[512]       = "/home/zeroc0d3/deploy";                               // Production Root Path
char PROD_CONFIG_UNICORN[512] = "/home/zeroc0d3/deploy/config/unicorn/production.rb";  // Production Unicorn Config
char PROD_CONFIG_FAYE[512]    = "/home/zeroc0d3/deploy/faye.ru";                       // Production Faye Config
char PROD_PID_UNICORN[512]    = "/home/zeroc0d3/deploy/tmp/pids/unicorn.pid";          // Production Path PID Unicorn
char PROD_PID_FAYE[512]       = "/home/zeroc0d3/deploy/tmp/pids/faye.pid";             // Production Path PID Faye
char PROD_PATH_UNICORN[512]   = "/home/zeroc0d3/.rbenv/shims/unicorn";                 // Production Path of Unicorn Binary
char PROD_PATH_RAKE[512]      = "/home/zeroc0d3/.rbenv/shims/rake";                    // Production Path of Rake Binary
char PROD_PATH_RACKUP[512]    = "/home/zeroc0d3/.rbenv/shims/rackup";                  // Production Path of Rackup Binary
char PROD_PATH_GEM[512]       = "/home/zeroc0d3/.rbenv/shims/gem";                     // Production Path of Gem Binary
char PROD_PATH_BUNDLE[512]    = "/home/zeroc0d3/.rbenv/shims/bundle";                  // Production Path of Bundle Binary

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
    // printf("\033[22;31m--------------------------------------------------------------------------\033[0m\n");
    // printf("\033[22;31m  __________                  _________ _______       .___________        \033[0m\n");
    // printf("\033[22;31m  \____    /___________  ____ \_   ___ \\   _  \    __| _/\_____  \  LAB  \033[0m\n");
    // printf("\033[22;31m    /     // __ \_  __ \/  _ \/    \  \//  /_\  \  / __ |   _(__  <       \033[0m\n");
    // printf("\033[22;31m   /     /\  ___/|  | \(  <_> )     \___\  \_/   \/ /_/ |  /       \      \033[0m\n");
    // printf("\033[22;31m  /_______ \___  >__|   \____/ \______  /\_____  /\____ | /______  /      \033[0m\n");
    // printf("\033[22;31m          \/   \/                     \/       \/      \/        \/       \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m  ZeroC0D3 Ruby Deploy                                                    \033[0m\n");
    printf("\033[22;34m  (c) 2017 ZeroC0D3 Team                                                  \033[0m\n");
}

void header()
{
    system("clear");
    logo();
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    get_time();
    printf("\033[22;31m# BEGIN PROCESS..... (Please Wait)  \033[0m\n");
    printf("\033[22;31m# Start at: %s  \033[0m\n", DATE_TIME);
}

void footer() {
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    get_time();
    printf("\033[22;31m# Finish at: %s  \033[0m\n", DATE_TIME);
    printf("\033[22;31m# END PROCESS.....                  \033[0m\n\n");
}

void menu()
{
    system("clear");
    logo();
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n");
    printf("\033[22;34m # ./rb_deploy -r             --> Restart NGINX                           \033[0m\n");
    printf("\033[22;34m # ./rb_deploy -o             --> Reload NGINX                            \033[0m\n");
    printf("\033[22;34m # ./rb_deploy -a             --> Assets Precompile                       \033[0m\n");
    printf("\033[22;34m # ./rb_deploy -c             --> Assets Clobber (Rollback)               \033[0m\n");
    printf("\033[22;34m # ./rb_deploy -n             --> Restart Unicorn                         \033[0m\n");
    printf("\033[22;34m # ./rb_deploy -f             --> Restart Faye                            \033[0m\n");
    printf("\033[22;34m # ./rb_deploy -up            --> Server Up                               \033[0m\n");
    printf("\033[22;34m # ./rb_deploy -down          --> Server Down                             \033[0m\n");
    printf("\033[22;34m # ./rb_deploy -deploy / -dep --> Running Deploy                          \033[0m\n");
    printf("\033[22;32m--------------------------------------------------------------------------\033[0m\n\n");
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
        sprintf(PID_UNICORN, "%s", DEV_PID_UNICORN);
        sprintf(PID_FAYE, "%s", DEV_PID_FAYE);
        sprintf(PATH_UNICORN, "%s", DEV_PATH_UNICORN);
        sprintf(PATH_RAKE, "%s", DEV_PATH_RAKE);
        sprintf(PATH_RACKUP, "%s", DEV_PATH_RACKUP);
        sprintf(PATH_GEM, "%s", DEV_PATH_GEM);
        sprintf(PATH_BUNDLE, "%s", DEV_PATH_BUNDLE);
    }
    else
    {
        sprintf(APP_ROOT, "%s", PROD_APP_ROOT);
        sprintf(CONFIG_UNICORN, "%s", PROD_CONFIG_UNICORN);
        sprintf(CONFIG_FAYE, "%s", PROD_CONFIG_FAYE);
        sprintf(PID_UNICORN, "%s", PROD_PID_UNICORN);
        sprintf(PID_FAYE, "%s", PROD_PID_FAYE);
        sprintf(PATH_UNICORN, "%s", PROD_PATH_UNICORN);
        sprintf(PATH_RAKE, "%s", PROD_PATH_RAKE);
        sprintf(PATH_RACKUP, "%s", PROD_PATH_RACKUP);
        sprintf(PATH_GEM, "%s", PROD_PATH_GEM);
        sprintf(PATH_BUNDLE, "%s", PROD_PATH_BUNDLE);
    }
}

/* --------------------------------------- 
        Running Command
   --------------------------------------- */
void run_fcmd(char STR_COMMAND[1024])
{
    sprintf(cmdRun, "%s", STR_COMMAND);
    ret = system(cmdRun);
}

void run_cmd(char STR_SERVICE[300],
             char STR_DESCRIPTION[300],
             char STR_COMMAND[1024])
{
    get_time();
    printf("\n\033[22;34m[ %s ] ### %s...       \033[0m\n\n", DATE_TIME, STR_DESCRIPTION);
    sprintf(cmdRun, "%s", STR_COMMAND);
    ret = system(cmdRun);
    if (!ret) {
        printf("\n\033[22;32m[  DONE  ] \033[0m");
        printf("\033[22;32m %s...       \033[0m\n", STR_SERVICE);
    } else {
        printf("\n\033[22;31m[ FAILED ] \033[0m");
        printf("\033[22;32m %s...       \033[0m\n", STR_SERVICE);
    }
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
        Assets
   --------------------------------------- */
void asset_precompile()
{
    select_env();
    char STR_DESCRIPTION[300] = "Precompile Assets";
    char STR_SERVICE[300]     = "Precompiling All Assets...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "%s assets:precompile RAILS_ENV=%s --trace", PATH_RAKE, ENV);
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
    sprintf(STR_COMMAND, "%s assets:clobber RAILS_ENV=%s --trace", PATH_RAKE, ENV);
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
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_unicorn()
{
    select_env();
    char STR_DESCRIPTION[300] = "Run Unicorn Service";
    char STR_SERVICE[300]     = "Unicorn Running...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "cd %s && %s -D -c %s -E %s", APP_ROOT, PATH_UNICORN, CONFIG_UNICORN, ENV);
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

/* --------------------------------------- 
        Faye
   --------------------------------------- */
void kill_faye()
{
    select_env();
    char STR_DESCRIPTION[300] = "Stop Faye Service";
    char STR_SERVICE[300]     = "Faye Terminated...";
    char STR_COMMAND[1024]    = "ps aux | grep -i faye | awk {'print $2'} | sudo xargs kill -9";
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void run_faye()
{
    select_env();
    char STR_DESCRIPTION[300] = "Run Faye Service";
    char STR_SERVICE[300]     = "Faye Running...";
    char STR_COMMAND[1024];
    sprintf(STR_COMMAND, "cd %s && RAILS_ENV=%s %s %s -E %s -o 0.0.0.0 -D -P %s", APP_ROOT, ENV, PATH_RACKUP, CONFIG_FAYE, ENV, PID_FAYE);
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
    sprintf(STR_COMMAND, "git clone %s %s", REPO_NAME, STR_FOLDER);
    //printf("%s, %s, %s", STR_FOLDER, SNAP_FOLDER, REPO_BRANCHR_COMMAND);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
}

void change_branch()
{
    select_env();
    char STR_DESCRIPTION[300] = "Setup Branch";
    char STR_SERVICE[300]     = "Changing Branch...";
    char STR_COMMAND[1024];

    sprintf(STR_COMMAND, "cd %s && git checkout %s", STR_FOLDER, REPO_BRANCH);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void install_bundle()
{
    select_env();
    char STR_DESCRIPTION[300] = "Install Bundle";
    char STR_SERVICE[300]     = "Running: `gem install bundle`...";
    char STR_COMMAND[1024];

    sprintf(STR_COMMAND, "%s install bundle", PATH_GEM);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void install_package()
{
    select_env();
    char STR_DESCRIPTION[300] = "Install Ruby Package";
    char STR_SERVICE[300]     = "Running: `bundle install`...";
    char STR_COMMAND[1024];

    sprintf(STR_COMMAND, "%s install", PATH_BUNDLE);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void initialize_shared()
{
    select_env();
    char STR_DESCRIPTION[300] = "Initialize Shared Folder";
    char STR_SERVICE[300]     = "Initializing Shared Folder...";
    char STR_COMMAND[1024];

    sprintf(STR_COMMAND, "mkdir -p %s/%s", APP_ROOT, APP_SHARED);
    run_cmd(STR_SERVICE, STR_DESCRIPTION, STR_COMMAND);
    sleep(1);
}

void initialize_current()
{
    select_env();
    char STR_DESCRIPTION[300] = "Initialize Current Folder";
    char STR_SERVICE[300]     = "Initializing Current Folder...";
    char STR_COMMAND[1024];

    sprintf(STR_COMMAND, "mkdir -p %s/%s", APP_ROOT, APP_CURRENT);
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
    initialize_shared();
    initialize_current();
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
        
    if (strcmp(argv[1], "-r") == 0) {
        nginx_restart();
    } else if (strcmp(argv[1], "-o") == 0) {
        nginx_reload();
    } else if (strcmp(argv[1], "-a") == 0) {
        asset_precompile();
    } else if (strcmp(argv[1], "-c") == 0) {
        asset_rollback(); 
    } else if (strcmp(argv[1], "-n") == 0) {
        restart_unicorn();
    } else if (strcmp(argv[1], "-f") == 0) {
        restart_faye();
    } else if (strcmp(argv[1], "-up") == 0) {
        server_up(); 
    } else if (strcmp(argv[1], "-down") == 0) {
        server_down();
    } else if ( (strcmp(argv[1], "-deploy") == 0) || (strcmp(argv[1], "-dep") == 0) ) {
        deploy();
    } else {
        menu();
    }

    return (EXIT_SUCCESS);
}
