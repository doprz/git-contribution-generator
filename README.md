# Git Contribution Generator

A lightweight C utility for generating Git repositories with customizable commit histories.
This tool creates backdated commits to populate contribution graphs, useful for testing, demonstrations, or reflecting work done in private repositories.

## Overview

Many developers work extensively on local, private, or external Git-based projects that don't appear in public contribution graphs.
This creates an incomplete picture of actual development activity and experience. This project was born from that frustration—serving both as a practical solution and an enjoyable exercise in systems programming with C.

### Features

- **Flexible date ranges** - Generate commits spanning any time period
- **Customizable patterns** - Control commit frequency and daily commit counts
- **Weekend exclusion** - Option to skip weekend commits for realistic patterns
- **Remote integration** - Automatically push to GitHub or other Git hosting services
- **Git config override** - Set custom user name and email per repository
- **Efficient implementation** - Fast execution with minimal memory footprint

## Building

### Prerequisites

- GCC or compatible C compiler
- Git (must be installed and available in PATH)
- Make (optional, but recommended)

### Using Make (Recommended)

```bash
make
```

Additional targets:
```bash
make debug      # Build with debug symbols and sanitizers
make install    # Install to /usr/local/bin (requires sudo)
make uninstall  # Remove from system installation
make clean      # Remove build artifacts and test repositories
make test       # Run quick validation test
make help       # Display all available targets
```

### Manual Compilation

```bash
gcc -O2 -Wall -Wextra -o git-contrib main.c
```

## Usage

### Basic Examples

**Generate a repository with default settings:**
```bash
./git-contrib
```
Creates 365 days of commits with 80% daily frequency and 1-10 commits per active day.

**Skip weekends for realistic patterns:**
```bash
./git-contrib --no-weekends
```

**Customize frequency and commit density:**
```bash
./git-contrib --frequency 60 --max-commits 5
```

**Push to a remote repository:**
```bash
./git-contrib --repository git@github.com:username/repo.git
```

**Override Git configuration:**
```bash
./git-contrib --user-name "Jane Doe" --user-email "jane@example.com"
```

**Custom date range (last 180 days):**
```bash
./git-contrib --days-before 180 --days-after 0
```

### Complete Example

```bash
./git-contrib \
--no-weekends \
--frequency 75 \
--max-commits 8 \
--days-before 365 \
--days-after 0 \
--repository git@github.com:username/contributions.git \
--user-name "Your Name" \
--user-email "your.email@example.com"
```

## Command Line Options

| Option | Short | Description | Default |
|--------|-------|-------------|---------|
| `--no-weekends` | `-w` | Skip commits on weekends | false |
| `--max-commits` | `-m` | Maximum commits per day (1-20) | 10 |
| `--frequency` | `-f` | Percentage of days with commits (0-100) | 80 |
| `--repository` | `-r` | Remote repository URL (SSH or HTTPS) | none |
| `--user-name` | `-n` | Override git user.name config | system default |
| `--user-email` | `-e` | Override git user.email config | system default |
| `--days-before` | `-b` | Days before today to start commits | 365 |
| `--days-after` | `-a` | Days after today for future commits | 0 |
| `--help` | `-h` | Display help message | - |

## How It Works

1. **Repository Creation** - Creates a new directory and initializes a Git repository
2. **Commit Generation** - Iterates through the specified date range, creating commits with backdated timestamps using `git commit --date`
3. **Pattern Control** - Uses randomization within specified parameters to create natural-looking contribution patterns
4. **Remote Push** - Optionally pushes the generated repository to a remote origin

Each commit appends a timestamp entry to `README.md`, creating a simple but valid repository history.

## Use Cases

- **Portfolio Development** - Demonstrate consistent coding activity
- **Testing** - Generate repositories for testing Git tools and workflows
- **Learning** - Understand Git internals and timestamp manipulation
- **Reflection** - Represent work done on private or local projects

## Technical Details

- Written in C11 with POSIX compliance
- Uses `getopt_long` for robust argument parsing
- Employs `system()` calls for Git command execution
- Random seed initialization for pattern variation
- Proper memory management and error handling

## Limitations

- Requires Git to be installed and configured
- Creates simplified commit history (single file modifications)
- Remote push requires proper Git credentials/SSH keys
- Does not modify existing repositories (always creates new ones)

## License

This project is licensed under the MIT License - see below for details.

## Contributing

Contributions, issues, and feature requests are welcome. This was primarily a learning project, but improvements are always appreciated.

## Disclaimer

This tool is intended for legitimate purposes such as testing, learning, and representing private work. Using it to misrepresent your actual development activity on professional platforms may violate terms of service. Use responsibly.
