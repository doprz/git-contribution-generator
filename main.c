/**
 * main.c - Git Contribution Generator
 *
 * Generates a Git repository with backdated commits to create
 * a contribution history pattern. Useful for testing, demonstration,
 * or reflecting work done on private/local repositories.
 *
 * Compile: gcc -o git-contrib main.c
 *   or simply: make
 */

#define _POSIX_C_SOURCE 200809L

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_CMD_LEN 1024
#define MAX_PATH_LEN 512
#define MAX_COMMITS_LIMIT 20
#define DEFAULT_MAX_COMMITS 10
#define DEFAULT_FREQUENCY 80
#define DEFAULT_DAYS_BEFORE 365
#define DEFAULT_DAYS_AFTER 0

typedef struct {
  bool no_weekends;
  int max_commits;
  int frequency;
  int days_before;
  int days_after;
  char *repository;
  char *user_name;
  char *user_email;
} config_t;

/**
 * Execute a shell command and wait for completion
 */
static int run_command(const char *cmd) {
  int ret = system(cmd);
  if (ret == -1) {
    fprintf(stderr, "Error: Failed to execute command\n");
    return -1;
  }
  return WEXITSTATUS(ret);
}

/**
 * Get random integer in range [min, max]
 */
static int rand_range(int min, int max) {
  return min + rand() % (max - min + 1);
}

/**
 * Add days to a time structure
 */
static time_t add_days(time_t base, int days) { return base + (days * 86400); }

/**
 * Check if date is a weekend (Saturday=6, Sunday=0)
 */
static bool is_weekend(struct tm *tm) {
  return tm->tm_wday == 0 || tm->tm_wday == 6;
}

/**
 * Create a commit with the specified date
 */
static int contribute(time_t commit_time) {
  char cmd[MAX_CMD_LEN];
  char date_str[64];
  char msg[128];
  struct tm *tm = localtime(&commit_time);

  // Format date for commit message and git date
  strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", tm);
  snprintf(msg, sizeof(msg), "Contribution: %s", date_str);

  // Append to README.md
  FILE *fp = fopen("README.md", "a");
  if (!fp) {
    perror("Error opening README.md");
    return -1;
  }
  fprintf(fp, "%s\n\n", msg);
  fclose(fp);

  // Git add
  if (run_command("git add .") != 0) {
    return -1;
  }

  // Git commit with backdated timestamp
  snprintf(cmd, sizeof(cmd),
           "git commit -m \"%s\" --date=\"%s\" > /dev/null 2>&1", msg,
           date_str);

  return run_command(cmd);
}

/**
 * Extract repository name from URL
 */
static void extract_repo_name(const char *repo_url, char *name, size_t len) {
  const char *start = strrchr(repo_url, '/');
  const char *end = strrchr(repo_url, '.');

  if (start && end && end > start) {
    size_t name_len = end - start - 1;
    if (name_len >= len)
      name_len = len - 1;
    strncpy(name, start + 1, name_len);
    name[name_len] = '\0';
  } else {
    strncpy(name, "repository", len - 1);
    name[len - 1] = '\0';
  }
}

/**
 * Initialize git repository with configuration
 */
static int init_repository(const config_t *cfg, const char *dir) {
  char cmd[MAX_CMD_LEN];

  printf("\n=== Initializing Repository ===\n");
  printf("Creating directory: %s\n", dir);

  // Create and enter directory
  if (mkdir(dir, 0755) != 0) {
    perror("Error creating directory");
    return -1;
  }

  if (chdir(dir) != 0) {
    perror("Error changing directory");
    return -1;
  }

  // Initialize git repo
  printf("Initializing git repository...\n");
  if (run_command("git init -b main > /dev/null 2>&1") != 0) {
    fprintf(stderr, "Error: Failed to initialize git repository\n");
    return -1;
  }

  // Set user name if provided
  if (cfg->user_name) {
    printf("Setting git user.name: %s\n", cfg->user_name);
    snprintf(cmd, sizeof(cmd), "git config user.name \"%s\"", cfg->user_name);
    run_command(cmd);
  }

  // Set user email if provided
  if (cfg->user_email) {
    printf("Setting git user.email: %s\n", cfg->user_email);
    snprintf(cmd, sizeof(cmd), "git config user.email \"%s\"", cfg->user_email);
    run_command(cmd);
  }

  printf("Repository initialized successfully\n");
  return 0;
}

