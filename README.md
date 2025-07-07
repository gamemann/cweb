## Not Finished & Not Intended For Production Use!!!
This is a low-level web server written in C. This project is a work-in-progress and I'm using it to learn more about web servers in general. It is currently **not in a runnable state**.

At this time, there are no plans to make this **production-ready**. This may change in the future depending on how much work I put into the project. However, right now my main focus is to learn more.

## Server Modes
There are two modes that can be run.

### Raw
This mode uses raw Linux sockets to parse and send HTTP requests and responses.

### Facil.io
This mode uses the [Facil.io](https://facil.io/) web framework to parse and send HTTP requests and responses.

## Building
If you're cloning this project, make sure to clone all of its submodules which includes [JSON-C](https://github.com/json-c/json-c) (required) and [Facil.io](https://facil.io/) (not required if using raw mode).

You can do this by passing the `--recursive` flag when cloning the repository like below.

```bash
git clone --recursive https://github.com/gamemann/cweb.git
```

If you've already cloned the repository, you can execute the following command inside of the cloned repository to download the submodules.

```bash
git submodule update --init
```

### Building & Installing JSON-C
The [JSON-C](https://github.com/json-c/json-c) library is required to read the config file.

I've made some chains in the Makefile that makes it easier to build and install JSON-C.

To build JSON-C, use the following command.

```bash
make json-c
```

To install JSON-C to the system, use the following command as root (or via `sudo`).

```bash
make json-c-install
```

### Building & Installing
To build the main project, simply use the command below.

```bash
make
```

To install the project to the system (`cweb` executable), use the following command as root (or via `sudo`).

```bash
make install
```

## Command Line Usage
### CWeb
The following command line arguments are supported for `cweb`.

| Argument | Default | Description |
| -------- | ------- | ----------- |
| `-c --cfg` | `./conf.json` | The path to the runtime config file. |
| `-l --list` | - | Prints the contents of the runtime config and exits. |
| `-h --help` | - | Prints the help menu and exits. |
| `-r --log-lvl` | - | Overrides the log level runtime config value. |
| `-f --log-file` | - | Overrides the log file runtime config value. |
| `-b --bind-addr` | - | Overrides the bind address runtime config value. |
| `-p --bind-port` | - | Overrides the bind port runtime config value. |

### CWeb Stress
The following command line arguments are supported for `cweb-stress`.

*To Do...*

## Configuration
### Build Time Configuration
There are config options in the [`common/config.h`](./common/config.h) file that may be adjusted.

When values are changed in the config, you must rebuild and reinstall the tool for the changes to take effect!

### Run Time Configuration
There are config files available in a config file on disk that utilizes JSON. The default config file location is `./conf.json`. I recommend copying or renaming the [`conf.ex.json`](./conf.ex.json) file to `conf.json`.

Modifying runtime config values only requires a restart in the tool for new values to take effect.

The following runtime config options are available.

| Name | Type | Default | Description |
| ---- | ---- | ------- | ----------- |
| `log_lvl` | int | `7` | The log level (1 = fatal, 2 = error, 3 = warn, 4 = notice, 5 = info, 6 = debug, and 7 = trace). |
| `log_file` | string | `/var/log/cweb.log` | The path to the log file. |
| `bind_addr` | string | `0.0.0.0` | The address to bind the web server to. |
| `bind_port` | int | `8` | The port to bind the web server to. |

## Credits
* [Christian Deacon](https://github.com/gamemann)