/**
 * Generate commits for the specified date range
 */
static int generate_commits(const config_t *cfg) {
  time_t now = time(NULL);
  struct tm *tm_now = localtime(&now);

  // Set to 8 PM on current day
  tm_now->tm_hour = 20;
  tm_now->tm_min = 0;
  tm_now->tm_sec = 0;
  time_t start = mktime(tm_now);

  // Move back days_before
  start = add_days(start, -cfg->days_before);

  int total_days = cfg->days_before + cfg->days_after;
  int commits_made = 0;
  int days_with_commits = 0;

  printf("\n=== Starting Commit Generation ===\n");
  printf("Total days to process: %d\n", total_days);
  printf("Target frequency: %d%%\n", cfg->frequency);
  printf("Max commits per day: %d\n", cfg->max_commits);
  printf("Skip weekends: %s\n\n", cfg->no_weekends ? "yes" : "no");

  // Progress tracking
  int last_progress = -1;
  int milestone = total_days / 10; // 10% milestones
  if (milestone < 1)
    milestone = 1;

  for (int day = 0; day < total_days; day++) {
    time_t day_time = add_days(start, day);
    struct tm *tm = localtime(&day_time);

    // Show progress every 10%
    int progress = (day * 100) / total_days;
    if (progress >= last_progress + 10) {
      printf("Progress: %d%% (%d/%d days processed, %d commits so far)\n",
             progress, day, total_days, commits_made);
      fflush(stdout);
      last_progress = progress;
    }

    // Skip weekends if requested
    if (cfg->no_weekends && is_weekend(tm)) {
      continue;
    }

    // Random frequency check
    if (rand_range(0, 100) >= cfg->frequency) {
      continue;
    }

    // Random number of commits for this day
    int commits = rand_range(1, cfg->max_commits);
    days_with_commits++;

    for (int i = 0; i < commits; i++) {
      time_t commit_time = day_time + (i * 60); // Space commits by 1 minute
      if (contribute(commit_time) == 0) {
        commits_made++;
      }
    }
  }

  printf("Progress: 100%% (%d/%d days processed)\n\n", total_days, total_days);
  printf("=== Generation Complete ===\n");
  printf("Total commits created: %d\n", commits_made);
  printf("Days with commits: %d\n", days_with_commits);
  printf("Average commits per active day: %.1f\n",
         days_with_commits > 0 ? (float)commits_made / days_with_commits : 0);

  return 0;
}

/**
 * Push to remote repository if specified
 */
static int push_to_remote(const char *repository) {
  char cmd[MAX_CMD_LEN];

  printf("\n=== Pushing to Remote Repository ===\n");
  printf("Remote URL: %s\n", repository);

  printf("Adding remote origin...\n");
  snprintf(cmd, sizeof(cmd), "git remote add origin %s", repository);
  if (run_command(cmd) != 0) {
    fprintf(stderr, "Warning: Failed to add remote\n");
  }

  printf("Setting main branch...\n");
  if (run_command("git branch -M main") != 0) {
    fprintf(stderr, "Warning: Failed to rename branch\n");
  }

  printf("Pushing commits (this may take a moment)...\n");
  if (run_command("git push -u origin main") != 0) {
    fprintf(stderr, "Error: Failed to push to remote\n");
    return -1;
  }

  printf("Successfully pushed to remote!\n");
  return 0;
}

/**
 * Print usage information
 */
static void print_usage(const char *prog) {
  printf("Usage: %s [OPTIONS]\n\n", prog);
  printf("Options:\n");
  printf("  -w, --no-weekends          Do not commit on weekends\n");
  printf("  -m, --max-commits NUM      Maximum commits per day (1-20, default: "
         "10)\n");
  printf("  -f, --frequency NUM        Percentage of days to commit (0-100, "
         "default: 80)\n");
  printf("  -r, --repository URL       Remote repository URL (SSH or HTTPS)\n");
  printf("  -n, --user-name NAME       Override git user.name\n");
  printf("  -e, --user-email EMAIL     Override git user.email\n");
  printf("  -b, --days-before NUM      Days before today to start (default: "
         "365)\n");
  printf("  -a, --days-after NUM       Days after today to end (default: 0)\n");
  printf("  -h, --help                 Display this help message\n");
}

/**
 * Parse command line arguments
 */
static int parse_arguments(int argc, char *argv[], config_t *cfg) {
  static struct option long_options[] = {
      {"no-weekends", no_argument, 0, 'w'},
      {"max-commits", required_argument, 0, 'm'},
      {"frequency", required_argument, 0, 'f'},
      {"repository", required_argument, 0, 'r'},
      {"user-name", required_argument, 0, 'n'},
      {"user-email", required_argument, 0, 'e'},
      {"days-before", required_argument, 0, 'b'},
      {"days-after", required_argument, 0, 'a'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "wm:f:r:n:e:b:a:h", long_options,
                            NULL)) != -1) {
    switch (opt) {
    case 'w':
      cfg->no_weekends = true;
      break;
    case 'm':
      cfg->max_commits = atoi(optarg);
      if (cfg->max_commits < 1)
        cfg->max_commits = 1;
      if (cfg->max_commits > MAX_COMMITS_LIMIT)
        cfg->max_commits = MAX_COMMITS_LIMIT;
      break;
    case 'f':
      cfg->frequency = atoi(optarg);
      if (cfg->frequency < 0)
        cfg->frequency = 0;
      if (cfg->frequency > 100)
        cfg->frequency = 100;
      break;
    case 'r':
      cfg->repository = strdup(optarg);
      break;
    case 'n':
      cfg->user_name = strdup(optarg);
      break;
    case 'e':
      cfg->user_email = strdup(optarg);
      break;
    case 'b':
      cfg->days_before = atoi(optarg);
      if (cfg->days_before < 0) {
        fprintf(stderr, "Error: days_before must not be negative\n");
        return -1;
      }
      break;
    case 'a':
      cfg->days_after = atoi(optarg);
      if (cfg->days_after < 0) {
        fprintf(stderr, "Error: days_after must not be negative\n");
        return -1;
      }
      break;
    case 'h':
      print_usage(argv[0]);
      exit(0);
    default:
      print_usage(argv[0]);
      return -1;
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  // Initialize configuration with defaults
  config_t cfg = {.no_weekends = false,
                  .max_commits = DEFAULT_MAX_COMMITS,
                  .frequency = DEFAULT_FREQUENCY,
                  .days_before = DEFAULT_DAYS_BEFORE,
                  .days_after = DEFAULT_DAYS_AFTER,
                  .repository = NULL,
                  .user_name = NULL,
                  .user_email = NULL};

  // Print header
  printf("\n");
  printf("╔════════════════════════════════════════════╗\n");
  printf("║   Git Contribution Generator v1.0          ║\n");
  printf("║   Creating your commit history...          ║\n");
  printf("╚════════════════════════════════════════════╝\n");

  // Seed random number generator
  srand(time(NULL));

  // Parse command line arguments
  if (parse_arguments(argc, argv, &cfg) != 0) {
    return 1;
  }

  // Determine directory name
  char dir_name[MAX_PATH_LEN];
  if (cfg.repository) {
    extract_repo_name(cfg.repository, dir_name, sizeof(dir_name));
  } else {
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    strftime(dir_name, sizeof(dir_name), "repository-%Y-%m-%d-%H-%M-%S", tm);
  }

  // Initialize repository
  if (init_repository(&cfg, dir_name) != 0) {
    return 1;
  }

  // Generate commits
  if (generate_commits(&cfg) != 0) {
    return 1;
  }

  // Push to remote if specified
  if (cfg.repository) {
    if (push_to_remote(cfg.repository) != 0) {
      fprintf(stderr, "Warning: Failed to push to remote\n");
    }
  }

  // Cleanup
  free(cfg.repository);
  free(cfg.user_name);
  free(cfg.user_email);

  printf("\n");
  printf("╔════════════════════════════════════════════╗\n");
  printf("║  ✓ Repository generation completed!        ║\n");
  printf("║    Location: %-30s ║\n", dir_name);
  printf("╚════════════════════════════════════════════╝\n");
  printf("\n");

  return 0;
}
